#pragma once

#include "fsc_pd/vif_macros.h"

#define PD_Port_Type 4                /* 0: C, 1: C/P, 2: P/C, 3: P, 4: DRP */
#define Type_C_State_Machine 2        /* 0: Src, 1: Snk, 2: DRP */

extern FSC_U8 PD_Specification_Revision;    // (1: 2.0, 2: 3.0) - variable instead of macro to allow modification at runtime

#define SOP_P_Capable NO
#define SOP_PP_Capable NO
#define USB_Comms_Capable NO
#define DR_Swap_To_DFP_Supported YES
#define DR_Swap_To_UFP_Supported YES
#define Unconstrained_Power YES
#define VCONN_Swap_To_On_Supported YES
#define VCONN_Swap_To_Off_Supported YES
#define Type_C_Supports_Vconn_Powered_Accessory NO
#define Type_C_Implements_Try_SRC NO
#define Type_C_Implements_Try_SNK NO
#define Rp_Value 2                          /* 0: Def 1: 1.5A 2: 3A Advertised*/
#define Type_C_Is_Debug_Target_SRC NO
#define Type_C_Is_Debug_Target_SNK NO
#define Type_C_Supports_Audio_Accessory NO
#define Type_C_Sources_VCONN NO
#define USB_Suspend_May_Be_Cleared YES

#define NUMBER_OF_SRC_PDOS_ENABLED  4

#define PDO_FIXED_VOLTAGE_STEP 50    // mV to fixed PDO voltage step (50 mV)
#define PDO_FIXED_CURRENT_STEP 10    // mA to fixed PDO current step (10 mA)
#define PDO_PPS_VOLTAGE_STEP 100     // mV to PPS voltage step (100 mV)
#define PDO_PPS_CURRENT_STEP 50      // mA to PPS current step (50 mA)

/* Source PDOs */
#define Src_PDO_Supply_Type1 0              //; 0: Fixed
#define Src_PDO_Peak_Current1 0             //; 0: 100% IOC
#define Src_PDO_Voltage1 (5000 / PDO_FIXED_VOLTAGE_STEP)     //; 5 V
#define Src_PDO_Max_Current1 (3000 / PDO_FIXED_CURRENT_STEP) //; 3 A
#define Src_PDO_Min_Voltage1 0              //; 0 V
#define Src_PDO_Max_Voltage1 0              //; 0 V
#define Src_PDO_Max_Power1 0                //; 0 W

#define Src_PDO_Supply_Type2 0              //; 0: Fixed
#define Src_PDO_Peak_Current2 0             //; 0: 100% IOC
#define Src_PDO_Voltage2 (9000 / PDO_FIXED_VOLTAGE_STEP)     //; 9 V
#define Src_PDO_Max_Current2 (3000 / PDO_FIXED_CURRENT_STEP) //; 3 A
#define Src_PDO_Min_Voltage2 0              //; 0 V
#define Src_PDO_Max_Voltage2 0              //; 0 V
#define Src_PDO_Max_Power2 0                //; 0 W

#define Src_PDO_Supply_Type3 0              //; 0: Fixed
#define Src_PDO_Peak_Current3 0             //; 0: 100% IOC
#define Src_PDO_Voltage3 (12000 / PDO_FIXED_VOLTAGE_STEP)    //; 12 V
#define Src_PDO_Max_Current3 (2500 / PDO_FIXED_CURRENT_STEP) //; 2.5 A
#define Src_PDO_Min_Voltage3 0              //; 0 V
#define Src_PDO_Max_Voltage3 0              //; 0 V
#define Src_PDO_Max_Power3 0                //; 0 W

#define Src_PDO_Supply_Type4 0              //; 0: Fixed
#define Src_PDO_Peak_Current4 0             //; 0: 100% IOC
#define Src_PDO_Voltage4 (15000 / PDO_FIXED_VOLTAGE_STEP)    //; 15 V
#define Src_PDO_Max_Current4 (2000 / PDO_FIXED_CURRENT_STEP) //; 2 A
#define Src_PDO_Min_Voltage4 0              //; 0 V
#define Src_PDO_Max_Voltage4 0              //; 0 V
#define Src_PDO_Max_Power4 0                //; 0 W

#define Src_PDO_Supply_Type5 3              //; 3: Augmented
#define Src_PDO_Peak_Current5 0             //; 0: 100% IOC
#define Src_PDO_Voltage5 0
#define Src_PDO_Max_Current5 (2000 / PDO_PPS_CURRENT_STEP)   //; 2 A
#define Src_PDO_Min_Voltage5 (3000 / PDO_PPS_VOLTAGE_STEP)   //; 3 V
#define Src_PDO_Max_Voltage5 (15000 / PDO_PPS_VOLTAGE_STEP)  //; 15 V
#define Src_PDO_Max_Power5 0                //; 0 W

