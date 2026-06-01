
/**
 * @file control_power2_funcs.h
 * @brief Use for control power functions
 */

#ifndef CONTROL_POWER2_FUNCS_H
#define CONTROL_POWER2_FUNCS_H


#include "motor_struct.h"
#include "control_power2_struct.h"
#include "adc_sample.h"

#define CURRENT_IBUS_SCALE_FACTOR_FLT                         (float)(2.5f / (PCB_RSENSE_IUVW_OHMS * AMPGAIN_AIO6))
#define POWER_CURRENT_DESCALE_FACTOR_Q16                      QCONST(CURRENT_IBUS_SCALE_FACTOR_FLT, Q16)

extern void power_compensation_coefficient_calc(Struct_Motor *motor_ptr);

//==========================================================================
//
// ADC Offset Configuration
//
//==========================================================================

//=====================================
// ADC Offset Channel Selection
// Seq Read Register Mapping for ADCCAL
//=====================================
#if defined(SELECT_AIO_POWER)

#if (SELECT_AIO_POWER == CHANNEL_AIO6)
    #define ENABLE_ADCCAL_AIO6
#else
    #error: SELECT_AIO_POWER is set to unsupported channel
#endif

#else   //#if defined(SELECT_AIO_POWER)
    #error: SELECT_AIO_POWER has not been defined in the board file.
#endif  //#if defined(SELECT_AIO_POWER)


#ifdef ENABLE_COMP_PULLUPGAINERROR
#define RF                      4700.0f
#define RP                      150000.0f
#define RF_DIV_RP_Q14           QCONST((RF / RP), Q14)
#define ADC_3P3V_Q14            QCONST((3.3 / 2.5 * (float)AMPGAIN_AIO6), Q14)
#define VBASE_Q14               QCONST(((3.3 * RF / (RF + RP)) / 2.5 * (float)AMPGAIN_AIO6), Q14)
#endif


/**
 * @brief This function use to read ADC offset of power ch aio6
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void power2_read_adcoffset(Struct_Motor *motor_ptr)
{
#if defined(SELECT_AIO_POWER)

#if (SELECT_AIO_POWER == CHANNEL_AIO6)
    motor_ptr->power.adc_offset_ibusavg_q14 = motor_ptr->post.adc_offset_aio6;
#else
    #error: SELECT_AIO_POWER is set to unsupported channel
#endif

#else   //#if defined(SELECT_AIO_POWER)
    #error: SELECT_AIO_POWER has not been defined in the board file.
#endif  //#if defined(SELECT_AIO_POWER)
}


/**
 * @brief This function use for initialization of current descale factor
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void power2_calc_param(Struct_Motor *motor_ptr)
{
    motor_ptr->power.current_descale_factor_q16 = POWER_CURRENT_DESCALE_FACTOR_Q16;
}


/**
 * @brief This function use to read ibusavg value
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void power2_read_ibusavg(Struct_Motor *motor_ptr)
{
    int32_t temp_q14;
#ifdef ENABLE_COMP_PULLUPGAINERROR
    int32_t comp_gainerror_q14;
#endif

    // Calculate scaled average current
    temp_q14 = ((readseq_ibusavg() * ADC_SCALE_CONST) >> 14);          //0q14 = "0q14" * 5q14 = q28>>14    
    // Compensate for pullup gain error
#ifdef ENABLE_COMP_PULLUPGAINERROR
    comp_gainerror_q14 = VBASE_Q14 - (((ADC_3P3V_Q14 - temp_q14) * RF_DIV_RP_Q14) >> 14);
    temp_q14 = temp_q14 + comp_gainerror_q14;                       // q14 * q14 = q18>>14 = q14
#endif

    // Subtract ADC cal offset
    temp_q14 = (temp_q14 - motor_ptr->power.adc_offset_ibusavg_q14);   // Original is centered at 0 so no need to x2

    motor_ptr->power.power_out_1divva_q14 = temp_q14;             // power/VA = Avg bus current
}

#endif  //#ifndef CONTROL_POWER2_FUNCS_H
