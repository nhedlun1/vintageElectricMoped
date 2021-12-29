#ifndef UART_H
#define UART_H

#include "common.h"

bool display_init(Vesc_data_t *data);

status_t display_start_task(void);

status_t display_get_status(void);

void display_set_page(page_t page);

page_t display_get_page(void);

bool display_is_unlocked(void);

bool display_data_available(void);

void display_get_data(char *buffer);

#endif