#define Src_PDO_Supply_Type6 0              //; 0: Fixed
#define Src_PDO_Peak_Current6 0             //; 0: 100% IOC
#define Src_PDO_Voltage6 0
#define Src_PDO_Max_Current6 0
#define Src_PDO_Min_Voltage6 0              //; 0 V
#define Src_PDO_Max_Voltage6 0              //; 0 V
#define Src_PDO_Max_Power6 0                //; 0 W

#define Src_PDO_Supply_Type7 0              //; 0: Fixed
#define Src_PDO_Peak_Current7 0             //; 0: 100% IOC
#define Src_PDO_Voltage7 0
#define Src_PDO_Max_Current7 0
#define Src_PDO_Min_Voltage7 0              //; 0 V
#define Src_PDO_Max_Voltage7 0              //; 0 V
#define Src_PDO_Max_Power7 0                //; 0 W

/* Sink PDOs */
#define PD_Power_as_Sink 30000              //; 30 W
#define No_USB_Suspend_May_Be_Set NO
#define Higher_Capability_Set NO

#define NUMBER_OF_SNK_PDOS_ENABLED  4

#define Snk_PDO_Supply_Type1 0              //; 0: Fixed
#define Snk_PDO_Voltage1 (5000 / PDO_FIXED_VOLTAGE_STEP)    //; 5V
#define Snk_PDO_Op_Current1 (3000 / PDO_FIXED_CURRENT_STEP) //; 3A
#define Snk_PDO_Min_Voltage1 0              //; 0 V
#define Snk_PDO_Max_Voltage1 0              //; 0 V
#define Snk_PDO_Op_Power1 0                 //; 0 W

#define Snk_PDO_Supply_Type2 0              //; 0: Fixed
#define Snk_PDO_Voltage2 (9000 / PDO_FIXED_VOLTAGE_STEP)                //; 9V
#define Snk_PDO_Op_Current2 (3000 / PDO_FIXED_CURRENT_STEP) //; 3A
#define Snk_PDO_Min_Voltage2 0              //; 0 V
#define Snk_PDO_Max_Voltage2 0              //; 0 V
#define Snk_PDO_Op_Power2 0                 //; 0 W

#define Snk_PDO_Supply_Type3 0              //; 0: Fixed
#define Snk_PDO_Voltage3 (12000 / PDO_FIXED_VOLTAGE_STEP)                //; 12V
#define Snk_PDO_Op_Current3 (2500 / PDO_FIXED_CURRENT_STEP) //; 2.5A
#define Snk_PDO_Min_Voltage3 0              //; 0 V
#define Snk_PDO_Max_Voltage3 0              //; 0 V
#define Snk_PDO_Op_Power3 0                 //; 0 W

#define Snk_PDO_Supply_Type4 0              //; 0: Fixed
#define Snk_PDO_Voltage4 (15000 / PDO_FIXED_VOLTAGE_STEP)                //; 15V
#define Snk_PDO_Op_Current4 (2000 / PDO_FIXED_CURRENT_STEP) //; 2A
#define Snk_PDO_Min_Voltage4 0              //; 0 V
#define Snk_PDO_Max_Voltage4 0              //; 0 V
#define Snk_PDO_Op_Power4 0                 //; 0 W

#define Snk_PDO_Supply_Type5 0              //; 0: Fixed
#define Snk_PDO_Voltage5 0
#define Snk_PDO_Op_Current5 0
#define Snk_PDO_Min_Voltage5 0              //; 0 V
#define Snk_PDO_Max_Voltage5 0              //; 0 V
#define Snk_PDO_Op_Power5 0                 //; 0 W

#define Snk_PDO_Supply_Type6 0              //; 0: Fixed
#define Snk_PDO_Voltage6 0
#define Snk_PDO_Op_Current6 0
#define Snk_PDO_Min_Voltage6 0              //; 0 V
#define Snk_PDO_Max_Voltage6 0              //; 0 V
#define Snk_PDO_Op_Power6 0                 //; 0 W

#define Snk_PDO_Supply_Type7 0              //; 0: Fixed
#define Snk_PDO_Voltage7 0
#define Snk_PDO_Op_Current7 0
#define Snk_PDO_Min_Voltage7 0              //; 0 V
#define Snk_PDO_Max_Voltage7 0              //; 0 V
#define Snk_PDO_Op_Power7 0                 //; 0 W

/* Dual Role settings */
#define Accepts_PR_Swap_As_Src YES
#define Accepts_PR_Swap_As_Snk YES
#define Requests_PR_Swap_As_Src NO
#define Requests_PR_Swap_As_Snk NO

/* Project related defines */
#define Attempt_DR_Swap_to_Ufp_As_Src        NO
#define Attempt_DR_Swap_to_Dfp_As_Sink       NO
#define Attempt_Vconn_Swap_to_Off_As_Src     NO
#define Attempt_Vconn_Swap_to_On_As_Sink     NO
#define Attempts_DiscvId_SOP_P_First         NO
