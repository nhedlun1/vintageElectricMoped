#include <stdint.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "io.h"
#include "common.h"

#define REAR_LIGHT_TIMER LEDC_TIMER_0
#define REAR_LIGHT_TIMER_MODE LEDC_LOW_SPEED_MODE
#define REAR_LIGHT_TIMER_CHANNEL LEDC_CHANNEL_0
#define REAR_LIGHT_TIMER_DUTY_RES LEDC_TIMER_10_BIT // Set duty resolution to 10 bits
#define REAR_LIGHT_TIMER_DUTY (1024)                // Set duty to 100%. ((2 ** 10) - 1) * 50% = 512
#define REAR_LIGHT_TIMER_FREQUENCY (5000)           // Frequency in Hertz. Set frequency at 5 kHz

#define REAR_LIGHT_BRAKE_LVL 0
#define REAR_LIGHT_OFF_LVL 1024
#define REAR_LIGHT_ON_LVL 750

static uint8_t vesc_state = OFF;
static uint8_t front_light_state = OFF;
static uint8_t rear_light_state = OFF;
static uint8_t led_state = OFF;

bool io_init()
{
    bool return_val = false;
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[MOT_ENABLE_PIN], PIN_FUNC_GPIO); //Needed to set gpio12 to a output instead of JTAG

    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode = REAR_LIGHT_TIMER_MODE,
        .timer_num = REAR_LIGHT_TIMER,
        .duty_resolution = REAR_LIGHT_TIMER_DUTY_RES,
        .freq_hz = REAR_LIGHT_TIMER_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode = REAR_LIGHT_TIMER_MODE,
        .channel = REAR_LIGHT_TIMER_CHANNEL,
        .timer_sel = REAR_LIGHT_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = REAR_LIGTH_PIN,
        .duty = 1023, // Set duty to 0%
        .hpoint = 0};

    //Check so io's are configured successfully
    if ((gpio_set_direction(MOT_ENABLE_PIN, GPIO_MODE_INPUT_OUTPUT) == ESP_OK) &&
        (gpio_set_direction(FRONT_LIGTH_PIN, GPIO_MODE_INPUT_OUTPUT) == ESP_OK) &&
        (gpio_set_direction(ONBOARD_LED_PIN, GPIO_MODE_INPUT_OUTPUT) == ESP_OK) &&
        (ledc_timer_config(&ledc_timer) == ESP_OK) &&
        (ledc_channel_config(&ledc_channel) == ESP_OK))
    {
        return_val = true;
    }

    io_set_front_light(OFF);
    io_set_rear_light(OFF);
    io_set_led_state(OFF);

    return return_val;
}

bool io_set_front_light(uint8_t state)
{
    front_light_state = state;
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
    rear_light_state = state;
    switch (state)
    {
    case BRAKE:
        ledc_set_duty(REAR_LIGHT_TIMER_MODE, REAR_LIGHT_TIMER_CHANNEL, REAR_LIGHT_BRAKE_LVL);
        ledc_update_duty(REAR_LIGHT_TIMER_MODE, REAR_LIGHT_TIMER_CHANNEL);
        break;

    case OFF:
        ledc_set_duty(REAR_LIGHT_TIMER_MODE, REAR_LIGHT_TIMER_CHANNEL, REAR_LIGHT_OFF_LVL);
        ledc_update_duty(REAR_LIGHT_TIMER_MODE, REAR_LIGHT_TIMER_CHANNEL);
        break;

    default:
    case ON:
        ledc_set_duty(REAR_LIGHT_TIMER_MODE, REAR_LIGHT_TIMER_CHANNEL, REAR_LIGHT_ON_LVL);
        ledc_update_duty(REAR_LIGHT_TIMER_MODE, REAR_LIGHT_TIMER_CHANNEL);
        break;
    }
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
    vesc_state = gpio_get_level(MOT_ENABLE_PIN);
    return vesc_state;
}

uint8_t io_get_front_light_state(void)
{
    return front_light_state;
}

uint8_t io_get_rear_light_state(void)
{
    return rear_light_state;
}

bool io_set_led_state(uint8_t state)
{
    led_state = state;
    bool return_value = false;
    switch (state)
    {
    case ON:
        gpio_set_level(ONBOARD_LED_PIN, ON);
        if (gpio_get_level(ONBOARD_LED_PIN) == 0)
        {
            return_value = true;
        }
        break;

    case OFF:
        gpio_set_level(ONBOARD_LED_PIN, OFF);
        if (gpio_get_level(ONBOARD_LED_PIN) == 1)
        {
            return_value = true;
        }
        break;
    }
    return return_value;
}
