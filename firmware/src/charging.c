#include "charging.h"
#include "sysconfig.h"
#include "debug.h"
#include "bq.h"
#include "fsc_pd_ctl.h"
#include "fsc_pd/timer.h"

static ConnectionState last_connection_state = 0;
static struct TimerObj pd_negotiation_timer;

void charging_run(bool pd_timer_expired) {
    // Determine the current input state and configure the charger accordingly

    // Check if we have an input on VAC2 (DC jack), which takes priority
    if (bq_get_ac2_present()) {
        // DC jack input present - set fixed input current limit and enable charging
        debug_printf("DC jack input present - setting fixed input current limit\n");
        bq_set_acdrv(false, true);
        bq_set_input_current_limit(sysconfig.dcInputCurrentLimit);
        bq_enable_charging();
        // TODO: what if we are in OTG mode?
        return;
    }

    // Are we attached to USB as a sink?
    if (fsc_pd_get_connection_state() == AttachedSink) {
        // Attached - determine advertised current. This could come from one of the following sources:
        // - PD contract (if any)
        // - Type C default current (if no PD contract)
        // - BC1.2 detection (if not Type C) => performed by BQ charger IC, not PD core
        uint16_t adv_current = fsc_pd_get_advertised_current();
        debug_printf("Current advertised by source: %u mA\n", adv_current);

        if (last_connection_state != AttachedSink) {
            // Start a timer to wait up to 5 seconds for PD negotiation
            TimerStart(&pd_negotiation_timer, 5000 * TICK_SCALE_TO_MS);
        }

        // Configure IBUS
        if (adv_current < 100) {
            // Not enough power - disable charging
            bq_disable_charging();
        } else {
            if (fsc_pd_policy_has_contract() || pd_timer_expired || sysconfig.pdMode == PD_OFF) {
                if (adv_current == 500 && !fsc_pd_policy_has_contract()) {
                    // No PD contract and default current. May not be a Type-C connector. Leave it up to BC1.2 detection,
                    // which automatically sets the current limit after D+/D- detection.
                    debug_printf("Enabling BC1.2 detection\n");
                    bq_set_input_current_limit(adv_current);    // Set default current first, and let BC1.2 override afterwards
                    bq_enable_bc12_detection();
                } else {
                    // PD contract or Type C negotiated - disable BC1.2 detection and use negotiated current
                    debug_printf("Set input current limit to %u mA (%s)\n", adv_current, fsc_pd_policy_has_contract() ? "PD" : "Type C");
                    bq_disable_bc12_detection();
                    bq_set_input_current_limit(adv_current);
                }
                bq_set_acdrv(true, false);
                bq_enable_charging();
                TimerDisable(&pd_negotiation_timer);
            }
        }
    } else {
        bq_disable_charging();
    }

    last_connection_state = fsc_pd_get_connection_state();
}

void charging_check_timer(void) {
    if (TimerExpired(&pd_negotiation_timer)) {
        TimerDisable(&pd_negotiation_timer);
        debug_printf("PD negotiation timer expired\n");
        charging_run(true);
    }
}
