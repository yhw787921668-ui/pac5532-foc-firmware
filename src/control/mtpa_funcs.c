
/**
 * @file mtpa_funcs.c
 * @brief Use for FOC MTPA function
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

#include "maths.h"
#include "mtpa_funcs.h"
#include "mtpa.h"
#include "fault.h"

#ifdef ENABLE_MTPA
#define ABS_MIN_MTPA_SALIENCY_RATIO      QCONST(0.01f, Q16)
#define ABS_MAX_MTPA_SALIENCY_RATIO      QCONST(100.0f, Q16)

void mtpa_init(Struct_Motor* motor_ptr)
{
    motor_ptr->mtpa.mtpa_current_filter_coeff_q16 = calc_filter_alpha(I_FILTER_CUTOFF_FREQ_Q16, motor_ptr->control.dt_control_q26);
    motor_ptr->mtpa.mtpa_angle_ramp_rate_q16 = MTPA_RAMP_RATE_Q16;
}

void mtpa_gain_calc(Struct_Motor* motor_ptr)
{
    int32_t temp_q26;
    // Enable auto gain calculation
    #ifdef MTPA_ANGLE_AUTO_CALC
    motor_ptr->mtpa.mtpa_auto_gain_calc_flag = 1;
    #else
    motor_ptr->mtpa.mtpa_auto_gain_calc_flag = 0;
    #endif
    // Max current for mtpa
    motor_ptr->mtpa.mtpa_current_scale_q16 = motor_ptr->app.descale_current_factor;
    motor_ptr->mtpa.mtpa_max_current_ampere_q16 = fix16_mul((motor_ptr->control.outerloop_pid.max_value << 2), motor_ptr->app.descale_current_factor);
    // Saliency calculation
    // Lamda/(4*(Lq-Ld))
    temp_q26 = (motor_ptr->mparam.lq_q26 - motor_ptr->mparam.ld_q26) << 2;       // q26
    
    // Checking if Lq > Ld
    if (temp_q26 > 0)
    {
        // Generate a warning if diff is smaller than 1/(2+2)^2 of Lq. 
        // In other words, warning is generated if (Lq-Ld) is less than 6.25% of Lq
        if (temp_q26 < (motor_ptr->mparam.lq_q26 >> 2))
        {
            // Indicate it is not a salient motor
            set_warning(WARNING_MTPA_PARAM_INVAILD);
            motor_ptr->mtpa.mtpa_saliency_q16 = 0;
        }
        else
        {
            // Ld and Lq are looking good for MTPA
            clear_warning(WARNING_MTPA_PARAM_INVAILD);
            
            motor_ptr->mtpa.mtpa_saliency_q16 = fix16_div(motor_ptr->mparam.ke_q26, temp_q26);
            // Checking Ke for MTPA
            // Generate a warning if saliency is less than "ABS_MIN_MTPA_SALIENCY_RATIO"
            if (motor_ptr->mtpa.mtpa_saliency_q16 < ABS_MIN_MTPA_SALIENCY_RATIO)
            {
                set_warning(WARNING_MTPA_PARAM_INVAILD);
                motor_ptr->mtpa.mtpa_saliency_q16 = 0;
            }
            // Generate a warning if saliency is larger than "ABS_MAX_MTPA_SALIENCY_RATIO"
            else if (motor_ptr->mtpa.mtpa_saliency_q16 > ABS_MAX_MTPA_SALIENCY_RATIO)
            {
                set_warning(WARNING_MTPA_PARAM_INVAILD);
                motor_ptr->mtpa.mtpa_saliency_q16 = ABS_MAX_MTPA_SALIENCY_RATIO;
            }
            else
            {
                // Motor parameters are looking good for MTPA
                clear_warning(WARNING_MTPA_PARAM_INVAILD);
            }
        }
    }
    else
    {
        // Indicate it is not a salient motor
        set_warning(WARNING_MTPA_PARAM_INVAILD);
        motor_ptr->mtpa.mtpa_saliency_q16 = 0;
    }
    // Calculate the MTPA gain
    mtpa_gain_calc_lib(&motor_ptr->mtpa);
}

void mtpa_reset(Struct_Motor* motor_ptr)
{
    motor_ptr->mtpa.mtpa_iqf_q14 = 0;
    motor_ptr->mtpa.mtpa_angle_rad_q16 = 0;
}

void mtpa_param_write(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    switch(param_code)
    {
    case mtpa_gain:
        // Unit: Degree/A
        motor_ptr->mtpa.mtpa_gain_degpa_q16 = rx_data;
        break;
    default:
        break;
    }
}

void mtpa_param_process(Struct_Motor* motor_ptr, ParameterCode param_code, int32_t rx_data)
{
    // Parameter cases that require recalculation of MTPA user inputs
    switch(param_code)
    {
    case mtpa_gain:
    case descale_current_factor:
        // Unit: Degree, scaled
        motor_ptr->mtpa.mtpa_gain_rad_q16 = fix16_mul(CONV_DEG2RAD_FIX16, fix16_mul(motor_ptr->mtpa.mtpa_gain_degpa_q16, motor_ptr->app.descale_current_factor));
        break;
    default:
        break;
    }
    
    // For MTPA auto calculation
    switch(param_code)
    {
    case Ld:
    case Lq:
    case Config_Ke:
    case outerloop_pid_max:
    case descale_current_factor:
        mtpa_gain_calc(motor_ptr);
        break;
    default:
        break;
    }
}
#endif
