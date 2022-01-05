#include "ma_filter.h"

ma_filter_int_t *ma_init_int(uint8_t num_readings)
{
    ma_filter_int_t *new_ma_filter = NULL;
    new_ma_filter = malloc(sizeof(ma_filter_int_t) + (num_readings * sizeof(int)));
    if (new_ma_filter != NULL)
    {
        new_ma_filter->val_counter = 0;
        new_ma_filter->num_readings = num_readings;
        new_ma_filter->full = false;
        for (int i = 0; i < new_ma_filter->num_readings; i++)
        {
            new_ma_filter->values[i] = 0;
        }
    }
    return new_ma_filter;
}

ma_filter_float_t *ma_init_float(uint8_t num_readings)
{
    ma_filter_float_t *new_ma_filter = NULL;
    new_ma_filter = malloc(sizeof(ma_filter_float_t) + (num_readings * sizeof(float)));
    if (new_ma_filter != NULL)
    {
        new_ma_filter->val_counter = 0;
        new_ma_filter->num_readings = num_readings;
        new_ma_filter->full = false;
        for (int i = 0; i < new_ma_filter->num_readings; i++)
        {
            new_ma_filter->values[i] = 0.0f;
        }
    }
    return new_ma_filter;
}

int ma_get_avg_int(ma_filter_int_t *filter)
{
    float return_value = 0;
    if (filter->mean_average && filter->full)
    {
        int *temp_arr = malloc(sizeof(int) * filter->num_readings);
        memcpy(temp_arr, filter->values, sizeof(int) * filter->num_readings);

        for (int i = 0; i < filter->num_readings; i++)
        {
            for (int j = i + 1; j < filter->num_readings; j++)
            {
                if (temp_arr[i] > temp_arr[j])
                {
                    int temp = temp_arr[i];
                    temp_arr[i] = temp_arr[j];
                    temp_arr[j] = temp;
                }
            }
        }
        free(temp_arr);
        //TODO: Add implementation for taking just "the right middle chunk" of values from the newly sorted array.
    }
    else
    {
        for (int i = 0; i < filter->num_readings; i++)
        {
            return_value += filter->values[i];
        }
    }
    int devider = (filter->full) ? filter->num_readings : filter->val_counter;
    return (int)((return_value / devider) + 0.5f);
}

float ma_get_avg_float(ma_filter_float_t *filter)
{
    float return_value = 0;
    for (int i = 0; i < filter->num_readings; i++)
    {
        return_value += filter->values[i];
        printf("value[%d]=%f\n", i, filter->values[i]);
    }

    int devider = (filter->full) ? filter->num_readings : filter->val_counter;
    return (return_value / devider);
}

void ma_add_value_int(ma_filter_int_t *filter, int value)
{
    if (filter->val_counter == filter->num_readings - 1)
    {
        filter->full = true;
        filter->val_counter = 0;
    }
    filter->values[filter->val_counter++] = value;
}

void ma_add_value_float(ma_filter_float_t *filter, float value)
{
    if (filter->val_counter == filter->num_readings - 1)
    {
        filter->full = true;
        filter->val_counter = 0;
    }
    filter->values[filter->val_counter++] = value;
}

void ma_free_filter_int(ma_filter_int_t *filter)
{
    free(filter);
}

void ma_free_filter_float(ma_filter_float_t *filter)
{
    free(filter);
}