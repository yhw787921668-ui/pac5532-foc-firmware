
/**
 * @file  maths.h
 * @brief Math support function declarations
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

#ifndef MATHS_H
#define MATHS_H

#ifndef __CC_ARM      // If not Keil RealView C Compiler
    #include <math.h>
#endif
#include "fix16.h"

// Lookup tables for Estimator and Transforms
extern const uint16_t foc_sin_tbl[];
extern const uint16_t foc_circle_tbl[];


#define TRUE 1
#define FALSE 0
// Math defines
#ifndef M_PI
    #define M_PI                      3.14159265358979323846
#endif
#define FLT_PI                  3.14159f
#define FLT_2PI                 (2.0f * FLT_PI)
#define FLT_SQRT_3              1.732051f

#define FIX16_PI                QCONST(FLT_PI, Q16)
#define FIX16_PI_DIV_2          QCONST((FLT_PI / 2), Q16)
#define FIX16_PI_DIV_3          QCONST((FLT_PI / 3), Q16)
#define FIX16_PI_DIV_4          QCONST((FLT_PI / 4), Q16)
#define FIX16_PI_DIV_6          QCONST((FLT_PI / 6), Q16)
#define FIX16_2PI               QCONST((2.0f * FLT_PI), Q16)
#define FIX16_2PI_DIV_3         QCONST((FLT_2PI / 3), Q16)
#define FIX16_2PI_DIV_5         QCONST((FLT_2PI / 5), Q16)
#define FIX16_1P5_PI            QCONST((1.5f * FLT_PI), Q16)
#define FIX16_1P0_DIV_2_PI      QCONST((1.0f / FLT_2PI), Q16)
#define FIX16_PI_X_1024         (FIX16_PI << 10)
#define FIX16_2PI_X_1024        (FIX16_2PI << 10)
#define FIX16_MINUS_PI_X_1024   ((int32_t)(((-M_PI * 1024) * 0x10000) + 0.5))
#define FIX16_PI_DIV_180_X_1024 ((int32_t)(((M_PI/180 * 1024) * 0x10000) + 0.5))

#define FIX16_NEG_PI            0xFFFCDBC1                  // -PI in fix16_t format
#define FIX16_NEG_PI_DIV_4      0xFFFF35C3                  // -PI/4
#define FIX16_NEG_PI_DIV_3      0xFFFEF3EB                  // -PI/3
#define FIX16_NEG_PI_DIV_2      0xFFFE6DE1                  // -PI/2

#define FIX16_1P0               0x00010000                  // 1.0 in fix16_t format
#define FIX16_1P5               0x00018000                  // 1.5 in fix16_t format
#define FIX16_0P9               0x0000E666                  // 0.9 in fix16_t format
#define FIX16_0P1               0x0000199A                  // 0.1 in fix16_t format
#define FIX16_SQRT_3            0x0001BB67                  // sqrt(3) in fix16_t format
#define FIX16_SQRT_3_DIV_2      0x0000DDB3                  // sqrt(3)/2 in fix16_t format
#define FIX16_1P0_DIV_SQRT_3    0x000093CD                  // 1/sqrt(3) in fix16_t format
#define FIX16_1P0_DIV_100       0x0000028F                  // 1.0/100 in fix16_t format

#define FIX16_1_DIV_SQRT2       0x0000B505                  // 1/sqrt(2) in f16 format
#define FIX1Q14_1P0_DIV_SQRT_3  0x49E6                      // 1/sqrt(3) in fix1q14_t format
#define FIXQ1P14_1P5            (0x00018000 >> 2)           // 1.5 in fixq1p14_t format
#define FIXQ1P14_0P9            (0x0000E666 >> 2)           // 0.9 in fixq1p14_t format
#define FIXQ1P14_SQRT_3         (0x0001BB67 >> 2)           // sqrt(3) in fixq1p14_t format
#define FIXQ1P14_SQRT_3_DIV_2   (0x0000DDB3 >> 2)           // sqrt(3)/2 in fixq1p14_t format

#define ZERO_POINT_5773_IN_Q14              0x24F2          // 0.5773 in Q14 format
#define ZERO_POINT_866_IN_Q14               0x376C          // 0.866 in Q14 format

#define ONE_DIVIDE_BY_5_FIX16   0x00003333                  // 0.2 in fix16 format

#define FIX16_1DIV3             0x5555

// Q-Constant Macro
#define Q00     0
#define Q01     1
#define Q02     2
#define Q03     3
#define Q04     4
#define Q05     5
#define Q06     6
#define Q07     7
#define Q08     8
#define Q09     9
#define Q10     10
#define Q11     11
#define Q12     12
#define Q13     13
#define Q14     14
#define Q15     15
#define Q16     16
#define Q17     17
#define Q18     18
#define Q19     19
#define Q20     20
#define Q21     21
#define Q22     22
#define Q23     23
#define Q24     24
#define Q25     25
#define Q26     26
#define Q27     27
#define Q28     28
#define Q29     29
#define Q30     30
#define QCONST(const_val, q_num)   ( ((int32_t) ((const_val) * (1 << (q_num)))) )


// Q14 Constant definitions
#define Q14_0P5                 QCONST(0.5f, Q14)
#define Q14_1P0                 QCONST(1.0f, Q14)
#define Q14_1P5                 QCONST(1.5f, Q14)
#define Q14_1_DIV_3             QCONST(0.33333333f, Q14)
#define Q14_SQRT3_DIV_SQRT2     QCONST(1.22474487f, Q14)
#define Q14_1_DIV_SQRT2         QCONST(0.70710678f, Q14)
#define Q14_SQRT3_DIV_2         QCONST((1.732051 / 2.0), Q14)


// Q16 Constant definitions
#define Q16_0P5                 QCONST(0.5f, Q16)
#define Q16_1P0                 QCONST(1.0f, Q16)
#define Q16_1P5                 QCONST(1.5f, Q16)
#define Q16_SQRT_2              QCONST(1.41421356f, Q16)
#define Q16_SQRT_3              QCONST(1.73205081f, Q16)
#define CONST_7PI_DIV_6_FIX16   QCONST((7.0 * FLT_PI / 6.0), Q16)
#define CONST_5PI_DIV_6_FIX16   QCONST((5.0 * FLT_PI / 6.0), Q16)
#define Q16_ONE_DIV_SQRT3       QCONST(0.57735027f, Q16)
#define Q16_1000                QCONST(1000.0f, Q16)
#define Q16_100                 QCONST(100.0f, Q16)
#define Q16_3DIV2               QCONST((3.0 / 2.0), Q16)

//typedef short q1p14_t;
typedef fix16_t q1p14_t;
typedef long int q1p30_t;

// Conversions
#define HZ2RAD_FIX16       411775    // 2*pi * 2^16 = 411775 [Hz * HZ2RAD_FIX16 = Rad/s]
#define RAD2HZ_FIX16        10430    // 1/(2*pi)    * 2^16 = 10430 [Rad/s * RAD2HZ_FIX16 = Hz]
#define CONV_DEG2RAD_FIX16        QCONST((FLT_PI / 180), Q16)      // pi/180
#define CONV_RAD2DEG_FIX16        QCONST((180 / FLT_PI), Q16)      // 180/pi

#define CL_SPEED_0HZ        0
#define CL_SPEED_0p5HZ      (HZ2RAD_FIX16/2)
#define CL_SPEED_0p75HZ     (3*HZ2RAD_FIX16/4)
#define CL_SPEED_1HZ        (1*HZ2RAD_FIX16)
#define CL_SPEED_2HZ        (2*HZ2RAD_FIX16)
#define CL_SPEED_3HZ        (3*HZ2RAD_FIX16)
#define CL_SPEED_4HZ        (4*HZ2RAD_FIX16)
#define CL_SPEED_5HZ        (5*HZ2RAD_FIX16)
#define CL_SPEED_6HZ        (6*HZ2RAD_FIX16)
#define CL_SPEED_7HZ        (7*HZ2RAD_FIX16)
#define CL_SPEED_8HZ        (8*HZ2RAD_FIX16)
#define CL_SPEED_9HZ        (9*HZ2RAD_FIX16)
#define CL_SPEED_10HZ       (10*HZ2RAD_FIX16)
#define CL_SPEED_20HZ       (20*HZ2RAD_FIX16)
#define CL_SPEED_25HZ       (25*HZ2RAD_FIX16)
#define CL_SPEED_50HZ       (50*HZ2RAD_FIX16)
#define CL_SPEED_100HZ      (100*HZ2RAD_FIX16)
#define CL_SPEED_150HZ      (150*HZ2RAD_FIX16)
#define CL_SPEED_200HZ      (200*HZ2RAD_FIX16)
#define CL_SPEED_250HZ      (250*HZ2RAD_FIX16)
#define CL_SPEED_300HZ      (300*HZ2RAD_FIX16)
#define CL_SPEED_400HZ      (400*HZ2RAD_FIX16)
#define CL_SPEED_500HZ      (500*HZ2RAD_FIX16)
#define CL_SPEED_600HZ      (600*HZ2RAD_FIX16)
#define CL_SPEED_750HZ      (750*HZ2RAD_FIX16)
#define CL_SPEED_1000HZ     (1000*HZ2RAD_FIX16)
#define CL_SPEED_1500HZ     (1500*HZ2RAD_FIX16)
#define CL_SPEED_2000HZ     (2000*HZ2RAD_FIX16)
#define CL_SPEED_2500HZ     (2500*HZ2RAD_FIX16)
#define CL_SPEED_3000HZ     (3000*HZ2RAD_FIX16)
#define CL_SPEED_3500HZ     (3500*HZ2RAD_FIX16)

#define VDC_1V      (1*65536)
#define VDC_5V      (5*65536)
#define VDC_6V      (6*65536)
#define VDC_7V      (7*65536)
#define VDC_8V      (8*65536)
#define VDC_9V      (9*65536)
#define VDC_10V     (10*65536)
#define VDC_11V     (11*65536)
#define VDC_12V     (12*65536)
#define VDC_13V     (13*65536)
#define VDC_14V     (14*65536)
#define VDC_15V     (15*65536)
#define VDC_16V     (16*65536)
#define VDC_17V     (17*65536)
#define VDC_18V     (18*65536)
#define VDC_19V     (19*65536)
#define VDC_20V     (20*65536)
#define VDC_24V     (24*65536)
#define VDC_25V     (25*65536)
#define VDC_30V     (30*65536)
#define VDC_45V     (45*65536)
#define VDC_50V     (50*65536)
#define VDC_60V     (60*65536)
#define VDC_70V     (70*65536)
#define VDC_75V     (75*65536)
#define VDC_100V    (100*65536)
#define VDC_150V    (150*65536)
#define VDC_200V    (200*65536)
#define VDC_250V    (250*65536)
#define VDC_300V    (300*65536)
#define VDC_350V    (350*65536)
#define VDC_400V    (400*65536)
#define VDC_450V    (450*65536)

#define DIFF_AMP_GAIN_1X  1
#define DIFF_AMP_GAIN_2X  2
#define DIFF_AMP_GAIN_4X  3
#define DIFF_AMP_GAIN_8X  4
#define DIFF_AMP_GAIN_16X  5
#define DIFF_AMP_GAIN_32X  6
#define DIFF_AMP_GAIN_48X  7

#define Q3_0P000     0
#define Q3_0P125     1
#define Q3_0P250     2
#define Q3_0P375     3
#define Q3_0P500     4
#define Q3_0P625     5
#define Q3_0P750     6
#define Q3_0P875     7

#define BIT_00      (1 << 0)
#define BIT_01      (1 << 1)
#define BIT_02      (1 << 2)
#define BIT_03      (1 << 3)
#define BIT_04      (1 << 4)
#define BIT_05      (1 << 5)
#define BIT_06      (1 << 6)
#define BIT_07      (1 << 7)
#define BIT_08      (1 << 8)
#define BIT_09      (1 << 9)
#define BIT_10      (1 << 10)
#define BIT_11      (1 << 11)
#define BIT_12      (1 << 12)
#define BIT_13      (1 << 13)
#define BIT_14      (1 << 14)
#define BIT_15      (1 << 15)
#define BIT_16      (1 << 16)
#define BIT_17      (1 << 17)
#define BIT_18      (1 << 18)
#define BIT_19      (1 << 19)
#define BIT_20      (1 << 20)
#define BIT_21      (1 << 21)
#define BIT_22      (1 << 22)
#define BIT_23      (1 << 23)
#define BIT_24      (1 << 24)
#define BIT_25      (1 << 25)
#define BIT_26      (1 << 26)
#define BIT_27      (1 << 27)
#define BIT_28      (1 << 28)
#define BIT_29      (1 << 29)
#define BIT_30      (1 << 30)
#define BIT_31      (1 << 31)

#define DEG_m180_IN_RAD_FIX16           0xFFFCDBC1
#define DEG_m150_IN_RAD_FIX16           0xFFFD61CC
#define DEG_m120_IN_RAD_FIX16           0xFFFDE7F0
#define DEG_m090_IN_RAD_FIX16           0xFFFE6DE1
#define DEG_m060_IN_RAD_FIX16           0xFFFEF3EB
#define DEG_m030_IN_RAD_FIX16           0xFFFF79F6
#define DEG_p000_IN_RAD_FIX16           0x00000000
#define DEG_p030_IN_RAD_FIX16           0x0000860A
#define DEG_p060_IN_RAD_FIX16           0x00010C15
#define DEG_p090_IN_RAD_FIX16           0x0001921F
#define DEG_p120_IN_RAD_FIX16           0x00021810
#define DEG_p150_IN_RAD_FIX16           0x00029E34
#define DEG_p180_IN_RAD_FIX16           0x0003243F

#define DEG_m135_IN_RAD_FIX16           0xFFFDA4D0
#define DEG_p045_IN_RAD_FIX16           0x0000C910
#define DEG_m015_IN_RAD_FIX16           0xFFFFBCFB
#define DEG_p165_IN_RAD_FIX16           0x0002E13A
#define DEG_p105_IN_RAD_FIX16           0x0001D525
#define DEG_m075_IN_RAD_FIX16           0xFFFEB0E6

#define ZERO_POINT_FIVE 0x00008000                                      // 1/2 in fix16

#define ONE_THIRD_IN_FIX16 0x00005555                                   // 0.3333 is fxi16 0x00005555

#define CONV_Q10TOQ14                                   QCONST((16383.0f / 1023.0f), Q14)              // ((2^14)-1) / ((2^10)-1)
#define CONV_ADC2VAIO_Q10Q14                            QCONST((2.5f / 1023.0f), Q14)
#define CONV_ADC2VAIO_Q10Q16                            QCONST((2.5f / 1023.0f), Q16)

#define Q14_ONE                         QCONST(1.0f, Q14)
#define Q14_0P5                         QCONST(0.5f, Q14)
#define Q16_ONE                         QCONST(1.0f, Q16)
#define Q28_ONE                         QCONST(1.0f, Q28)

/**
 * @brief  Create floating point number from Q30 format
 * @param  value Q30 number
 * @return Result in floating point format
 */
