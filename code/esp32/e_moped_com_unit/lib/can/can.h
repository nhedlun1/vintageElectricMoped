#ifndef CAN_H
#define CAN_H

#include <stdbool.h>
#include "common.h"

/**
 * @brief initialize the canbus.
 * 
 * @param data a pointer to the Vesc data struct that is used to store values read by can.
 * @return true if initialized successfully
 * @return false if it could not be initialized
 */
bool can_init(Vesc_data_t *data);

/**
 * @brief start/create the can task that listens to the canbus.
 * 
 * @return status_t STATUS_OK if initialized properly.
 */
status_t can_start_task();

/**
 * @brief getter for the can status.
 * 
 * @return status_t status of the can module.
 */
status_t can_get_status(void);

#endif /*CAN_H*/