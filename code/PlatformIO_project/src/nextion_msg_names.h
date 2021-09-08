#ifndef __NEXTION_MSG_NAMES__
#define __NEXTION_MSG_NAMES__

/**
 *Nextion display variable names
 *<PAGE>_<VARIABLE>_<DISP>
*/

#define MAIN_SPEED_DISP "n0"
#define MAIN_RPM_DISP "n1"
#define MAIN_BATT_VOLTS_DISP "x1"
#define MAIN_BATT_AMP_DISP "x0"
#define MAIN_FET_TEMP_DISP "x2"
#define MAIN_LIGHT_BTN_DISP "b0"

#define LIST_DUTY_DISP "n0"
#define LIST_MOTOR_AMP_DISP "x0"
#define LIST_ERPM_DISP "n1"

#define LIST_AMPS_USED_DISP "x1"
#define LIST_AMPS_CHARGED_DISP "x2"
#define LIST_WATTS_USED_DISP "x3"
#define LIST_WATTS_CHARGED_DISP "x4"

#define LIST_TEMP_FETS_DISP "x5"
#define LIST_TEMP_MOTOR_DISP "x6"
#define LIST_BATT_AMPS_DISP "x7"
#define LIST_PID_POS_DISP "n2"

#define LIST_BATT_VOLT_DISP "x8"
#define LIST_TACH_VAL_DISP "n3"

#define MAIN 0
#define LIST 1
#define BATT 2

#endif