static inline float float_from_q30(q1p30_t value) { return (float)value / (2 << 29); }

/**
 * @brief  Create floating point number from Q14 format
 * @param  value Q14 number
 * @return Result in floating point format
 */
static inline float float_from_q14(q1p14_t value) { return (float)value / (2 << 13); }

/**
 * @brief  Create double-precision floating point number from Q14 format
 * @param  value Q14 number
 * @return Result in double-precision floating point format
 */
static inline double double_from_q14(q1p14_t value) { return (double)value / (2 << 13); }

/**
 * @brief  Create Q14 format number from floating point format
 * @param  value floating point number
 * @return Result in Q14 format
 */
static inline q1p14_t q14_from_float(float value) { return (q1p14_t)((float)(value * (2 << 13))); }

/**
 * @brief  Create Q14 format number from double-precision floating point format
 * @param  value double precision floating point number
 * @return Result in Q14 format
 */
static inline q1p14_t q14_from_double(double value) { return (q1p14_t)((double)(value * (2 << 13))); }

/**
 * @brief  Add two Q14 format numbers
 * @param  a Q14 number
 * @param  a Q14 number
 * @return Sum in Q14 format
 */
static inline q1p14_t q14_add(q1p14_t a, q1p14_t b) { return a + b; }

/**
 * @brief  Subtract two Q14 format numbers (a - b)
 * @param  a Q14 number
 * @param  a Q14 number
 * @return Difference in Q14 format (a - b)
 */
