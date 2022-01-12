#include "ma_filter.h"

ma_filter_int_t *ma_init_int(bool has_mean_avg)
{
    ma_filter_int_t *new_ma_filter = NULL;
    new_ma_filter = malloc(sizeof(ma_filter_int_t) + (NUM_READINGS * sizeof(int)));
    if (new_ma_filter != NULL)
    {
        new_ma_filter->median_average = has_mean_avg;
        new_ma_filter->val_counter = 0;
        new_ma_filter->full = false;
        for (int i = 0; i < NUM_READINGS; i++)
        {
            new_ma_filter->values[i] = 0;
        }
    }
    return new_ma_filter;
}

ma_filter_float_t *ma_init_float(bool has_mean_avg)
{
    ma_filter_float_t *new_ma_filter = NULL;
    new_ma_filter = malloc(sizeof(ma_filter_float_t) + (NUM_READINGS * sizeof(float)));
    if (new_ma_filter != NULL)
    {
        new_ma_filter->median_average = has_mean_avg;
        new_ma_filter->val_counter = 0;
        new_ma_filter->full = false;
        for (int i = 0; i < NUM_READINGS; i++)
        {
            new_ma_filter->values[i] = 0.0f;
        }
    }
    return new_ma_filter;
}

int ma_get_avg_int(ma_filter_int_t *filter)
{
    int devider = (filter->full) ? NUM_READINGS : filter->val_counter;
    float return_value = 0;
    if (filter->median_average && filter->full)
    {
        devider = WANTED_READINGS;
        int *temp_arr = malloc(sizeof(int) * NUM_READINGS);
        memcpy(temp_arr, filter->values, sizeof(int) * NUM_READINGS);

        for (int i = 0; i < NUM_READINGS; i++)
        {
            for (int j = i + 1; j < NUM_READINGS; j++)
            {
                if (temp_arr[i] > temp_arr[j])
                {
                    int temp = temp_arr[i];
                    temp_arr[i] = temp_arr[j];
                    temp_arr[j] = temp;
                }
            }
        }

        uint8_t start_pos = (NUM_READINGS - WANTED_READINGS) >> 1; //bit shift right instead of devide by 2.
        for (int i = start_pos; i < WANTED_READINGS + start_pos; i++)
        {
            return_value += temp_arr[i];
        }
        free(temp_arr);
    }
    else
    {
        for (int i = 0; i < NUM_READINGS; i++)
        {
            return_value += filter->values[i];
        }
    }
    return (int)((return_value / devider) + 0.5f);
}

float ma_get_avg_float(ma_filter_float_t *filter)
{
    float return_value = 0;
    int devider = (filter->full) ? NUM_READINGS : filter->val_counter;

    if (filter->median_average && filter->full)
    {
        devider = WANTED_READINGS;
        float *temp_arr = malloc(sizeof(float) * NUM_READINGS);
        memcpy(temp_arr, filter->values, (sizeof(float) * NUM_READINGS));

        for (int i = 0; i < NUM_READINGS; i++)
        {
            for (int j = i + 1; j < NUM_READINGS; j++)
            {
                if (temp_arr[i] > temp_arr[j])
                {
                    float temp = temp_arr[i];
                    temp_arr[i] = temp_arr[j];
                    temp_arr[j] = temp;
                }
            }
        }

        uint8_t start_pos = (NUM_READINGS - WANTED_READINGS) >> 1; //bit shift right instead of devide by 2.
        for (int i = start_pos; i < WANTED_READINGS + start_pos; i++)
        {
            return_value += temp_arr[i];
        }
        free(temp_arr);
    }
    else
    {
        for (int i = 0; i < NUM_READINGS; i++)
        {
            return_value += filter->values[i];
        }
    }
    return (return_value / devider);
}

void ma_add_value_int(ma_filter_int_t *filter, int value)
{
    filter->values[filter->val_counter++] = value;

    if (filter->val_counter == NUM_READINGS)
    {
        filter->full = true;
        filter->val_counter = 0;
    }
}

void ma_add_value_float(ma_filter_float_t *filter, float value)
{
    filter->values[filter->val_counter++] = value;

    if (filter->val_counter == NUM_READINGS)
    {
        filter->full = true;
        filter->val_counter = 0;
    }
}

void ma_free_filter_int(ma_filter_int_t *filter)
{
    free(filter);
}

void ma_free_filter_float(ma_filter_float_t *filter)
{
    free(filter);
}