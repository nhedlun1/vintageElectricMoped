#ifndef BUZZER_DRIVER_H
#define BUZZER_DRIVER_H
#include <stdint.h>
#include <stdbool.h>

bool buzzer_init();

bool buzzer_play_sound(uint8_t sound);

#endif /*LIGHTS_DRIVER_H*/