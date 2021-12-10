#ifndef UART_H
#define UART_H

#include "common.h"

bool display_init(Vesc_data_t *data);

status_t display_start_task();

status_t display_get_status(void);

#endif