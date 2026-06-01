/**
 * @file atune_funcs.c
 * @brief Use for parameters update on the current and speed-loop PI gains
 */

//=============================================================================
// Copyright (C) 2018 - 2020, Qorvo, Inc.
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


#include "atune_funcs.h"
#include "pid.h"
#include "motor_struct.h"

#if defined(ENABLE_AUTOTUNE_PI)

////////////////////////////////
// ATPI Functions
////////////////////////////////
void atpi_reset_vars(Struct_Motor *motor_ptr)
{
    motor_ptr->atune.id_ka_q16 = 0;
    motor_ptr->atune.id_kb_q16 = 0;
    motor_ptr->atune.iq_ka_q16 = 0;
    motor_ptr->atune.iq_kb_q16 = 0;
    motor_ptr->atune.speed_ka_q16 = 0;
    motor_ptr->atune.speed_ka_q16 = 0;
//    motor_ptr->atune.status_atune |= (FLAG_ATUNE_ATPI_UPDATE_I | FLAG_ATUNE_ATPI_UPDATE_SPD);
}


void atpi_paramchange_check(Struct_Motor *motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    int32_t calc_current = 0;
    int32_t calc_speed = 0;

    // Dependency value change check for Kai & Kbi
    switch (param_code)
    {
        case dt:
            if ((rx_data != motor_ptr->control.dt_control_q26) && (motor_ptr->control.dt_control_q26 != 0) && (motor_ptr->control.dt_control_q26 != FOC_DT_Q26))
                calc_current = 1;
            break;
        case Ld:
            if ((rx_data != motor_ptr->mparam.ld_q26) && (motor_ptr->mparam.ld_q26 != 0) && (motor_ptr->mparam.ld_q26 != FOC_LD_DEFAULT_VALUE))
                calc_current = 1;
            break;
        case Lq:
            if ((rx_data != motor_ptr->mparam.lq_q26) && (motor_ptr->mparam.lq_q26 != 0) && (motor_ptr->mparam.lq_q26 != FOC_LQ_DEFAULT_VALUE))
                calc_current = 1;
            break;
        case Ralpha:
            if ((rx_data != motor_ptr->mparam.rs_q16) && (motor_ptr->mparam.rs_q16 != 0) && (motor_ptr->mparam.rs_q16 != FOC_RALPHA_DEFAULT_VALUE))
                calc_current = 1;
            break;
#if (MODULE_MPOS_LBG1 == ENABLED)
        case LBG1_fc_speed:          // Speed filter cutoff freq
            if ((rx_data != motor_ptr->mpos.est.filter_fc_speed_q16) && (motor_ptr->mpos.est.filter_fc_speed_q16 != 0) && (motor_ptr->mpos.est.filter_fc_speed_q16 != EST_LBG1_KSPEED_DEFAULT_VALUE))
                calc_current = 1;
            break;
#endif
        default:
            break;
    }

    // Dependency value change check for Kav & Kbv
    switch (param_code)
    {
//        case dt:
//        case max_cl_speed_limit:
//        case diff_amp_gain:
        case Config_Ke:
            if ((rx_data != motor_ptr->mparam.ke_q26) && (motor_ptr->mparam.ke_q26 != 0) && (motor_ptr->mparam.ke_q26 != AUTOTUNE_KE_DEFAULT_VALUE))
                calc_speed = 1;
            break;
        case Confi_J:
            if ((rx_data != motor_ptr->mparam.j_q26) && (motor_ptr->mparam.j_q26 != 0) && (motor_ptr->mparam.j_q26 != AUTOTUNE_J_DEFAULT_VALUE))
                calc_speed = 1;
            break;
        case ATune_ATPI_Delta:
            if ((rx_data != motor_ptr->atune.delta_q6) && (motor_ptr->atune.delta_q6 != 0) && (motor_ptr->atune.delta_q6 != QCONST(10.0f, Q06)))
                calc_speed = 1;
            break;
        case num_pole_pairs:
            if ((rx_data != motor_ptr->mparam.pole_pairs) && (motor_ptr->mparam.pole_pairs != 0) && (motor_ptr->mparam.pole_pairs != FOC_NUM_POLE_PAIRS_DEFAULT_VALUE))
                calc_speed = 1;
            break;
#if (MODULE_MPOS_LBG1 == ENABLED)
        case LBG1_fc_speed:           // Speed filter cutoff freq
            if ((rx_data != motor_ptr->mpos.est.filter_fc_speed_q16) && (motor_ptr->mpos.est.filter_fc_speed_q16 != 0) && (motor_ptr->mpos.est.filter_fc_speed_q16 != EST_LBG1_KSPEED_DEFAULT_VALUE))
                calc_speed = 1;
            break;
#endif
        default:
            break;
    }

    if (calc_current)
        motor_ptr->atune.status_atune |= WARNING_ATUNE_ATPI_CALCNEEDED_I;

    if (calc_speed)
        motor_ptr->atune.status_atune |= WARNING_ATUNE_ATPI_CALCNEEDED_SPD;
}


