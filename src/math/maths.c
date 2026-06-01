
/**
 * @file  maths.c
 * @brief Math support functions
 */
 
//=============================================================================
// Copyright (C) 2017 - 2023, Qorvo, Inc.
//
// THIS SOFTWARE IS SUBJECT TO A SOURCE CODE LICENSE AGREEMENT WHICH PROVIDES,
// AMONG OTHER THINGS:  (i) THAT IT CAN BE USED ONLY TO ADAPT THE LICENSEE'S
// APPLICATION TO PAC PROCESSORS SUPPLIED BY QORVO, INC.;
// (ii) THAT  IT IS PROVIDED "AS IS" WITHOUT WARRANTY;  (iii) THAT
// QORVO, INC. IS NOT LIABLE FOR ANY INDIRECT DAMAGES OR FOR DIRECT
// DAMAGES EXCEEDING US$1,500;  AND (iv) THAT IT CAN BE DISCLOSED TO AND USED
// ONLY BY CERTAIN AUTHORIZED PERSONS.
//
//=============================================================================

#include "pac5xxx.h"
#include "maths.h"


// Sort array in ascending order, smaller to bigger value
void bubble_sort_array(int32_t *sort_array, uint32_t size)
{
    volatile int32_t temp;
    volatile uint32_t i,j;

    // Sort array with max at the end
    // Sort array in ascending order, Min to Max values
    for(i = 0 ; i < size ; i++)
    {
        for(j = 0 ; j < size-i-1 ; j++)
        {
            if(sort_array[j] > sort_array[j+1])
            {
                temp = sort_array[j];
                sort_array[j] = sort_array[j+1];
                sort_array[j+1] = temp;
            }
        }
    }
}


// Sort array in descending order
void bubble_sort_array_descending(int32_t *sort_array, uint32_t size)
{
    volatile int32_t temp;
    volatile uint32_t i,j;

    // Sort array with max at the end
    // Sort array in ascending order, Min to Max values
    for(i = 0 ; i < size ; i++)
    {
        for(j = 0 ; j < size-i-1 ; j++)
        {
            if(sort_array[j] < sort_array[j+1])
            {
                temp = sort_array[j];
                sort_array[j] = sort_array[j+1];
                sort_array[j+1] = temp;
            }
        }
    }
}


void maths_calc_avg3(int32_t n1, int32_t n2, int32_t n3, int32_t *avg)
{
    int32_t temp1 = 0;

    temp1 += n1;
    temp1 += n2;
    temp1 += n3;

    *avg = fix16_mul(temp1, FIX16_1DIV3);
}

