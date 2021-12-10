#include <stdio.h>
#include "can.h"
#include "common.h"
#include "io.h"
#include "display.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

//Vesc_data struct that data is read to and from using ptr
Vesc_data_t vesc = {0};

void setup()
{
    vTaskDelay(1500 / portTICK_PERIOD_MS);
    vesc.mutex = xSemaphoreCreateMutex();
    io_init();
    display_init(&vesc);

    while (!io_enable_vesc(ON))
    {
        printf("trying to start VESC\n");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    if (!can_init(&vesc))
    {
        printf("Could not initiate CAN task\n");
    }
    else if (can_start_task() == STATUS_OK)
    {
        printf("Started CAN task successfully!\n");
    }
    else
    {
        printf("Could not start CAN task!\n");
    }
    display_start_task();
}

void app_main(void)
{
    setup();

    while (1)
    {
        if (vesc.voltage > 125)
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