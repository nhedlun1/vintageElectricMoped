#ifndef __CAN_MSG__
#define __CAN_MSG__

#include <ctime>

#define MSG_ID_1 2356 //= 0x934 = 100100110100
#define MSG_ID_2 3636 //= 0xE34 = 111000110100
#define MSG_ID_3 3892 //= 0xF34 = 111100110100
#define MSG_ID_4 4148 //= 0x1034= 0001000000110100
#define MSG_ID_5 6964 //= 0x1B34= 0001101100110100

typedef union
{
    uint64_t data;
    uint8_t byte[8];
    uint16_t word[4];
    uint32_t dword[2];
} can_msg_t;

typedef struct
{
    int id;
    time_t rx_time;
    float rpm;
    float current;
    float duty;
} can_status_msg;

typedef struct
{
    int id;
    time_t rx_time;
    float amp_hours;
    float amp_hours_charged;
} can_status_msg_2;

typedef struct
{
    int id;
    time_t rx_time;
    float watt_hours;
    float watt_hours_charged;
} can_status_msg_3;

typedef struct
{
    int id;
    time_t rx_time;
    float temp_fet;
    float temp_motor;
    float current_in;
    float pid_pos_now;
} can_status_msg_4;

typedef struct
{
    int id;
    time_t rx_time;
    float v_in; //Byte 5 and 6
    long tacho_value;
} can_status_msg_5;

#endif