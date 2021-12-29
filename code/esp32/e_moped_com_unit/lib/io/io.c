#include <stdint.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "io.h"
#include "common.h"

#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_10_BIT // Set duty resolution to 10 bits
#define LEDC_DUTY (1023)                // Set duty to 100%. ((2 ** 10) - 1) * 50% = 512
#define LEDC_FREQUENCY (5000)           // Frequency in Hertz. Set frequency at 5 kHz

static uint8_t vesc_state = OFF;

bool io_init()
{
    bool return_val = false;
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[MOT_ENABLE_PIN], PIN_FUNC_GPIO); //Needed to set gpio12 to a output instead of JTAG

    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = REAR_LIGTH_PIN,
        .duty = 1023, // Set duty to 0%
        .hpoint = 0};

    if ((gpio_set_direction(MOT_ENABLE_PIN, GPIO_MODE_INPUT_OUTPUT) == ESP_OK) &&
        (gpio_set_direction(FRONT_LIGTH_PIN, GPIO_MODE_INPUT_OUTPUT) == ESP_OK) &&
        (gpio_set_level(MOT_ENABLE_PIN, OFF) == ESP_OK) &&
        (gpio_set_level(FRONT_LIGTH_PIN, ON) == ESP_OK) &&
        (ledc_timer_config(&ledc_timer) == ESP_OK) &&
        (ledc_channel_config(&ledc_channel) == ESP_OK))
    {
        return_val = true;
    }
    return return_val;
}

bool io_set_front_light(uint8_t state)
{
    bool return_value = false;
    switch (state)
    {
    case ON:
        gpio_set_level(FRONT_LIGTH_PIN, 0);
        if (gpio_get_level(FRONT_LIGTH_PIN) == 0)
        {
            return_value = true;
        }
        break;

    case OFF:
        gpio_set_level(FRONT_LIGTH_PIN, 1);
        if (gpio_get_level(FRONT_LIGTH_PIN) == 1)
        {
            return_value = true;
        }
        break;
    }
    return return_value;
}

bool io_set_rear_light(uint8_t state)
{
    switch (state)
    {
    case BRAKE:
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        break;

    case OFF:
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 1023);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        break;

    default:
    case NORMAL:
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 600);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        break;
    }
    // ledc_set_freq(LEDC_MODE, LEDC_TIMER, LEDC_FREQUENCY);

    return false;
}

bool io_set_vesc_state(uint8_t state)
{
    bool return_value = false;
    switch (state)
    {
    case ON:
        vesc_state = ON;
        gpio_set_level(MOT_ENABLE_PIN, vesc_state);
        if (gpio_get_level(MOT_ENABLE_PIN) == ON)
        {
            return_value = true;
        }
        break;

    case OFF:
        vesc_state = OFF;
        gpio_set_level(MOT_ENABLE_PIN, vesc_state);
        if (gpio_get_level(MOT_ENABLE_PIN) == OFF)
        {
            return_value = true;
        }
        break;
    }
    return return_value;
}

uint8_t io_get_vesc_state(void)
{
    return vesc_state;
}