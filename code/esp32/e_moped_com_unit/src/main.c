#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "common.h"
#include "io.h"
#include "buzzer_driver.h"
#include "can.h"
#include "display.h"
#include "driver/gpio.h"

//Vesc_data struct that is filled from CAN-task and read from display-task, locked by a mutex.
Vesc_data_t vesc = {0};

void init()
{
    vTaskDelay(1500 / portTICK_PERIOD_MS);
    vesc.mutex = xSemaphoreCreateMutex();

    if (!io_init())
    {
        printf("Could not init io");
    }

    if (!buzzer_init())
    {
        printf("Could not initiate Buzzer\n");
    }

    if (!display_init(&vesc))
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
}

void app_main(void)
{
    init();

    while (1)
    {
        while (display_is_unlocked())
        {
            if (io_get_vesc_state() == OFF)
            {
                enable_vesc();
            }
            else
            {
                if (vesc.current_in <= -5)
                {
                    io_set_rear_light(BRAKE);
                }
                else
                {
                    io_set_rear_light(NORMAL);
                }
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }

        io_set_vesc_state(OFF);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}