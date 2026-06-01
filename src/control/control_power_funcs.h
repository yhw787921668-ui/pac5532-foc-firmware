
/**
 * @file control_power_funcs.h
 * @brief Use for control power calculation
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

#ifndef CONTROL_POWER_FUNCS_H
#define CONTROL_POWER_FUNCS_H

#include "motor_struct.h"

/*****************************************************************************************************************
    LINEAR CORRECTION CALIBRATION

    Notes:
    1) Ibus correction and Power correction are independent from each other.  Depending on the application, it may be necessary to choose one of the two options, or even both.
    2) If correcting for Ibus and power, then the Ibus correction must be completed first.

    Linear Correction for Ibus:
        ibus_corr = (LCORR_IBUS_M * ibus) + LCORR_IBUS_B

            where:
            LCORR_IBUS_M = (I_ACTUAL_HI - I_ACTUAL_LO) / (I_CALC_HI - I_CALC_LO)
            LCORR_IBUS_B = I_ACTUAL_LO - (LCORR_PWR_M * I_CALC_LO)

        1) Set linear model values as follows for calibration:
            LCORR_IBUS_M = 1
            LCORR_IBUS_B = 0
        2) Set power target to the lowest value where power regulation is desired.  Note that the power reading from the power supply may not match the set power target because it is not calibrated yet.
        3) Define I_ACTUAL_LO to be the current reading (Amps) from the power supply.
        4) Define I_CALC_LO to be the Ibus value (Amps) calculated by the firmware.
        5) Set the power target to the highest value where power regulation is desired and allow the motor to warm up before taking any readings. The power readings from the supply should settle when the motor temperature is settled.
        6) Define I_ACTUAL_HI to be the current reading (Amps) from the power supply.
        7) Define I_CALC_HI to be the Ibus value (Amps) calculated by the firmware.
        8) Define LCORR_IBUS_M = (I_ACTUAL_HI - I_ACTUAL_LO) / (I_CALC_HI - I_CALC_LO)
        9) Define LCORR_IBUS_B = I_ACTUAL_LO - (LCORR_IBUS_M * I_CALC_LO)
        10) Compile project with new values and verify that the Ibus calculation matches the current reading from the power supply.

    Linear Correction for Power:
        power_corr = (LCORR_PWR_M * power) + LCORR_PWR_B

            where:
            LCORR_PWR_M = (P_ACTUAL_HI - P_ACTUAL_LO) / (P_CALC_HI - P_CALC_LO)
            LCORR_PWR_B = P_ACTUAL_LO - (LCORR_PWR_M * P_CALC_LO)

        1) Set linear model values as follows for calibration:
            LCORR_PWR_M = 1
            LCORR_PWR_B = 0
        2) Set power target to the lowest value where power regulation is desired.  Note that the power reading from the power supply may not match the set power target because it is not calibrated yet.
        3) Define P_ACTUAL_LO to be the input power reading (Watts) from the power supply.
        4) Define P_CALC_LO to be the set power reading (Watts) calculated by the firmware.
        5) Set the power target to the highest value where power regulation is desired and allow the motor to warm up before taking any readings. The power readings from the supply should settle when the motor temperature is settled.
        6) Define P_ACTUAL_HI to be the input power reading (Watts) from the power supply.
        7) Define P_CALC_HI to be the set power reading (Watts) calculated by the firmware.
        8) Define LCORR_PWR_M = (P_ACTUAL_HI - P_ACTUAL_LO) / (P_CALC_HI - P_CALC_LO)
        9) Define LCORR_PWR_B = P_ACTUAL_LO - (LCORR_PWR_M * P_CALC_LO)
        10) Compile project with new values and verify that the power calculation matches the reading from the power supply.

******************************************************************************************************************/

#if defined(ENABLE_POWERMEASURE)

#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_1)
    #include "control_power1.h"
    #include "control_power1_funcs.h"
#elif (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2)
    #include "control_power2_funcs.h"
#endif


/**
 * @brief This function use to calculate low pass filter alpha
 * @param Struct_Motor* motor_ptr pointer to motor structure
 * @param int32_t freq_cutoff_q16
 */
static inline void power_calc_filter_alpha(Struct_Motor *motor_ptr, int32_t freq_cutoff_q16)
{
    motor_ptr->power.filter_alpha = calc_filter_alpha(freq_cutoff_q16, motor_ptr->control.dt_control_q26);
}


