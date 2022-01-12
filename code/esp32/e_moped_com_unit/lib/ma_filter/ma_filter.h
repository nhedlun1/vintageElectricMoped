#ifndef MA_FILTER_H
#define MA_FILTER_H
/**
 * @file ma_filter.h
 * @author Niklas Hedlund (nhedlund87@gmail.com)
 * @brief Moving average module, enables user to add a value to an
 * array and then read out a moving average, with or without being "median average".
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

/**
 * @brief struct that holds the values needed for the filter
 * 
 */
typedef struct
{
    uint8_t val_counter;
    bool full;
    bool median_average;
    int values[]; //flexible struct member needs no size.
} ma_filter_int_t;

/**
 * @brief struct that holds the values needed for the filter
 * 
 */
typedef struct
{
    uint8_t val_counter;
    bool full;
    bool median_average;
    float values[]; //flexible struct member needs no size.
} ma_filter_float_t;

/**
 * @brief initialize a filter struct and get a pointer to the filter.
 * 
 * @param has_median_avg true if you want the "median" of all readings
 * @return ma_filter_int_t* 
 */
ma_filter_int_t *ma_init_int(bool has_median_avg);

/**
 * @brief initialize a filter struct and get a pointer to the filter.
 * 
 * @param has_median_avg true if you want the "median" of all readings
 * @return ma_filter_int_t* 
 */
ma_filter_float_t *ma_init_float(bool has_median_avg);

/**
 * @brief get the avg from the filter
 * 
 * @param filter which filter to get the avg from.
 * @return int the average
 */
int ma_get_avg_int(ma_filter_int_t *filter);

/**
 * @brief get the avg from the filter
 * 
 * @param filter which filter to get the avg from.
 * @return int the average
 */
float ma_get_avg_float(ma_filter_float_t *filter);

/**
 * @brief add value to the filter.
 * 
 * @param filter which filter to add to
 * @param value the value to add
 */
void ma_add_value_int(ma_filter_int_t *filter, int value);

/**
 * @brief add value to the filter.
 * 
 * @param filter which filter to add to
 * @param value the value to add
 */
void ma_add_value_float(ma_filter_float_t *filter, float value);

/**
 * @brief delete or deallocate memory for the filter. 
 * 
 * @param filter which filter to delete
 */
void ma_free_filter_int(ma_filter_int_t *filter);

/**
 * @brief delete or deallocate memory for the filter. 
 * 
 * @param filter which filter to delete
 */
void ma_free_filter_float(ma_filter_float_t *filter);

#endif /*MA_FILTER_H*/