static inline q1p14_t q14_sub(q1p14_t a, q1p14_t b) { return a - b; }

/**
 * @brief  Multiply two Q14 format numbers (a * b)
 * @param  a Q14 number
 * @param  a Q14 number
 * @return Product in Q30 format (a * b)
 */
static inline q1p30_t q30_mul(q1p14_t a, q1p14_t b)
{
    q1p30_t result = a * b;

    return result;
}

/**
 * @brief  Multiply and shift two Q14 format numbers (a * b)into Q30 format
 * @param  a Q14 number
 * @param  a Q14 number
 * @return Result in Q30 format (a * b) >> 14
 */
static inline q1p30_t q14_mul_shift(q1p14_t a, q1p14_t b) {
    q1p30_t result = (a * b) >> 14;

    return result;
}

/**
 * @brief  Calculate radians / second
 * @param  speed_radpdt_1024 Speed in radians per dt
 * @param  one_div_dt_1024 1 / dt / 1024
 * @return Result in fix16_t format
 */
static inline fix16_t conv_radpdt_radps(fix16_t speed_radpdt_1024, fix16_t one_div_dt_1024)
{
    // speed_radpdt_1024 = rad/dt * 1024
    // one_div_dt_1024 = 1 / (dt * 1024)
    // (speed_radpdt_1024*1024) * foc_one_div_dt/1024) = speed_radps
    // f16>>9=q7 * f16>>7=q9 = q16
    return (speed_radpdt_1024 >> 9) * (one_div_dt_1024 >> 7);
}

