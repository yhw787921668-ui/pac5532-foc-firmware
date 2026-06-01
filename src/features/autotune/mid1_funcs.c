/**
 * @file mid1_funcs.c
 * @brief Use for auto-tuning function calls in motor identification 1
 */

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


#include "mid1_funcs.h"


#if defined(ENABLE_AUTOTUNE_MID)

void mid1_init(Struct_Motor *motor_ptr)
{
#if (INVERTER_TYPE_SEL == INVERTER_TYPE_MOS)
    motor_ptr->atune.mid1_inverter_type = MID1_INVERTER_TYPE_MOS;
#elif (INVERTER_TYPE_SEL == INVERTER_TYPE_IGBT)
    motor_ptr->atune.mid1_inverter_type = MID1_INVERTER_TYPE_IGBT;
#endif

    motor_ptr->atune.i_align1_gui_q14 = QCONST(0.25f, Q14);
    motor_ptr->atune.i_align2_gui_q14 = QCONST(0.50f, Q14);
    motor_ptr->atune.i_inj_gui_q14 = QCONST(0.125f, Q14);
    
    motor_ptr->atune.i_align_limit1_q14 = MID1_CURRENT_LIMIT1_PERCENT_Q14;
    motor_ptr->atune.i_align_limit2_q14 = MID1_CURRENT_LIMIT2_PERCENT_Q14;
    motor_ptr->atune.mid1_motor_enable_cmd = FALSE;

    motor_ptr->atune.motiondet_delta_threshold_pct_q14 = MID1_MOTIONDET_DELTA_THRESHOLD_PCT_Q14;
    motor_ptr->atune.motiondet_delay_limit_cnt = (fix16_div(MID1_MOTIONDET_DELAY_LIMIT_SEC_Q16, DT_DIVSYSTICK_Q16)) >> 16;
    motor_ptr->atune.motiondet_to_limit_cnt = (fix16_div(MID1_MOTIONDET_TO_LIMIT_SEC_Q16, DT_DIVSYSTICK_Q16)) >> 16;

    motor_ptr->atune.filter_shift = MID1_FILTER_SHIFT_Q0;
    motor_ptr->atune.dt_systickdiv_q16 = DT_DIVSYSTICK_Q16;

#if defined(CONFIG_MID1_3STEP_MODE)
    motor_ptr->atune.config_mid1_3step_mode = 1;
#else
    motor_ptr->atune.config_mid1_3step_mode = 0;
#endif

}


void mid1_process_motor_enable (Struct_Motor *motor_ptr)
{
    // Wait for the motor enable command and then transition to the STARTUP state
    if (motor_ptr->atune.mid1_motor_enable_cmd)
    {
        __disable_irq();
        motor_ptr->state.main = STATE_MAIN_MID1;
        motor_ptr->status.motor |= STATUS_MOTOR_ENABLED;
        motor_ptr->status.motor &= ~STATUS_MOTOR_STOPPED;
        __enable_irq();
    }
}

#endif  //#if defined(ENABLE_AUTOTUNE_MID)
