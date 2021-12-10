#ifndef CAN_H
#define CAN_H

#include <stdbool.h>
#include "common.h"

bool can_init(Vesc_data_t *data);

status_t can_start_task();

status_t can_get_status(void);

#endif /*CAN_H*/