/**
 * @brief  Calculate a ramp reference based on a set-point and ramp rate
 * @param  reference Returned reference
 * @param  setpoint Starting set-point
 * @param  ramp_rate Ramp rate (gain)
 */
#define GENERIC_RAMP(reference, setpoint, ramp_rate) do{\
    register fix16_t temp_diff;                  \
    temp_diff = reference - setpoint;            \
    if (temp_diff > 0)                           \
    {                                            \
        if (temp_diff <= ramp_rate)              \
            reference = setpoint;                \
        else                                     \
            reference = reference - ramp_rate;   \
    }                                            \
    else if (temp_diff < 0)                      \
    {                                            \
        if (-temp_diff <= ramp_rate)             \
            reference = setpoint;                \
        else                                     \
            reference = reference + ramp_rate;   \
    }                                            \
} while(0)

/**
 * @brief  Perform an alpha filter
 * @param  new_value New value
 * @param  filtered_value Filtered Value
 * @param  alpha_gain Gain 
 * @result Filtered result: y(n) = y(n-1) + a * (x(n-1) - y(n-1))
 */
static inline fix16_t filter_alpha(fix16_t new_value, fix16_t filtered_value, fix16_t alpha_gain)
{
    
    return (filtered_value + fix16_mul((new_value - filtered_value), alpha_gain));
}