/**
 * @brief This function use to reset filter 1divva value
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void power_reset(Struct_Motor *motor_ptr)
{
    motor_ptr->power.power_out_1divva_filter_q30 = 0;
}


/**
 * @brief This function use for calculating power
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void power_calculator(Struct_Motor *motor_ptr)
{
#if (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_1)
    // Measure power in 1/VA units
    power1_calc_instpwr(&motor_ptr->power, &motor_ptr->control);
#elif (POWER_MEASURE_SELECT == POWER_MEASURE_METHOD_2)
    // Measure power in 1/VA units
    power2_read_ibusavg(motor_ptr);
#endif

    // Filter power calculation
    motor_ptr->power.power_out_1divva_filter_q30 = filter_alpha_q30(motor_ptr->power.power_out_1divva_q14, motor_ptr->power.power_out_1divva_filter_q30, motor_ptr->power.filter_alpha);
    motor_ptr->power.power_out_1divva_filter_q14 = motor_ptr->power.power_out_1divva_filter_q30 >> 16;
//    debug_dac_1a = motor_ptr->power.power_out_1divva_q14;
//    debug_dac_1b = motor_ptr->power.power_out_1divva_filter_q14;

    // Descale average bus current into Amps
    motor_ptr->power.ibus_avg_q16 = (((motor_ptr->power.power_out_1divva_filter_q30 >> 16) * (motor_ptr->power.current_descale_factor_q16 >> 7)) >> 7);               // q30>>16=q14 * (q16>>7=q9) = q23>>7 = q16 (Up to 2^8=256A average bus current supported)

#ifdef ENABLE_LCORR_IBUS
    // Linear Correction: Ibus
    motor_ptr->power.ibus_avg_q16 = (((motor_ptr->power.ibus_avg_q16 >> 3) * (LCORR_IBUS_M >> 4)) >> 9) + LCORR_IBUS_B;   // 6q16>>3=6q13 * q16>>4=q12 = 6q25>>9=6q16 (Up to 2^6=64A average bus current supported)
#endif

    // Multiply average ibus by Vbus to get power
    motor_ptr->power.power_preresult  = (((motor_ptr->power.ibus_avg_q16 >> 6) * (motor_ptr->app.dcbus_voltage >> 5)) >> 5);  // 6q16>>6=q10 * 9q16>>5=q11 = 10q21>>5 = 10q16 (Up to 2^10=1024W power supported)

#ifdef ENABLE_LCORR_PWR
    // Linear Correction: Power
    motor_ptr->power.power_result = (((motor_ptr->power.power_preresult >> 6) * (LCORR_PWR_M >> 5)) >> 5) + LCORR_PWR_B;  // 10q16>>6=10q10 * q16>>5=q11 = 10q21>>5=10q16 (Up to 2^10=1024W power supported)
#else
    // Pass power calculation
    motor_ptr->power.power_result = motor_ptr->power.power_preresult;
#endif

#ifdef ENABLE_LCORR_VBUS
    // Linear Correction: Volatge based
    motor_ptr->power.power_vbus_result_delta = ((((motor_ptr->app.dcbus_voltage - VDC_8V) >> 6) * (motor_ptr->power.lcorr_power_m >> 5)) >> 5) + motor_ptr->power.lcorr_power_b;  // 10q16>>6=10q10 * q16>>5=q11 = 10q21>>5=10q16 (Up to 2^10=1024W power supported)
    motor_ptr->power.power_result = motor_ptr->power.power_result - motor_ptr->power.power_vbus_result_delta;
#endif

#ifdef ENABLE_LCORR_TEMP
    // Linear Correction: Temperature based
    motor_ptr->power.power_temperature_result_delta = (((motor_ptr->temperature_degree_value >> 7) * (LCORR_TEMP_M >> 5)) >> 4) + LCORR_TEMP_B;   // 6q16>>3=6q13 * q16>>4=q12 = 6q25>>9=6q16 (Up to 2^6=64A average bus current supported)
    motor_ptr->power.power_result = motor_ptr->power.power_result + motor_ptr->power.power_temperature_result_delta;
#endif
}

#endif  //#if defined(ENABLE_POWERMEASURE)

#endif  //#ifndef CONTROL_POWER_FUNCS_H
