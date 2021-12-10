#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/twai.h"
#include "can.h"

#define READ_TWAI_TASK_STACK_SIZE (1024)

//Initialize configuration structures using macro initializers
twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_21, GPIO_NUM_22, TWAI_MODE_NORMAL);
twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

//Pointer to the vesc struct used to store data from vesc motor controller.
static Vesc_data_t *vesc = NULL;
status_t can_status = STATUS_UNINITIALIZED;

//Function declaration
void read_twai_task(void *pvParameters);

bool can_init(Vesc_data_t *vesc_ptr)
{
    gpio_set_direction(ONBOARD_LED_PIN, GPIO_MODE_OUTPUT);

    bool return_val = false;
    vesc = vesc_ptr;
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
    {
        return_val = true;
    }
    if (twai_start() == ESP_ERR_INVALID_STATE)
    {
        return_val = false;
    }
    return return_val;
}

status_t can_start_task(void)
{
    xTaskCreate(read_twai_task, "read_twai_task", READ_TWAI_TASK_STACK_SIZE * 4, NULL, 10, NULL);
    vTaskDelay(100 / portTICK_PERIOD_MS); //delay to give task a chance to start and change status.
    return can_status;
}

status_t can_get_status(void)
{
    return can_status;
}

void read_twai_task(void *pvParameters)
{
    twai_message_t message;
    twai_status_info_t status;
    twai_get_status_info(&status);
    static uint8_t led_state = 0;
    // static uint8_t led_state_counter = 0;
    static uint8_t error_counter = 0;

    can_status = STATUS_OK;

    while (true)
    {
        if (twai_receive(&message, pdMS_TO_TICKS(25)) == ESP_OK)
        {
            error_counter = 0;
            can_status = STATUS_OK;

            led_state = (led_state == 0) ? 1 : 0;
            gpio_set_level(ONBOARD_LED_PIN, led_state);

            //try to take mutex and write data to the vesc struct
            if (xSemaphoreTake(vesc->mutex, (TickType_t)10) == pdTRUE)
            {
                switch (message.identifier)
                {
                case MSG_ID_1:
                    vesc->erpm = (message.data[3] | (message.data[2] << 8) | (message.data[1] << 16) | (message.data[0] << 24)); // *((int32_t *)message.data);
                    vesc->current = ((message.data[4] << 8) | message.data[5]);
                    vesc->duty = ((message.data[6] << 8) | message.data[7]);
                    // printf("Status1 = erpm:% 7d  rpm:% 4d current:% 4.2f duty:% 3d%%\n", vesc->erpm, vesc->erpm / 7, (float)vesc->current / 10, vesc->duty / 10);
                    break;

                case MSG_ID_2:
                    vesc->amp_hours = (message.data[3] | (message.data[2] << 8) | (message.data[1] << 16) | (message.data[0] << 24));
                    vesc->amp_hours_charged = (message.data[7] | (message.data[6] << 8) | (message.data[5] << 16) | (message.data[4] << 24));
                    // printf("Status2 = amp_hours:% 6.3f amp_hours_charged:% 6.3f\n", vesc->amp_hours / 10000, vesc->amp_hours_charged / 10000);
                    break;

                case MSG_ID_3:
                    vesc->watt_hours = (message.data[3] | (message.data[2] << 8) | (message.data[1] << 16) | (message.data[0] << 24));
                    vesc->watt_hours_charged = (message.data[7] | (message.data[6] << 8) | (message.data[5] << 16) | (message.data[4] << 24));
                    // printf("Status3 = watt_hours:%.2f watt_hours_charged:%.2f\n", vesc->watt_hours / 10000, vesc->watt_hours_charged / 10000);
                    break;

                case MSG_ID_4:
                    vesc->temp_fet = (message.data[0] << 8) | message.data[1];
                    vesc->temp_motor = (message.data[2] << 8) | message.data[3];
                    vesc->current_in = (message.data[4] << 8) | message.data[5];
                    vesc->pid_pos_now = ((message.data[6] << 8) | message.data[7]);
                    // printf("Status4 = temp_fet:% 6.3f temp_motor: % 6.3f current_in: % 6.3f pid_pos_now: %5d\n", (float)vesc->temp_fet / 10.f, (float)vesc->temp_motor / 10.0f, (float)vesc->current_in / 10.0f, vesc->pid_pos_now / 50);
                    break;

                case MSG_ID_5:
                    vesc->voltage = ((message.data[4] << 8) | message.data[5]);
                    vesc->tacho_value = (message.data[3] | (message.data[2] << 8) | (message.data[1] << 16) | (message.data[0] << 24));
                    // printf("Status5 = voltage:%.2f tacho_value:% 10d\n", (float)vesc->voltage / 10.0f, vesc->tacho_value);
                    break;
                }

                //release the mutex so it can be used by other thread.
                xSemaphoreGive(vesc->mutex);
            }
            else
            {
                // Increment the error counter and if counter reaches 11 turn off the onboard led to show that no CAN
                // comm is active and set the status to error. Delay 10ms to give other task chance to release mutex
                error_counter++;
                if (error_counter > 10)
                {
                    led_state = 0;
                    gpio_set_level(ONBOARD_LED_PIN, led_state);
                    can_status = STATUS_ERROR;
                }
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
        }
        else //when task could not read can, delay 10ms to yeild to other thread.
        {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}
