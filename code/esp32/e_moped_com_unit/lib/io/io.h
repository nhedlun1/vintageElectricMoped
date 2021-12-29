#ifndef GPIO_H
#define GPIO_H
#include <stdint.h>
#include <stdbool.h>

bool io_init();

bool io_set_front_light(uint8_t state);

bool io_set_rear_light(uint8_t state);

bool io_set_vesc_state(uint8_t state);

uint8_t io_get_vesc_state(void);

uint8_t io_get_front_light_state(void);

uint8_t io_get_rear_light_state(void);

#endif /*GPIO_H*/