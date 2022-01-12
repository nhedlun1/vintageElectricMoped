#ifndef GPIO_H
#define GPIO_H
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief initialize the io module
 * 
 * @return true if success
 * @return false if failed
 */
bool io_init();

/**
 * @brief set the state of the front light
 * 
 * @param state the state to set use macros ON or OFF
 * @return true if success 
 * @return false if no success
 */
bool io_set_front_light(uint8_t state);

/**
 * @brief set the state of the rear light
 * 
 * @param state the state to set use macros ON,OFF or BRAKE
 * @return true if success 
 * @return false if no success
 */
bool io_set_rear_light(uint8_t state);

/**
 * @brief enable the vesc
 * 
 * @param state the state to set use macros ON or OFF
 * @return true 
 * @return false 
 */
bool io_set_vesc_state(uint8_t state);

/**
 * @brief get get vesc state
 * 
 * @return uint8_t the state
 */
uint8_t io_get_vesc_state(void);

/**
 * @brief get the state of the front light
 * 
 * @return uint8_t the state
 */
uint8_t io_get_front_light_state(void);

/**
 * @brief get the state of the rear light
 * 
 * @return uint8_t the state
 */
uint8_t io_get_rear_light_state(void);

/**
 * @brief set the state of the onboard led
 * 
 * @param state the state to set use macros ON or OFF
 * @return true if success
 * @return false if no success
 */
bool io_set_led_state(uint8_t state);

#endif /*GPIO_H*/