#include "buzzer_driver.h"
#include "notes.h"
#include "driver/ledc.h"
#include "common.h"

#define LEDC_TIMER LEDC_TIMER_1
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_1
#define LEDC_DUTY_RES LEDC_TIMER_10_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY (512)                 // Set duty to 100%. ((2 ** 10) - 1) * 50% = 512
#define LEDC_FREQUENCY (5000)           // Frequency in Hertz. Set frequency at 5 kHz

bool buzzer_init()
{
    bool return_val = false;
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[BUZZER_PIN], PIN_FUNC_GPIO); //Needed to set gpio12 to a output instead of JTAG

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
        .gpio_num = BUZZER_PIN,
        .duty = 0, // Set duty to 0%
        .hpoint = 0};

    if ((ledc_timer_config(&ledc_timer) == ESP_OK) &&
        (ledc_channel_config(&ledc_channel) == ESP_OK))
    {
        return_val = true;
    }
    return return_val;
}

bool buzzer_play_sound(uint8_t sound)
{
    switch (sound)
    {
    case 1:
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY);
        ledc_set_freq(LEDC_MODE, LEDC_TIMER, NOTE_G6);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        break;

    case 0:
    default:
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
        ledc_set_freq(LEDC_MODE, LEDC_TIMER, LEDC_FREQUENCY);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
        break;
    }
    return false;
}