/**
 * @brief  Perform an alpha filter (high-speed)
 * @param  new_value New value
 * @param  filtered_value Filtered Value
 * @param  alpha_gain Gain
 * @result Filtered result: y(n) = y(n-1) + a * (x(n-1) - y(n-1))
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline fix16_t filter_alpha_fast(fix16_t new_value, fix16_t filtered_value, fix16_t alpha_gain)
{
    //Filter: y(n) = y(n-1) + a * (x(n-1) - y(n-1))

    return (filtered_value + fix16_mul_inline((new_value - filtered_value), alpha_gain));
}
PAC5XXX_OPTIMIZE_SPEED_OFF

/**
 * @brief  Perform an alpha filter and return the result in Q14 format
 * @param  new_value New value
 * @param  filtered_value Filtered Value
 * @param  alpha_gain Gain
 * @result Filtered result: y(n) = y(n-1) + a * (x(n-1) - y(n-1))
 */
static inline fix16_t filter_alpha_q14(int32_t new_value, int32_t filtered_value, fix16_t alpha_gain)
{
    //Filter: y(n) = y(n-1) + a * (x(n-1) - y(n-1))

    return (filtered_value + (((new_value - filtered_value) * alpha_gain) >> 16));      //q14 = q14 * q16 = q30>>16
}

