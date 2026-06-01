
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
#include "motor_struct.h"
#include "debug_capture.h"

#if defined (ENABLE_DEBUG_SAMPLE_RAM)
#include "debug_dac.h"

int32_t Dgb_Ram_Array[DBG_ARRAY_LENGTH]; // the ram size will be consumed is N x 4 (bytes)

int32_t* debug_extraction_ptr1;
int32_t* debug_extraction_ptr2;
int32_t* debug_extraction_ptr3;
int32_t* debug_extraction_ptr4;

#define DEBUG_EXTRACT_PARAM_1           (0)
#define DEBUG_EXTRACT_PARAM_2           (1)
#define DEBUG_EXTRACT_PARAM_3           (2)
#define DEBUG_EXTRACT_PARAM_4           (3)


/**
 * @brief  This function use to initialize debug extaction
* @param  motor_ptr, pointer to motor struct
 */
void debug_extraction_init(Struct_Motor* motor_ptr)
{
    debug_extraction_ptr1 =  DAC_ARRAY_Q14[0];
    debug_extraction_ptr2 =  DAC_ARRAY_Q14[0];
    debug_extraction_ptr3 =  DAC_ARRAY_Q14[0];
    debug_extraction_ptr4 =  DAC_ARRAY_Q14[0];
    motor_ptr->debug.extraction_state = DEBUG_EXTRACTION_STOP;
    motor_ptr->debug.number_variables = 0;
    motor_ptr->debug.number_samples = 0;
    motor_ptr->debug.capture_cnt = 0;
}


/**
 * @brief  This function use to set up variable to be captured
 * @param  motor_ptr, pointer to motor struct
 * @param  slot, index of array pointer 
 * @param  index, index of DAC array
 */
void debug_extraction_param_index(Struct_Motor* motor_ptr, int32_t **slot, int8_t index)
{
    if((index > -1) && (index < PWM_DEBUG_ARRAY_LENGTH))
    {
        *slot = DAC_ARRAY_Q14[index];
        motor_ptr->debug.number_variables++;      
    }    
}

/**
 * @brief  This function use to set up variable to be captured
 * @param  motor_ptr, pointer to motor struct
 * @param  rx_data, received data
 */
void debug_extraction_param_write(Struct_Motor* motor_ptr, fix16_t rx_data)
{
    int8_t index = 0;

    motor_ptr->debug.number_variables = 0;
    // select parameter index for debug data 1
    index = (rx_data & 0xFF);
    debug_extraction_param_index(motor_ptr, &debug_extraction_ptr1, index);
     
    // select parameter index for debug data 2
    index = (rx_data & 0xFF00) >> 8;
    debug_extraction_param_index(motor_ptr, &debug_extraction_ptr2, index);

    // select parameter index for debug data 3
    index = (rx_data & 0xFF0000) >> 16;
    debug_extraction_param_index(motor_ptr, &debug_extraction_ptr3, index);

    // select parameter index for debug data 4
    index = (rx_data & 0xFF000000) >> 24;
    debug_extraction_param_index(motor_ptr, &debug_extraction_ptr4, index);
    
    // Calculate number of samples
    write_debug_extraction_num_sample(motor_ptr);

}

/**
 * @brief  This function use to read debug extraction data
 * @param  uint32_t index, data index in array
 */
int32_t read_debug_extraction_data(uint32_t index)
{
    // return data
    if(index < DBG_ARRAY_LENGTH)
    {
        return Dgb_Ram_Array[index];
    }
    else
    {
        return -1;
    }
}

/**
 * @brief  This function use to calculate number of samples could be captured
  * @param  motor_ptr pointer to motor structure
 */
void write_debug_extraction_num_sample(Struct_Motor* motor_ptr)
{
    if(motor_ptr->debug.number_variables)
    {
        motor_ptr->debug.number_samples = DBG_ARRAY_LENGTH / motor_ptr->debug.number_variables;
    }
    else
    {
        motor_ptr->debug.number_samples = 0;
    }
    //debug_dac_1a = motor_ptr->debug.number_variables;
    //debug_dac_1b = motor_ptr->debug.number_samples;
}

/**
 * @brief  This function use to start running debug extraction data
  * @param  motor_ptr pointer to motor structure
 */
void debug_extraction_start(Struct_Motor* motor_ptr)
{
    if(motor_ptr->debug.number_samples > 0)
    {
        __disable_irq();
        motor_ptr->debug.extraction_state = DEBUG_EXTRACTION_START;
        __enable_irq();
    }
}

/**
 * @brief  This function use to read debug extraction state
  * @param  motor_ptr pointer to motor structure
 */
uint32_t read_debug_extraction_status(Struct_Motor* motor_ptr)
{
    return motor_ptr->debug.extraction_state;
}

#endif // ENABLE_DEBUG_SAMPLE_RAM
