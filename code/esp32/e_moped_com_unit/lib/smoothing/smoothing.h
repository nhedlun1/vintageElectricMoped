#ifndef SMOOTHING_H
#define SMOOTHING_H
/**
 * @file smoothing.h
 * @author Niklas Hedlund (nhedlund87@gmail.com)
 * @brief A simple smoothing module for smoothing out integer and float readings,
 * Note: Not tested properly, use at own risk!
 * @version 0.1
 * @date 2022-01-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

typedef struct
{
    int value;
    float coeff;
} sm_int_t;

typedef struct
{
    float value;
    float coeff;
} sm_float_t;

/**
 * @brief initialize a smothing filter that handles integers.
 * 
 * @param coeff which coefficent to use, 1.0f=no smoothing 0.5f=half smoothing etc.
 * @return sm_int_t* a pointer to the allocated memory
 */
sm_int_t *smooth_init_int(float coeff);

/**
 * @brief initialize a smothing filter that handles floats.
 * 
 * @param coeff which coefficent to use, 1.0f=no smoothing 0.5f=half smoothing etc.
 * @return sm_int_t* a pointer to the allocated memory
 */
sm_float_t *smooth_init_float(float coeff);

/**
 * @brief add a value to the smoothing filter and get 
 * the smoothed value back.
 * 
 * @param ptr pointer to which smoothing filter to use
 * @param input the new value
 * @return int the smoothed output
 */
int smooth_int(sm_int_t *ptr, int input);

/**
 * @brief add a value to the smoothing filter and get 
 * the smoothed value back.
 * 
 * @param ptr pointer to which smoothing filter to use
 * @param input the new value
 * @return float the smoothed output
 */
float smooth_float(sm_float_t *ptr, float input);

/**
 * @brief deallocates or free's the memory that is allocated
 * for the smoothing filter.
 * 
 * @param ptr the filter to free or delete.
 */
void smooth_free_int(sm_int_t *ptr);

/**
 * @brief deallocates or free's the memory that is allocated
 * for the smoothing filter.
 * 
 * @param ptr the filter to free or delete.
 */
void smooth_free_float(sm_float_t *ptr);

#endif /*SMOOTHING_H*/