/**
 * @brief  Perform an alpha filter and return the result in fix16_t format
 * @param  new_value_q14 New value
 * @param  filtered_value_q30 Filtered Value
 * @param  alpha_gain_q16 Gain
 * @result Filtered result: y(n) = y(n-1) + a * (x(n-1) - y(n-1))
 */
static inline fix16_t filter_alpha_q30(int32_t new_value_q14, int32_t filtered_value_q30, fix16_t alpha_gain_q16)
{
    //Filter: y(n) = y(n-1) + a * (x(n-1) - y(n-1))

    return (filtered_value_q30 + (((new_value_q14 - (filtered_value_q30 >> 16)) * alpha_gain_q16) >> 0));      //q30 = q30>>16=q14 * q16 = q30
}

/**
 * @brief  Perform an alpha filter and return the upper 16-bits of the result in fix16_t format
 * @param  new_value_q30 New value
 * @param  filtered_value_q30 Filtered Value
 * @param  alpha_gain_q11 Gain
 * @result Filtered result: y(n) = y(n-1) + a * (x(n-1) - y(n-1))
 */
static inline fix16_t filter_alpha_hi(int32_t new_value_q30, int32_t filtered_value_q30, fix16_t alpha_gain_q11)
{
    //Filter: y(n) = y(n-1) + a * (x(n-1) - y(n-1))

    return (filtered_value_q30 + ((((new_value_q30 - filtered_value_q30) >> 6) * alpha_gain_q11) >> 5));      //q18 = q18 * q12 = q30>>12
}

/**
 * @brief  Perform an alpha filter and return the lower 16-bits of the result in fix16_t format
 * @param  new_value_q30 New value
 * @param  filtered_value_q30 Filtered Value
 * @param  alpha_gain_q14 Gain
 * @result Filtered result: y(n) = y(n-1) + a * (x(n-1) - y(n-1))
 */
static inline fix16_t filter_alpha_lo(int32_t new_value_q30, int32_t filtered_value_q30, fix16_t alpha_gain_q14)
{
    //Filter: y(n) = y(n-1) + a * (x(n-1) - y(n-1))

    return (filtered_value_q30 + ((((new_value_q30 - filtered_value_q30) >> 6) * alpha_gain_q14) >> 8));      //q18 = q18 * q12 = q30>>12
}

/**
 * @brief  Perform an alpha filter and return value shifted by the given number of bits
 * @param  new_value_qx New value
 * @param  filtered_value_qxpshift Filtered Value
 * @param  shift Number of bits to shift
 * @result Filtered result: y(n) = y(n-1) + a * (x(n-1) - y(n-1))
 */
PAC5XXX_RAMFUNC static inline fix16_t filter_shift(int32_t new_value_qx, int32_t *filtered_value_qxpshift, uint32_t shift)
{
    //Filter: y(n) = y(n-1) + a * (x(n-1) - y(n-1))

    *filtered_value_qxpshift = *filtered_value_qxpshift + (((new_value_qx << shift) - *filtered_value_qxpshift) >> shift);      //qx = qxpshift >> shift

    return *filtered_value_qxpshift >> shift;
}

/**
 * @brief  Perform an alpha filter and return value shifted by the given number of bits
 * @param  new_value_qx New value
 * @param  filtered_value_qx Filtered Value
 * @param  shift Number of bits to shift
 * @result Filtered result: y(n) = y(n-1) + a * (x(n-1) - y(n-1))
 */
