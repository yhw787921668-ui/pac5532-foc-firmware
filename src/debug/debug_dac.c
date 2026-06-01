
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
#include "debug_dac.h"

#if defined (ENABLE_DEBUG_PWMDAC) || defined (ENABLE_DEBUG_DAS) || defined (ENABLE_DEBUG_SAMPLE_RAM) || defined (ENABLE_DEBUG_RTT)
fix16_t debug_scale_factor1 = 0;
fix16_t debug_scale_factor2 = 0;
fix16_t debug_scale_factor3 = 0;
fix16_t debug_scale_factor4 = 0;

fix16_t debug_dac_1a = 0;
fix16_t debug_dac_1b = 0;
fix16_t debug_dac_1c = 0;
fix16_t debug_dac_2a = 0;
fix16_t debug_dac_2b = 0;
fix16_t debug_dac_2c = 0;

fix16_t temp_var1 = 0;

int32_t* DAC_ARRAY_Q14 [PWM_DEBUG_ARRAY_LENGTH] =
{
        // Case 0: Speed Ref
        &motor.control.outerloop_ramp_scaled_q14,
        // Case 1: Speed Feedback
        &motor.control.outerloop_fdbk_scaled_q14,
        // Case 2: Speed PI output
        &motor.foc.iqref_ramp_q14,

        // Case 3: Iq Ref
        &motor.foc.iqref_ramp_q14,
        // Case 4: Iq Feedback
        &motor.foc.iq_q14,
        // Case 5: Iq PI Output
        &motor.foc.vq_q14,

        // Case 6: OL Ideal Angle
        &motor.debug.angle_q14,
        // Case 7: Estimated Angle
        &motor.debug.estimated_angle_q14,

        // Case 8: Id Ref
        &motor.foc.idref_ramp_q14,
        // Case 9: Id Feedback
        &motor.foc.id_q14,
        // Case 10: Id PI Output
        &motor.foc.vd_q14,

        // Case 11: Ialpha
        &motor.foc.ialpha_q14,
        // Case 12: Ibeta
        &motor.foc.ibeta_q14,

        // Case 13: Id (un-filtered)
        &motor.foc.id_prefilter_q14,
        // Case 14: Iq (un-filtered)
        &motor.foc.iq_prefilter_q14,

        // Case 15: Vd
        &motor.foc.vd_q14,
        // Case 16: Vq
        &motor.foc.vq_q14,

        // Case 17: Valpha
        &motor.foc.valpha_q14,
        // Case 18: Vbeta
        &motor.foc.vbeta_q14,

#if defined(ENABLE_STALL_DETECT)
        // Case 19: Vdrive
        &motor.app.vdrvsq_q14_peak_stored,
        // Case 20: Vbemf
        &motor.stall.ve_sqr_q14,
#else
        &temp_var1,
        &temp_var1,
#endif

#ifdef DEBUGDAC_DISPLAYDUTY_TICKS
        // Case 21: Duty U
        &motor.debug.duty_value_u_q14,
        // Case 22: Duty V
        &motor.debug.duty_value_v_q14,
        // Case 23: Duty W
        &motor.debug.duty_value_w_q14,
#else
        // Case 21: Duty U
        &motor.control.mod_va_q14,
        // Case 22: Duty V
        &motor.control.mod_vb_q14,
        // Case 23: Duty W
        &motor.control.mod_vc_q14,
#endif
        // Case 24: Iphase U
        &motor.control.Iu_measured_q14,
        // Case 25: Iphase V
        &motor.control.Iv_measured_q14,
        // Case 26: Iphase W
        &motor.control.Iw_measured_q14,

        // Case 27: mpos Ealpha
//        &motor.mpos.est.est_ealpha_q14,
        &temp_var1,
        // Case 28: mpos Ialpha
//        &motor.mpos.est.est_ialpha_q14,
        &temp_var1,

        // Case 29: Debug 1a
        &debug_dac_1a,
        // Case 30: Debug 1b
        &debug_dac_1b,
        // Case 31: Debug 1c
        &debug_dac_1c,

        // Case 32: Debug 2a
        &debug_dac_2a,
        // Case 33: Debug 2b
        &debug_dac_2b,
        // Case 34: Debug 2c
        &debug_dac_2c
};

int32_t* dac_ptr1 = (int32_t*) &DAC_ARRAY_Q14[0];
int32_t* dac_ptr2 = (int32_t*) &DAC_ARRAY_Q14[1];
int32_t* dac_ptr3 = (int32_t*) &DAC_ARRAY_Q14[4];
int32_t* dac_ptr4 = (int32_t*) &DAC_ARRAY_Q14[9];

#ifdef ENABLE_DEBUG_PWMDAC 
void pwmdac_enable(void)
{
    // Set PWM DAC duty cycles to 50%
    pwmdac_1_set_duty(PWMDAC_PERIOD_TICKS >> 1);
    pwmdac_2_set_duty(PWMDAC_PERIOD_TICKS >> 1);
    pwmdac_3_set_duty(PWMDAC_PERIOD_TICKS >> 1);

    // Select PWM peripheral for Port D
    dac_psel_pwm();

    // Turn on output enables
    dac_outen_enable();
}


void pwmdac_disable(void)
{
    // Set IO state of all pins to 0
    dac_out_clear();

    // Select GPIO peripheral for Port A
    dac_psel_gpio();

    // Turn on output enables
    dac_outen_enable();

    // Set PWM DAC duty cycles to 50%
    pwmdac_1_set_duty(PWMDAC_PERIOD_TICKS >> 1);
    pwmdac_2_set_duty(PWMDAC_PERIOD_TICKS >> 1);
    pwmdac_3_set_duty(PWMDAC_PERIOD_TICKS >> 1);
}
#endif

#endif  //#ifdef ENABLE_DEBUG_PWMDAC
