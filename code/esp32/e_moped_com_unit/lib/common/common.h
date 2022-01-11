#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

//Pins:
#define MOT_ENABLE_PIN 12
#define FRONT_LIGTH_PIN 32
#define REAR_LIGTH_PIN 33
#define BUZZER_PIN 15
#define ONBOARD_LED_PIN 2
#define UART_2_TXD_PIN 17
#define UART_2_RXD_PIN 16

//Lighting defines:
#define OFF 0
#define ON 1
#define BRAKE 2

//CAN message id's:
#define MSG_ID_1 0x9
#define MSG_ID_2 0xE
#define MSG_ID_3 0xF
#define MSG_ID_4 0x10
#define MSG_ID_5 0x1B

//Buzzer sounds
#define TUNE_ACCESS_DENIED 0
#define TUNE_ACCESS_GRANTED 1
#define TUNE_WARNING 2
#define TUNE_HORN 3
#define TUNE_LOCKING 4

/**
 *Nextion display variable names:
 *<PAGE>_<VARIABLE>_DISP "value"
*/
#define MAIN_SPEED_DISP "n0"
#define MAIN_RPM_DISP "n1"
#define MAIN_BATT_VOLTS_DISP "x1"
#define MAIN_BATT_AMP_DISP "x0"
#define MAIN_MOT_TEMP_DISP "x2"
#define MAIN_FET_TEMP_DISP "x3"
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

//Display dim values
#define DIM_DAY 100
#define DIM_NIGHT 5

#define WHEEL_RADIUS 0.25f  //Diameter of rear wheel in m
#define CIRCUMFERENCE 1.57f //PI * 2 * WHEEL_RADIUS
#define MINUTES_IN_HOUR 60
#define KILOMETER 1000
#define GEAR_RATIO 16     //One turn of the rear wheel gives this many turns on motor.
#define MOTOR_POLEPAIRS 7 //14 motor poles gives 7 motor pole pairs.

typedef enum
{
    STATUS_UNINITIALIZED,
    STATUS_OK,
    STATUS_ERROR
} status_t;

typedef enum
{
    PAGE_LOGIN,
    PAGE_MAIN,
    PAGE_LIST,
    PAGE_BAT
} page_t;

typedef struct
{
    volatile int16_t duty;
    volatile int16_t current;
    volatile int32_t erpm;

    volatile float amp_hours;
    volatile float amp_hours_charged;

    volatile float watt_hours;
    volatile float watt_hours_charged;

    volatile int16_t temp_fet;
    volatile int16_t temp_motor;
    volatile int16_t current_in;
    volatile int16_t pid_pos_now;

    volatile int16_t voltage;
    volatile int32_t tacho_value;

    SemaphoreHandle_t mutex;

} Vesc_data_t;

#endif /*COMMON_H*/