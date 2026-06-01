
/**
 * @file foc.h
 * @brief Use for basic FOC process function
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

#ifndef FOC_H
#define FOC_H

#include "motor_struct.h"
#include "foc_math.h"
#include "foc_funcs.h"
#include "mod_ovm.h"
#include "utils.h"

#if (MODULATION_SELECT == MODULATION_SVM_HARMONICINJ)
#include "harmonic_inj_funcs.h"
#endif

#if (MODULATION_SELECT == MODULATION_HYBRID_TRAP_DMIN)
#include "trap_funcs.h"
#endif

/**
 * @brief process_foc, use to call basic FOC math flow related functions
 * @param Struct_Motor* motor_ptr motor pointer to FOC structure
 */
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting
PAC5XXX_RAMFUNC static inline void process_foc(Struct_Motor* motor_ptr)
{
    switch(motor_ptr->foc.state_foc)
    {
    case STATE_FOC_FULLFLOW:
#if (MODULATION_SELECT != MODULATION_HYBRID_TRAP_DMIN)
        // Clarke transform
        clarke_transform_q14(motor_ptr->control.Iu_measured_q14, motor_ptr->control.Iv_measured_q14, motor_ptr->control.Iw_measured_q14, &motor_ptr->foc.ialpha_q14, &motor_ptr->foc.ibeta_q14);

        // Park transform
        park_transform_q14(motor_ptr->mpos.mpos_angle, motor_ptr->foc.ialpha_q14, motor_ptr->foc.ibeta_q14, &motor_ptr->foc.id_prefilter_q14, &motor_ptr->foc.iq_prefilter_q14);
#elif(MODULATION_SELECT == MODULATION_HYBRID_TRAP_DMIN)
        if (motor_ptr->mpos.mposmode != STATE_MPOSMODE_TRAP)
        {
            // Clarke transform
            clarke_transform_q14(motor_ptr->control.Iu_measured_q14, motor_ptr->control.Iv_measured_q14, motor_ptr->control.Iw_measured_q14, &motor_ptr->foc.ialpha_q14, &motor_ptr->foc.ibeta_q14);

            // Park transform
            park_transform_q14(motor_ptr->mpos.mpos_angle, motor_ptr->foc.ialpha_q14, motor_ptr->foc.ibeta_q14, &motor_ptr->foc.id_prefilter_q14, &motor_ptr->foc.iq_prefilter_q14);
        }
        else
        {
            trap_current_mapping(motor_ptr->control.mod_sector, motor_ptr->control.Iu_measured_q14, motor_ptr->control.Iv_measured_q14, motor_ptr->control.Iw_measured_q14, &motor_ptr->foc.id_prefilter_q14, &motor_ptr->foc.iq_prefilter_q14);
        }
#endif

    #ifdef ENABLE_VOLTAGE_BASED_OPEN_LOOP_STARTUP
        // This is a temp implementation of the fixed voltage startup for the wing motor
        if (absval(motor_ptr->mpos.aligngo.speedrefol_ramp_q26) < ((absval(motor_ptr->mpos.aligngo.speedrefol_target_q26) >> 4) * VOL_SWITCH_OVER_CUR_PERCENT_Q04))
        {
            // D-axis startup voltage over-ride
            motor_ptr->foc.vd_q14 = 0;

            // Q-axis startup voltage over-ride
            if (motor_ptr->mpos.aligngo.startup_volt_reach_target_flag == 0)
            {
                if ((absval(motor_ptr->control.Iu_measured_q14) < (motor_ptr->mpos.aligngo.itotrefol_target_q16 >> 2)) &&
                    (absval(motor_ptr->control.Iv_measured_q14) < (motor_ptr->mpos.aligngo.itotrefol_target_q16 >> 2)) &&
                    (absval(motor_ptr->control.Iw_measured_q14) < (motor_ptr->mpos.aligngo.itotrefol_target_q16 >> 2))    )
                {
                    motor_ptr->mpos.aligngo.startup_volt_ramp_q14 += motor_ptr->mpos.aligngo.startup_volt_inc_q14;
                    // Limit the max Q-axis voltage
                    if (motor_ptr->mpos.aligngo.startup_volt_ramp_q14 > VOL_MAX_TARGET_Q14)
                    motor_ptr->mpos.aligngo.startup_volt_ramp_q14 = VOL_MAX_TARGET_Q14;
                }
                else 
                    {
                        // Reach the target current with this voltage
                        motor_ptr->mpos.aligngo.startup_volt_reach_target_flag = 1;
                        // Initilize the current PI
                        init_pi(&motor_ptr->foc.Id_pid, motor_ptr->foc.vd_q14);
                        init_pi(&motor_ptr->foc.Iq_pid, (motor_ptr->mpos.aligngo.startup_volt_ramp_q14 * motor_ptr->app.app_motor_direction));
                        // Initialize the Iqref
                        motor_ptr->foc.iqref_ramp_f16 = motor_ptr->mpos.aligngo.itotrefol_target_q16 * motor_ptr->app.app_motor_direction;
                        motor_ptr->foc.iqref_ramp_q14 = (motor_ptr->foc.iqref_ramp_f16 >> 2);
                        // Initialize the Idref
                        motor_ptr->foc.idref_ramp_f16 = motor_ptr->control.idref_target_f16;
                        motor_ptr->foc.idref_ramp_q14 = (motor_ptr->foc.idref_ramp_f16 >> 2);
                    }
            }

            // Q-axis startup voltage over-ride
            motor_ptr->foc.vq_q14 = motor_ptr->mpos.aligngo.startup_volt_ramp_q14 * motor_ptr->app.app_motor_direction;
        }
        else
    #endif
        {
            // Run Id/Iq PI loops
            idiq_pi_controllers(motor_ptr);
        }

    case STATE_FOC_BACKENDFLOW:
        // Inverse Park transform
        inverse_park_transform_q14(motor_ptr->mpos.mpos_angle, motor_ptr->foc.vd_q14, motor_ptr->foc.vq_q14, &motor_ptr->foc.valpha_q14, &motor_ptr->foc.vbeta_q14);
        break;

    case STATE_FOC_IDLE:
    default:
        break;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF

#endif // FOC_H
