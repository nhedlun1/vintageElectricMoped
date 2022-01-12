#include "smoothing.h"
#include <stdlib.h>

sm_int_t *smooth_init_int(float coeff)
{
    sm_int_t *new_sm = NULL;
    new_sm = malloc(sizeof(sm_int_t));
    if (new_sm != NULL)
    {
        new_sm->coeff = coeff;
        new_sm->value = 0;
    }
    return new_sm;
}

sm_float_t *smooth_init_float(float coeff)
{
    sm_float_t *new_sm = NULL;
    new_sm = malloc(sizeof(sm_float_t));
    if (new_sm != NULL)
    {
        new_sm->coeff = coeff;
        new_sm->value = 0.0f;
    }
    return new_sm;
}

int smooth_int(sm_int_t *ptr, int input)
{
    float diff = input - ptr->value;
    ptr->value += ptr->coeff * diff;
    return (int)ptr->value + 0.5f;
}

float smooth_float(sm_float_t *ptr, float input)
{
    float diff = input - ptr->value;
    ptr->value += ptr->coeff * diff;
    return ptr->value;
}

void smooth_free_int(sm_int_t *ptr)
{
    free(ptr);
}

void smooth_free_float(sm_float_t *ptr)
{
    free(ptr);
}