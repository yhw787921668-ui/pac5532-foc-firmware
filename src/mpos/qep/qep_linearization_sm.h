
//=============================================================================
// Copyright (C) 2018 - 2023, Qorvo, Inc.
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

#ifndef QEP_LINEARIZATION_SM_H
#define QEP_LINEARIZATION_SM_H

#include "maths.h"
//#include "debug.h"
#include "qep_funcs.h"

#ifdef ENABLE_QEP_LINEARIZATION_TUNING
typedef enum
{
    STATE_QEP_LINEARIZATION_IDLE = 0,
    STATE_QEP_LINEARIZATION_INIT,      // 1
    STATE_QEP_LINEARIZATION_START,     // 2
    STATE_QEP_LINEARIZATION_DONE,      // 3
} eStateQEP_LINEARIZATION;

void clear_qep_linearization_array(Struct_Motor* motor_ptr)
{
    // Clear the accumulation
    uint32_t i;
    for(i = 0; i < QEP_PULSE_PER_RESOLUTION_FINAL; i++)
    {
        motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[i] = 0;
    }
}

void generate_count_diff_arrary_from_ideal_to_raw(Struct_Motor* motor_ptr)
{
    uint32_t i;
    for(i = 0; i < QEP_PULSE_PER_RESOLUTION_FINAL; i++)
    {
        motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[i] = motor_ptr->mpos.qep.qep_count_diff_between_ideal_and_raw_q06[i] >> ACCUMULATION_NUM_OF_SHIFT;
    }
}

static inline void qep_linearization_sm(Struct_Motor* motor_ptr)
{
    static volatile eStateQEP_LINEARIZATION state_qep_linearization;

    // State Machine
    switch(state_qep_linearization)
    {
        //////////////////////////////////////////////////
        // START
        //////////////////////////////////////////////////
        case STATE_QEP_LINEARIZATION_IDLE:
            if ((motor_ptr->mpos.qep.qep_linearization_enable_cmd) && (motor_ptr->state.main == STATE_MAIN_RUN))
            {
                state_qep_linearization = STATE_QEP_LINEARIZATION_INIT;
            }
            break;

        case STATE_QEP_LINEARIZATION_INIT:
            // Clear the accumulation
            clear_qep_linearization_array(motor_ptr);
            motor_ptr->mpos.qep.qep_linearization_start_flag = TRUE;
            motor_ptr->mpos.qep.qep_linearization_done_flag =  FALSE;
            state_qep_linearization = STATE_QEP_LINEARIZATION_START;
            break;

        case STATE_QEP_LINEARIZATION_START:
            if (motor_ptr->mpos.qep.index_count == (TOTAL_ACCUMULATION_CNT + 1))
            {
                state_qep_linearization = STATE_QEP_LINEARIZATION_DONE;
            }
            // If enable cmd is removed from GUI, it is reset to IDLE state
            if(!motor_ptr->mpos.qep.qep_linearization_enable_cmd)
            {
                motor_ptr->mpos.qep.qep_linearization_done_flag = FALSE;
                motor_ptr->mpos.qep.qep_linearization_start_flag = FALSE;
                state_qep_linearization = STATE_QEP_LINEARIZATION_IDLE;
                // Clear the accumulation
                clear_qep_linearization_array(motor_ptr);
            }
            break;

        case STATE_QEP_LINEARIZATION_DONE:
            generate_count_diff_arrary_from_ideal_to_raw(motor_ptr);
            motor_ptr->mpos.qep.qep_linearization_done_flag = TRUE;
            motor_ptr->mpos.qep.qep_linearization_start_flag = FALSE;
            state_qep_linearization = STATE_QEP_LINEARIZATION_IDLE;
            motor_ptr->mpos.qep.qep_linearization_enable_cmd = FALSE;
            // Update motor status
            motor.status.motor |= STATUS_QEP_LINEAR_COMPLETE;
            break;

        default:
            break;
    }
}
#endif  //#ifdef ENABLE_QEP_LINEARIZATION_TUNING

#endif  //#ifndef QEP_LINEARIZATION_SM_H

