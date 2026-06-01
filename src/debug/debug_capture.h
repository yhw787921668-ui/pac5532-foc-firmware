
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

#ifndef DEBUG_CAPTURE_H
#define DEBUG_CAPTURE_H

#include "motor_struct.h"
#include "pac5xxx.h"
#include "config_debug.h"
#include "config_app.h"
#include "board_options.h"
#include "debug_dac.h"

#ifdef ENABLE_DEBUG_SAMPLE_RAM

#define MAX_DBG_NUMBER_VARIABLES         (4) // Number of variable FW can capture per time

extern int32_t Dgb_Ram_Array[DBG_ARRAY_LENGTH];
//extern int32_t* dbg_ptr[MAX_DBG_NUMBER_VARIABLES];

extern int32_t* debug_extraction_ptr1;
extern int32_t* debug_extraction_ptr2;
extern int32_t* debug_extraction_ptr3;
extern int32_t* debug_extraction_ptr4;


extern void debug_extraction_init(Struct_Motor* motor_ptr);
extern void debug_extraction_param_write(Struct_Motor* motor_ptr, fix16_t rx_data);
extern int32_t read_debug_extraction_data(uint32_t index);
extern void debug_extraction_start(Struct_Motor* motor_ptr);
extern void write_debug_extraction_num_sample(Struct_Motor* motor_ptr);
extern uint32_t read_debug_extraction_status(Struct_Motor* motor_ptr);


typedef enum
{
    DEBUG_EXTRACTION_START = 0x01,
    DEBUG_EXTRACTION_STOP = 0x02
} debug_extraction_state_e;

PAC5XXX_RAMFUNC static inline void run_debug_capture_sample(Struct_Motor* motor_ptr)
{
    static uint16_t index = 0;
    static uint16_t skip_cycles = 0;
    static uint16_t sample_ctr = 0;
    
    uint16_t num_variables;

    num_variables = motor_ptr->debug.number_variables;
    if(motor_ptr->debug.extraction_state == DEBUG_EXTRACTION_START)
    {
        //debugio1_on();
        skip_cycles++;
        if(skip_cycles > motor_ptr->debug.skip_cycles)
        {
            // for reducing time execution without for loop
            Dgb_Ram_Array[index++] = *debug_extraction_ptr1;
            if(num_variables > 1)
            {
                Dgb_Ram_Array[index++] = *debug_extraction_ptr2;
            }
            if(num_variables > 2)
            {
                Dgb_Ram_Array[index++] = *debug_extraction_ptr3;
            }  
            if(num_variables > 3)
            {
                Dgb_Ram_Array[index++] = *debug_extraction_ptr4;
            }
         
            sample_ctr++;
            if(sample_ctr == motor_ptr->debug.number_samples)
            {
                index = 0;
                sample_ctr = 0;
                motor_ptr->debug.extraction_state = DEBUG_EXTRACTION_STOP;
                motor_ptr->debug.capture_cnt++; // debug capture counter increases
            }
            skip_cycles = 0;
        }
        //debugio1_off();
    }
}
#endif // ENABLE_DEBUG_SAMPLE_RAM
#endif // DEBUG_CAPTURE_H