#ifdef ENABLE_AUTOTUNE_REGULATE_IQREF
void regulate_iqref(Struct_Motor* motor_ptr)
{
    // When used, regulate_iqref() function needs to be called from systick ISR
    int32_t limit_low;
    int32_t limit_high;
    int32_t increment;

    if (motor_ptr->state.control == STATE_CONTROL_TORQUE)
    {
        limit_low = motor_ptr->control.speedrefol_target_radps - fix16_mul(motor_ptr->control.speedrefol_target_radps, QCONST(0.1f, Q16));
        limit_high = motor_ptr->control.speedrefol_target_radps + fix16_mul(motor_ptr->control.speedrefol_target_radps, QCONST(0.1f, Q16));
        increment = QCONST(0.0001f, Q16);

        if (motor_ptr->mpos.est_spd_radps < limit_low)
        {
            motor_ptr->control.iqref_target_f16 += increment;
        }
        else if ((motor_ptr->mpos.est_spd_radps > limit_high) && (motor_ptr->control.iqref_target_f16 > motor_ptr->app.iqrefcl_cmd_f16))
        {
            motor_ptr->control.iqref_target_f16 -= increment;
        }

        if (motor_ptr->control.iqref_target_f16 > motor_ptr->app.iqrefcl_cmd_f16)
        {
            motor_ptr->atune.flag_iqref_reg_active = 1;
//            debugio2_on();
        }
        else
        {
            motor_ptr->atune.flag_iqref_reg_active = 0;
//            debugio2_off();
        }

    }
}
#endif


////////////////////////////////
// AT General Functions
////////////////////////////////
void at_calc_param(Struct_Motor *motor_ptr)
{
    int32_t temp1, temp2;
    int32_t fcfilter_q16;

    motor_ptr->atune.dt_control_q26 = motor_ptr->control.dt_control_q26;
    motor_ptr->atune.dt_control_1div_q6 = fix16_div(FIX16_1P0, motor_ptr->atune.dt_control_q26);
    // q26 = q26 * q0
    motor_ptr->atune.dt_speed_q26 = motor_ptr->atune.dt_control_q26 * SPEED_PI_LOOP_DIVIDER;
    motor_ptr->atune.i_max_control_q14 = motor_ptr->control.outerloop_pid.max_value;
    // Import filter_fc_speed_q16
#if (MODULE_MPOS_LBG1 == ENABLED)
    motor_ptr->atune.filter_fc_speed_q16 = motor_ptr->mpos.est.filter_fc_speed_q16;
#elif (MODULE_MPOS_HALL == ENABLED)
    motor_ptr->atune.filter_fc_speed_q16 = QCONST(75.0f, Q16);
#endif

    // Calculate tao_e
    // q26 = 2*pi* (q16 * q10)
    temp1 = fix16_mul(FIX16_2PI, motor_ptr->atune.filter_fc_speed_q16);
    motor_ptr->atune.filter_tau_speed_q26 = fix16_div(QCONST(1.0f, Q26), temp1);

#if defined(ENABLE_AUTOTUNE_MID)
    // Calculate ATPI filter frequency and gains
    // f0 = fc_scpeed / delta
    temp1 = fix16_div(motor_ptr->atune.filter_fc_speed_q16, motor_ptr->atune.delta_q6);      // q16 / q6 = q26
    // wcfilter = f0 / fcfilter_divider
    temp2 = fix16_div(QCONST(1.0f, Q06), MID2_FILTER_FCDIV_Q16);                        // q6 = q6 / q16
    fcfilter_q16 = fix16_mul(temp1, temp2);                                                 // q16 = q26 * q6
    // Calculate filter gain
    motor_ptr->atune.filter_alpha_dc_q16 = calc_filter_alpha(fcfilter_q16, DT_DIVSYSTICK_Q26);

    // Calculate deadtime voltage = td / Ts
    motor_ptr->atune.v_led_scaled_q14 = (fix16_mul(QCONST(((float)MOTOR1_LED_NSEC / 1000000000), Q30), motor_ptr->atune.dt_control_1div_q6)) >> 6;    // q30 (*16) q6 = q20>>6 = q14
    motor_ptr->atune.v_ted_scaled_q14 = (fix16_mul(QCONST(((float)MOTOR1_TED_NSEC / 1000000000), Q30), motor_ptr->atune.dt_control_1div_q6)) >> 6;    // q30 (*16) q6 = q20>>6 = q14
#endif
}


