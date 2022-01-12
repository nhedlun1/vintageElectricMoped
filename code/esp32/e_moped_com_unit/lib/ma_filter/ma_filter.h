#ifndef MA_FILTER_H
#define MA_FILTER_H
/**
 * @file ma_filter.h
 * @author Niklas Hedlund (nhedlund87@gmail.com)
 * @brief Moving average module, enables user to add a value to an
 * array and then read out a moving average, with or without being "mean average".
 * @version 0.1
 * @date 2022-01-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_READINGS 20
#define WANTED_READINGS 10

typedef struct
{
    uint8_t val_counter;
    bool full;
    bool mean_average;
    int values[]; //flexible struct member needs no size.
} ma_filter_int_t;

typedef struct
{
    uint8_t val_counter;
    bool full;
    bool mean_average;
    float values[]; //flexible struct member needs no size.
} ma_filter_float_t;

ma_filter_int_t *ma_init_int(bool has_mean_avg);

ma_filter_float_t *ma_init_float(bool has_mean_avg);

int ma_get_avg_int(ma_filter_int_t *filter);

float ma_get_avg_float(ma_filter_float_t *filter);

void ma_add_value_int(ma_filter_int_t *filter, int value);

void ma_add_value_float(ma_filter_float_t *filter, float value);

void ma_free_filter_int(ma_filter_int_t *filter);

void ma_free_filter_float(ma_filter_float_t *filter);

#endif /*MA_FILTER_H*/