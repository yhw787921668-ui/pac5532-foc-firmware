
/**
 * @file  pid.h
 * @brief PID function declarations
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

#ifndef PID_H
#define PID_H

#include "maths.h"

typedef struct
{
    // Operational variables - updated controller
    fix16_t I_prev;                             // I term from the previous PID calculation
    fix16_t PI;                                 // Current value of PID controller output before saturation check
    fix16_t PI_sat;                             // Current value of PID controller output after saturation check

    // Configuration variables - specified by user. Controller does not change
    int32_t ka_q16;
    int32_t kb_q16;
    fix16_t kp_q11;                             // Proportional gain term * 2^8 (scaling for integer math)
    fix16_t kidt_q16;                           // combine Ki*dt
    fix16_t min_value;                          // Saturation filter minimum output value
    fix16_t max_value;                          // Saturation filter maximum output value
    fix16_t dt_q26;                             // dt for d/dt calculation (scaled by 2^10)
} PID_Data_Type;

/**
 * @brief Reset PID controller
 * @param pid_data Pointer to PID data structure
 */
void pid_reset(PID_Data_Type* pid_data);

/**
 * @brief Convert gain to PID data
 * @param pid_data pointer to PID data structure
 * @param scale_in_q16 input scale
 * @param scale_out_q16 output scale
 */
void picon_convert_gain(PID_Data_Type* pid_data, uint32_t scale_in_q16, int32_t scale_out_q16);

/**
 * @brief Initialize PI data structure
 * @param pid_data pointer to PID data structure
 * @param value_q14 Initialization value
 */
static inline void init_pi(PID_Data_Type* pid_data, int32_t value_q14)
{
    pid_data->PI_sat = value_q14;
    pid_data->I_prev = (value_q14 << 16);
}

// Add check_pi_gain_range function
PAC5XXX_OPTIMIZE_SPEED_ON // Optimize Speed for this function is currently needed to build the CoIDE project with -Os = Optimize Size setting

/**
 * @brief Executes the PID controller
 * @param f_pid_data pointer to the PID data
 * @param f_error error
 */
PAC5XXX_RAMFUNC static inline void pid_run(PID_Data_Type* f_pid_data, fix16_t f_error)
{
    fix16_t f_P = 0;
    fix16_t f_I = 0;

    // P = error * Kp
    f_P = ((f_pid_data->kp_q11 * f_error) >> 11);                       // q11 * q14 = q25>>11=q14  Kpmax=2^6=64

    // I = (error * Ki * TD + I_prev)
    f_I = (f_pid_data->kidt_q16 * f_error);                             // q16 * q14 = q30>>0 = q30
    f_I = f_I + f_pid_data->I_prev;

    // Calculate control output
    f_pid_data->PI = f_pid_data->PI_sat = f_P + (f_I >> 16);            // q14 + q30>>16=q14 = q14

    // Saturate output
    if ((f_pid_data->PI > f_pid_data->max_value) && (f_error > 0))
    {
        f_pid_data->PI_sat = f_pid_data->max_value;
        f_pid_data->I_prev = f_pid_data->max_value << 16;               // q30 = q14<<16
    }
    else if ((f_pid_data->PI < f_pid_data->min_value) && (f_error < 0))
    {
        f_pid_data->PI_sat = f_pid_data->min_value;
        f_pid_data->I_prev = f_pid_data->min_value << 16;               // q30 = q14<<16
    }
    else
    {
        f_pid_data->I_prev = f_I;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF

#if defined(ENABLE_DQ_DECOUPLING)
/**
 * @brief Executes the PID controller
 * @param f_pid_data pointer to the PID data
 * @param f_error error
 * @param feedforward_q14 feedforward terms for id and iq controller
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline void pid_run_decoupled(PID_Data_Type* f_pid_data, fix16_t f_error, int32_t feedforward_q14)
{
    fix16_t f_P = 0;
    fix16_t f_I = 0;

    // P = error * Kp
    f_P = ((f_pid_data->kp_q11 * f_error) >> 11);                       // q11 * q14 = q25>>11=q14  Kpmax=2^6=64

    // I = (error * Ki * TD + I_prev)
    f_I = (f_pid_data->kidt_q16 * f_error);                             // q16 * q14 = q30>>0 = q30
    f_I = f_I + f_pid_data->I_prev;

    // Calculate control output
    f_pid_data->PI = f_pid_data->PI_sat = f_P + (f_I >> 16) + feedforward_q14;            // q14 + q30>>16=q14 = q14

    // Saturate output
    if ((f_pid_data->PI > f_pid_data->max_value) && (f_error > 0))
    {
        f_pid_data->PI_sat = f_pid_data->max_value;
        if(f_pid_data->I_prev > (f_pid_data->max_value << 16))
        {
            f_pid_data->I_prev = f_pid_data->max_value << 16;               // q30 = q14<<16
        }
    }
    else if ((f_pid_data->PI < f_pid_data->min_value) && (f_error < 0))
    {
        f_pid_data->PI_sat = f_pid_data->min_value;
        if (f_pid_data->I_prev < (f_pid_data->min_value << 16))
        {
            f_pid_data->I_prev = f_pid_data->min_value << 16;               // q30 = q14<<16
        }
    }
    else
    {
        f_pid_data->I_prev = f_I;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif

#endif // PID_H

