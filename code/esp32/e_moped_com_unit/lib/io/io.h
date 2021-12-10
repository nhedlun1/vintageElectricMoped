#ifndef GPIO_H
#define GPIO_H
#include <stdint.h>
#include <stdbool.h>

bool io_init();

bool io_set_front_light(uint8_t state);

bool io_set_rear_light(uint8_t state);

bool io_enable_vesc(uint8_t state);

#endif /*GPIO_H*/