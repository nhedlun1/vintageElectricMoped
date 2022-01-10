#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "common.h"
#include "io.h"
#include "buzzer_driver.h"
#include "can.h"
#include "display.h"
#include "driver/gpio.h"
#include "login_code.h"

//Vesc_data struct that is filled from CAN-task and read from display-task, locked by a mutex.
Vesc_data_t vesc = {0};

//que variables for receiveing strings from the display.
static const int msg_queue_len = 5;
static const int msg_len = 50;
static QueueHandle_t display_msg_queue;
static char *data;

static bool is_unlocked = false;
static uint16_t temp_code = 0;

//Function prototypes:
void eval_in_data(char *in_data);

void init()
{
    vTaskDelay(1500 / portTICK_PERIOD_MS);
    vesc.mutex = xSemaphoreCreateMutex();

    data = (char *)malloc(msg_len);
    if (data != NULL)
    {
        display_msg_queue = xQueueCreate(msg_queue_len, sizeof(data));
    }
    else
    {
        printf("Could not allocate memory for serial data in main\n");
    }

    if (!io_init())
    {
        printf("Could not init io");
    }

    if (!buzzer_init())
    {
        printf("Could not initiate Buzzer\n");
    }

    if (!display_init(&vesc, &display_msg_queue))
    {
        printf("Could not start uart to display");
    }
    else if (display_start_task() != STATUS_OK)
    {
        printf("Could not start UART display task!\n");
    }
}

void enable_vesc()
{
    while (!io_set_vesc_state(ON))
    {
        printf("trying to start VESC\n");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    if (can_get_status() != STATUS_OK)
    {
        if (!can_init(&vesc))
        {
            printf("Could not initiate CAN task\n");
        }
        else if (can_start_task() != STATUS_OK)
        {
            printf("Could not start CAN task!\n");
        }
    }
    io_set_front_light(ON);
    io_set_rear_light(ON);
}

void app_main(void)
{
    init();

    while (1)
    {
        //if a string has been put in the que by the display-task, read it and do something about it.
        if (xQueueReceive(display_msg_queue, (void *)&data, 0) == pdTRUE)
        {
            eval_in_data(data);
        }

        if (is_unlocked)
        {
            if (io_get_vesc_state() == OFF)
            {
                enable_vesc();
            }
            else
            {
                if (vesc.current_in <= -5 && io_get_rear_light_state() != OFF)
                {
                    io_set_rear_light(BRAKE);
                }
                else if (io_get_rear_light_state() != OFF)
                {
                    io_set_rear_light(ON);
                }
            }
        }
        else
        {
            io_set_vesc_state(OFF);
            io_set_front_light(OFF);
            io_set_rear_light(OFF);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void code_add_digit(char digit)
{
    static uint8_t digit_counter = 0;

    if (digit_counter > 3 || temp_code == 0)
    {
        digit_counter = 0;
        temp_code = 0;
    }
    temp_code = ((temp_code * 10) + (digit - '0'));
    digit_counter++;
}

void eval_in_data(char *in_data)
{
    printf("in_data:%s\n", in_data);
    if (strcmp("b=light", in_data) == 0)
    {
        uint8_t light_state = io_get_front_light_state();
        set_disp(MAIN_LIGHT_BTN_DISP, light_state);
        light_state = (light_state == OFF) ? 1 : 0;
        io_set_front_light(light_state);
        io_set_rear_light(light_state);
    }
    else if (strcmp("p=main", in_data) == 0)
    {
        display_set_page(PAGE_MAIN);
    }
    else if (strcmp("p=list", in_data) == 0)
    {
        display_set_page(PAGE_LIST);
    }
    else if (strcmp("p=bat", in_data) == 0)
    {
        display_set_page(PAGE_BAT);
    }
    else if (strcmp("b=horn", in_data) == 0)
    {
        buzzer_play_sound(TUNE_HORN);
    }
    else if (strcmp("b=dim", in_data) == 0)
    {
        if (display_get_dim() == DIM_DAY)
        {
            display_dim(DIM_NIGHT);
        }
        else
        {
            display_dim(DIM_DAY);
        }
    }
    else if (strcmp("b=lock", in_data) == 0)
    {
        if (is_unlocked)
        {
            buzzer_play_sound(TUNE_LOCKING);
            is_unlocked = false;
            display_set_page(PAGE_LOGIN);
            set_disp_message("page login");
        }
    }
    else if (in_data[0] == 'b' && isdigit(in_data[2]))
    {
        code_add_digit(in_data[2]);
        set_disp_txt("t0.txt=", temp_code);
    }
    else if (strcmp("b=del", in_data) == 0)
    {
        temp_code = 0;
        set_disp_message("t0.txt=\" \"");
    }
    else if (strcmp("b=unlock", in_data) == 0)
    {
        if (temp_code == LOGIN_CODE)
        {
            temp_code = 0;
            buzzer_play_sound(TUNE_ACCESS_GRANTED);
            is_unlocked = true;
            set_disp_message("page main");
        }
        else
        {
            buzzer_play_sound(TUNE_ACCESS_DENIED);
            temp_code = 0;
            set_disp_message("t0.txt=\" \"");
        }
    }
}