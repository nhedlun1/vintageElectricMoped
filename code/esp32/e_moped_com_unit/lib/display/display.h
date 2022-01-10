#ifndef UART_H
#define UART_H

#include "common.h"

bool display_init(Vesc_data_t *data, QueueHandle_t *display_msg_queue);

status_t display_start_task(void);

status_t display_get_status(void);

void display_set_page(page_t page);

page_t display_get_page(void);

void set_disp(const char *var, int msg);

void set_disp_message(const char *string);

void set_disp_txt(const char *string, int val);

void display_dim(uint8_t value);

uint8_t display_get_dim(void);

#endif