PAC5XXX_RAMFUNC static inline void filter_shift2(int32_t new_value_qx, int32_t *filtered_value_qx, uint32_t shift)
{
    //Filter: y(n) = y(n-1) + a * (x(n-1) - y(n-1))

    *filtered_value_qx = *filtered_value_qx + ((new_value_qx - *filtered_value_qx) >> shift);      //qx = qx

//    return *filtered_value_qx;
}

/**
 * @brief  Perform an alpha filter with cutoff frequency and dt
 * @param  freq_cutoff_q16 cutoff frequency
 * @param  filter_dt_q26 Dt
 * @result alpha gain - dt / (dt + 1/2PI*Hz)
 */
static inline fix16_t calc_filter_alpha(int32_t freq_cutoff_q16, int32_t filter_dt_q26)
{
    return fix16_div(filter_dt_q26, (filter_dt_q26 + fix16_div(QCONST(1.0f, Q26), fix16_mul(FIX16_2PI, freq_cutoff_q16))));
}

/**
 * @brief  Perform an alpha filter with DC blocker
 * @param  time_q26 time in seconds
 * @param  filter_dt_q26 dt
 * @result alpha gain
 */
static inline int32_t calc_filter_alpha_dcblocker(int32_t time_q26, int32_t filter_dt_q26)
{
    // Calculation for alpha gain:
    // 1 / (1 - alpha) = N (N=number of samples to settle)
    // N = T / dt (T=Time to settle (seconds))
    // alpha = (N - 1) / N

    int32_t n_q16;

    n_q16 = fix16_div (time_q26, filter_dt_q26);

    return fix16_div((n_q16 - FIX16_1P0), n_q16);
}

/**
 * @brief  Calculate DC blocker
 * @param  input_q14 Filter input
 * @param  output_q14 Filter output_iterator
 * @param  input_prev_q14 Previous filter input
 * @param  alpha_q16 Alpha
 */
static inline void filter_dcblocker(int32_t input_q14, int32_t *output_q14, int32_t *input_prev_q14, int32_t alpha_q16)
{
    // Filter: y(n) = x(n) - x(n-1) + a*y(n-1)

    int32_t output_prev_q14 = 0;

    output_prev_q14 = *output_q14;
    *output_q14 = input_q14 - *input_prev_q14 + ((alpha_q16 * output_prev_q14) >> 16);
    *input_prev_q14 = input_q14;
}

/**
 * @brief  Calculate the filter cutoff frequency
 * @param  alpha_q16 alpha
 * @param  filter_dt_q26 dt
 * @result alpha = alpha_gain / (dt*2*Pi - dt*2*Pi*alpha_gain) in Hz
 */
static inline fix16_t calc_filter_freq_cutoff(int32_t alpha_q16, int32_t filter_dt_q26)
{
    // Calculation for freq_cutoff:
    // freq_cutoff_q16 = alpha_gain / (dt*2*Pi - dt*2*Pi*alpha_gain) in Hz
    int32_t temp1_q26;

    temp1_q26 = fix16_mul(filter_dt_q26, FIX16_2PI);                                        // mul(q26,q16) = q26
    return fix16_div(alpha_q16 << 10,  (temp1_q26 - fix16_mul(temp1_q26, alpha_q16)));      // div(q26,q26=mul(q26,q16)) = q16
}

/**
 * @brief  Bubble sort an array
 * @param  sort_array pointer to array
 * @param  size length of array
 */
extern void bubble_sort_array(int32_t *sort_array, uint32_t size);

/**
 * @brief  Bubble sort an array in descending order
 * @param  sort_array pointer to array
 * @param  size length of array
 */
extern void bubble_sort_array_descending(int32_t *sort_array, uint32_t size);

extern void maths_calc_avg3(int32_t n1, int32_t n2, int32_t n3, int32_t *avg);

// q19 * m5q11 = q30
// q24 * m5q11 = q35>>5 = q30

// q16 * m8q14 = q30
// q24 * m8q14 = q38>>8 = q30
#endif

