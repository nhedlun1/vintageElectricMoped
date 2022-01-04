#include "buzzer_driver.h"
#include "notes.h"
#include "driver/ledc.h"
#include "common.h"

#define BUZZER_TIMER LEDC_TIMER_1
#define BUZZER_MODE LEDC_LOW_SPEED_MODE
#define BUZZER_CHANNEL LEDC_CHANNEL_1
#define BUZZER_DUTY_RES LEDC_TIMER_10_BIT // Set duty resolution to 13 bits
#define BUZZER_DUTY (512)                 // Set duty to 100%. ((2 ** 10) - 1) * 50% = 512
#define BUZZER_FREQUENCY (5000)           // Frequency in Hertz. Set frequency at 5 kHz
#define PLAY_SONG_TASK_STACK_SIZE (1024)

#define VOLUME_LOW 10
#define VOLUME_MED 30
#define VOLUME_HIGH 512

static bool playing_tone = false;
static uint8_t g_sound_to_play = 0;

bool buzzer_init()
{
    bool return_val = false;
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[BUZZER_PIN], PIN_FUNC_GPIO); //Needed to set gpio12 to a output instead of JTAG

    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode = BUZZER_MODE,
        .timer_num = BUZZER_TIMER,
        .duty_resolution = BUZZER_DUTY_RES,
        .freq_hz = BUZZER_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK};

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode = BUZZER_MODE,
        .channel = BUZZER_CHANNEL,
        .timer_sel = BUZZER_TIMER,
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

void buzzer_play_sound_task(void *pvParameters)
{
    switch (g_sound_to_play)
    {
    case TUNE_ACCESS_DENIED:
        ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, VOLUME_LOW);
        ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);
        ledc_set_freq(BUZZER_MODE, BUZZER_TIMER, NOTE_G3);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, 0);
        ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);
        break;
    case TUNE_ACCESS_GRANTED:
        ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, VOLUME_LOW);
        ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);
        ledc_set_freq(BUZZER_MODE, BUZZER_TIMER, NOTE_CS6);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        ledc_set_freq(BUZZER_MODE, BUZZER_TIMER, NOTE_C6);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        ledc_set_freq(BUZZER_MODE, BUZZER_TIMER, NOTE_DS6);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, 0);
        ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);
        break;

    case TUNE_WARNING:
        ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, VOLUME_HIGH);
        ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);
        ledc_set_freq(BUZZER_MODE, BUZZER_TIMER, NOTE_C7);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ledc_set_freq(BUZZER_MODE, BUZZER_TIMER, NOTE_C8);
        ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, 0);
        ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);
        break;

    case TUNE_HORN:
        ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, VOLUME_HIGH);
        ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);
        ledc_set_freq(BUZZER_MODE, BUZZER_TIMER, NOTE_G3);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, 0);
        ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);
        break;

    case TUNE_LOCKING:
        ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, VOLUME_LOW);
        ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);
        ledc_set_freq(BUZZER_MODE, BUZZER_TIMER, NOTE_B6);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        ledc_set_freq(BUZZER_MODE, BUZZER_TIMER, NOTE_C6);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        ledc_set_freq(BUZZER_MODE, BUZZER_TIMER, NOTE_GS5);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, 0);
        ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);
        break;

    default:
        break;
    }
    vTaskDelete(NULL);
}

bool buzzer_play_sound(uint8_t sound)
{
    g_sound_to_play = sound;
    bool return_val = false;
    if (!playing_tone)
    {
        xTaskCreate(buzzer_play_sound_task, "buzzer_play_sound_task", PLAY_SONG_TASK_STACK_SIZE * 4, NULL, 10, NULL);
        return_val = true;
    }
    return return_val;
}