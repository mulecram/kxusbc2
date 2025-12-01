#include "twi.h"

#include <avr/io.h>
#include <stdbool.h>

#define TWI_READ true
#define TWI_WRITE false

#define TWI_IS_CLOCKHELD() TWI0.MSTATUS & TWI_CLKHOLD_bm
#define TWI_IS_BUSERR() TWI0.MSTATUS & TWI_BUSERR_bm
#define TWI_IS_ARBLOST() TWI0.MSTATUS & TWI_ARBLOST_bm
#define CLIENT_NACK() TWI0.MSTATUS & TWI_RXACK_bm
#define TWI_IS_BUSBUSY() ((TWI0.MSTATUS & TWI_BUSSTATE_BUSY_gc) == TWI_BUSSTATE_BUSY_gc)
#define TWI_WAIT() while (!((TWI_IS_CLOCKHELD()) || (TWI_IS_BUSERR()) || (TWI_IS_ARBLOST()) || (TWI_IS_BUSBUSY())))
#define TWI_STOP() {TWI0.MCTRLB |= TWI_MCMD_STOP_gc; while ((TWI0.MSTATUS & TWI_BUSSTATE_gm) != TWI_BUSSTATE_IDLE_gc); }

static bool twi_is_bad(void);
static bool twi_start(uint8_t addr, bool read);
static void twi_read_from(uint8_t* data, uint8_t len);
static bool twi_write_to(uint8_t* data, uint8_t len);

static bool twi_is_bad(void) {
    if (((TWI0.MSTATUS & (TWI_RXACK_bm | TWI_ARBLOST_bm | TWI_BUSERR_bm)) != 0) || (TWI_IS_BUSBUSY())) {
        return true;
    }
    return false;
}

void twi_init(void) {        
    // Configure pins for output
    PORTB.DIRSET = PIN1_bm | PIN0_bm;
    
    // 400kHz TWI, 4 cycle SDA setup, 50ns SDA Hold Time
    TWI0.CTRLA = TWI_SDAHOLD_50NS_gc;
      
    // Clear MSTATUS (write 1 to flags). BUSSTATE set to idle
    TWI0.MSTATUS = TWI_RIF_bm | TWI_WIF_bm | TWI_CLKHOLD_bm | TWI_RXACK_bm |
            TWI_ARBLOST_bm | TWI_BUSERR_bm | TWI_BUSSTATE_IDLE_gc;
    
    // Set for 400kHz from a 20MHz oscillator, no CLKDIV
    TWI0.MBAUD = 20;
   
    // No ISRs, host mode
    TWI0.MCTRLA = TWI_ENABLE_bm | TWI_TIMEOUT_200US_gc;
}

static bool twi_start(uint8_t addr, bool read) {
    if (TWI_IS_BUSBUSY()){
        return false;
    }
    
    // Send address
    TWI0.MADDR = (addr << 1) | read;
    
    TWI_WAIT();
                
    if (twi_is_bad()){
        TWI_STOP();
        return false;
    }
    
    return true;
}

// Reads len bytes from TWI, then issues a bus STOP
static void twi_read_from(uint8_t* data, uint8_t len) {
    uint8_t bCount = 0;
    
    TWI0.MSTATUS = TWI_CLKHOLD_bm;
    
    while (bCount < len) {
        TWI_WAIT();
        
        data[bCount] = TWI0.MDATA;
        bCount += 1;        
        if (bCount != len) {
            // If not done, then ACK and read the next byte
            TWI0.MCTRLB = TWI_ACKACT_ACK_gc | TWI_MCMD_RECVTRANS_gc;
        }
    }
    
    // NACK and STOP the bus
    TWI0.MCTRLB = TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc;
}

// Write len bytes to TWI. Does NOT STOP the bus. Returns true if successful
static bool twi_write_to(uint8_t* data, uint8_t len) {
    uint8_t count = 0;
    
    while (count < len) {
        TWI0.MDATA = data[count];
        
        TWI_WAIT();
        
        // If the client NACKed, then abort the write
        if (CLIENT_NACK()) {
            return false;
        }
        
        count++;
    }
    
    return true;
}

bool twi_send_byte(uint8_t addr, uint8_t data) {
    if (!twi_start(addr, TWI_WRITE)) {
        TWI_STOP();
        return false;
    }
    
    bool success = twi_write_to(&data, 1);
    
    TWI0.MCTRLB = TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc;
    
    return success;
}

bool twi_send_reg_bytes(uint8_t addr, uint8_t regAddress, uint8_t* data, uint8_t len) {
    if (!twi_start(addr, TWI_WRITE)) {
        TWI_STOP();
        return false;
    }
    
    bool success = twi_write_to(&regAddress, 1);
    success &= twi_write_to(data, len);

    TWI_STOP();
        
    return success;
}

bool twi_send_bytes(uint8_t addr, uint8_t* data, uint8_t len) {
    if (!twi_start(addr, TWI_WRITE)){
        TWI_STOP();
        return false;
    }
        
    bool success = twi_write_to(data, len);

    TWI_STOP();
        
    return success;
}

bool twi_read_byte(uint8_t addr, uint8_t* data) {
    if (!twi_start(addr, TWI_READ)) {
        TWI_STOP();
        return false;
    }
        
    twi_read_from(data, 1);

    return true;
}


bool twi_read_bytes(uint8_t addr, uint8_t* data, uint8_t len){
    if (!twi_start(addr, TWI_READ)) {
        TWI_STOP();
        return false;
    }
    
    twi_read_from(data, len);
    
    return true;
}

bool twi_send_and_read_bytes(uint8_t addr, uint8_t regAddress, uint8_t* data, uint8_t len)
{
    if (!twi_start(addr, TWI_WRITE)) {
        TWI_STOP();
        return false;
    }
        
    if (!twi_write_to(&regAddress, 1)) {
        TWI_STOP();
        return false;
    }
    
    // Restart TWI in READ mode
    TWI0.MADDR |= TWI_READ;
    TWI0.MCTRLB |= TWI_MCMD_REPSTART_gc;
    
    TWI_WAIT();
    
    if (twi_is_bad()) {
        //Stop the TWI Bus if an error occurred
        TWI0.MCTRLB = TWI_MCMD_STOP_gc;
        return false;
    }
    
    twi_read_from(data, len);
    
    TWI_STOP();
    
    return true;
}