void at_reset_state(Struct_Motor *motor_ptr)
{
    motor_ptr->atune.state_mid2 = STATE_MID2_IDLE;
    motor_ptr->atune.uicmd_atune = UICMD_ATUNE_NEXTCMD;
    motor_ptr->atune.status_atune &= ~(STATUS_ATUNE_ALLSTATUS_RESET | STATUS_ATUNE_ALLWARNINGS_RESET);
    motor_ptr->atune.state_mainsub_mid1 = STATE_MAINSUB_MID1_RESET;

    // Signal to clear accumulators and storing variables
    motor_ptr->atune.staterec_mid2 = STATEREC_MID2_VALUES_READ;
}


void at_param_write(Struct_Motor *motor_ptr, ParameterCode param_code, int32_t rx_data)
{

    switch (param_code)
    {
         case ATune_Command:
             //motor_ptr->atune.uicmd_atune = rx_data;
             // Clear all the status bit for MID1 when received command from MID2
             motor_ptr->atune.status_atune &= ~(STATUS_ATUNE_MID1_ALLSTATUS | STATUS_ATUNE_MID1_ALLWARNINGS | STATUS_ATUNE_MID1_ALLFAULTS);
             if (rx_data == UICMD_ATUNE_RESET)
                 at_uicmd_reset(motor_ptr);
             else if (rx_data == UICMD_ATUNE_ISTEP)
                 at_uicmd_istep(motor_ptr);
             else if (rx_data == UICMD_ATUNE_CALCMID)
                 at_uicmd_calcmid(motor_ptr);
             else if (rx_data == UICMD_ATUNE_CALCGAIN_SPEED)
                 at_uicmd_calcgain_speed(motor_ptr);
             else if (rx_data == UICMD_ATUNE_CALCGAIN_CURRENT)
                 at_uicmd_calcgain_current(motor_ptr);
             if (rx_data == UICMD_ATUNE_MID2_STOP)
                 at_uicmd_stop(motor_ptr);
             break;
         case ATune_MID2_Istep:
             motor_ptr->atune.istep_q16 = rx_data;
             break;
         case ATune_ATPI_Delta:
             motor_ptr->atune.delta_q6 = rx_data;
             break;
         case ATune_MID2_RunMode:
             // Auto case
             if (rx_data == 0)
                 motor_ptr->atune.mid2_runmode = MID2_RUNMODE_AUTO;
             else if (rx_data == 1)
                 motor_ptr->atune.mid2_runmode = MID2_RUNMODE_AUTO_MANCFG;
             else if (rx_data == 2)
                 motor_ptr->atune.mid2_runmode = MID2_RUNMODE_MANUAL;
             break;

             // Read and write AutoTune storage variables and command for MID1
         case ATune_MID1_Start:
             if(rx_data > 0)
                 motor_ptr->atune.mid1_motor_enable_cmd = TRUE;
             else
                 motor_ptr->atune.mid1_motor_enable_cmd = FALSE;
             break;
         case ATune_MID1_RunMode:
         case ATune_MID1_ManCmd:
             if (param_code == ATune_MID1_RunMode)
                 motor_ptr->atune.mid1_runmode = (eMID1RunMode) rx_data;
             if (param_code == ATune_MID1_ManCmd)
                 motor_ptr->atune.mid1_mancmd = (eMID1ManCmd) rx_data;

             if ((motor_ptr->atune.mid1_runmode == MID1_RUNMODE_AUTO) || (motor_ptr->atune.mid1_runmode == MID1_RUNMODE_AUTO_MANCFG)
                 || (motor_ptr->atune.mid1_runmode == MID1_RUNMODE_AUTO_MANCFG))
                 motor_ptr->atune.mid1_mancmd = MID1_MANCMD_IDLE;
             break;

         case ATune_MID1_AlignV1:
             motor_ptr->atune.i_align1_gui_q14 = rx_data >> 2;
             break;
         case ATune_MID1_AlignV2:
             motor_ptr->atune.i_align2_gui_q14 = rx_data >> 2;
             break;
         case ATune_MID1_Vinj:
             motor_ptr->atune.i_inj_gui_q14 = rx_data >> 2;
             break;
         case ATune_MID1_numlimit:
             motor_ptr->atune.num_limit_gui = rx_data;
             break;
         default:
             break;
    }
}


void at_param_process(Struct_Motor *motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    // Call at_calc_param() for the listed cases
    switch (param_code)
    {
        case dt:
        case outerloop_pid_max:
        case ATune_ATPI_Delta:
        case LBG1_fc_speed:           // Speed filter cutoff freq
            at_calc_param(motor_ptr);
            break;
        default:
            break;
    }
}

#endif  //#if defined(ENABLE_AUTOTUNE_PI)
