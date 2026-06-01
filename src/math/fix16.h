
/**
 * @file  fix16.h
 * @brief Fixed math library declarations (fix16_t)
 */
 
 /**************************************************************************
 * Copyright (c) the authors of libfixmath as seen on https://code.google.com/p/libfixmath/
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ***************************************************************************/

#ifndef __libfixmath_fix16_h__
#define __libfixmath_fix16_h__

#include "pac5xxx.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* These options may let the optimizer to remove some calls to the functions.
 * Refer to http://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
 */
#ifndef FIXMATH_FUNC_ATTRS
# ifdef __GNUC__
//#   define FIXMATH_FUNC_ATTRS __attribute__((leaf, nothrow, pure))
#   define FIXMATH_FUNC_ATTRS __attribute__((nothrow, pure))
# else
#   define FIXMATH_FUNC_ATTRS
# endif
#endif

//#define RAMFUNC_LINK

#ifdef RAMFUNC_LINK
#define FIX16_RAMFUNC PAC5XXX_RAMFUNC
#else
#define FIX16_RAMFUNC
#endif

#include <stdint.h>

#define FIXMATH_NO_OVERFLOW
#define FIXMATH_NO_ROUNDING

typedef int32_t fix16_t;

#define FOUR_DIV_PI                 0x145F3             /*!< Fix16 value of 4/PI */
#define _FOUR_DIV_PI2               0xFFFF9840          /*!< Fix16 value of -4/PI² */
#define X4_CORRECTION_COMPONENT     0x399A              /*!< Fix16 value of 0.225 */
#define PI_DIV_4                    0x0000C90F          /*!< Fix16 value of PI/4 */
#define THREE_PI_DIV_4              0x00025B2F          /*!< Fix16 value of 3PI/4 */

#define fix16_max                   0x7FFFFFFF          /*!< the maximum value of fix16_t */
#define fix16_min                   0x80000000          /*!< the minimum value of fix16_t */
#define fix16_overflow              0x80000000          /*!< the value used to indicate overflows when FIXMATH_NO_OVERFLOW is not specified */

#define fix16_pi                    205887              /*!< fix16_t value of pi */
#define fix16_e                     78145               /*!< fix16_t value of e */
#define FIX16_ONE                   0x00010000          /*!< fix16_t value of 1 */
#define fix16_neg_one               0xFFFF0000          /*!< fix16_t value of -1 */
 
/**
 * @brief  Convert integer into fix16_t
 * @param  a value (int)
 * @return Value in fix16_t format
 * @note   Inline to allow compiler to optimize away constant numbers
 */
FIX16_RAMFUNC static inline fix16_t fix16_from_int(int a) { return a * FIX16_ONE; }

/**
 * @brief  Convert fix16_t into floating point
 * @param  a value (fix16_t)
 * @return Value in floating point format
 * @note   Inline to allow compiler to optimize away constant numbers
 */
FIX16_RAMFUNC static inline float fix16_to_float(fix16_t a) { return (float)a / FIX16_ONE; }

/**
 * @brief  Convert fix16_t into double-precision floating point number
 * @param  a value (fix16_t)
 * @return Value in double-precision floating point format
 * @note   Inline to allow compiler to optimize away constant numbers
 */
FIX16_RAMFUNC static inline double fix16_to_dbl(fix16_t a) { return (double)a / FIX16_ONE; }

/**
 * @brief  Convert fix16_t into integer 
 * @param  a value (fix16_t)
 * @return Value in integer format
 * @note   Inline to allow compiler to optimize away constant numbers
 */
FIX16_RAMFUNC static inline int fix16_to_int(fix16_t a)
{
#ifdef FIXMATH_NO_ROUNDING
    return a >> 16;
#else
    if (a >= 0)
        return (a + FIX16_ONE / 2) / FIX16_ONE;
    else
        return (a - FIX16_ONE / 2) / FIX16_ONE;
#endif
}

/**
 * @brief  Convert floating point number into fix16_t
 * @param  a value (float)
 * @return Value in fix16_t format
 * @note   Inline to allow compiler to optimize away constant numbers
 */
FIX16_RAMFUNC static inline fix16_t fix16_from_float(float a)
{
    float temp = a * FIX16_ONE;
#ifndef FIXMATH_NO_ROUNDING
    temp += (temp >= 0) ? 0.5f : -0.5f;
#endif
    return (fix16_t)temp;
}

/**
 * @brief  Convert double-precision floating point number into fix16_t
 * @param  a value (fix16_t)
 * @return Value in double format
 * @note   Inline to allow compiler to optimize away constant numbers
 */
FIX16_RAMFUNC static inline fix16_t fix16_from_dbl(double a)
{
    double temp = a * FIX16_ONE;
#ifndef FIXMATH_NO_ROUNDING
    temp += (temp >= 0) ? 0.5f : -0.5f;
#endif
    return (fix16_t)temp;
}

/* Subtraction and addition with (optional) overflow detection. */
#ifdef FIXMATH_NO_OVERFLOW

/**
 * @brief  Add two fix16_t numbers
 * @param  a Value a
 * @param  b Value b
 * @return sum
 * @note   Inline to allow compiler to optimize away constant numbers
 */
FIX16_RAMFUNC static inline  fix16_t fix16_add(fix16_t inArg0, fix16_t inArg1) { return (inArg0 + inArg1); }

/**
 * @brief  Subtract two fix16_t numbers (a - b)
 * @param  a Value a
 * @param  b Value b
 * @return difference (a - b)
 * @note   Inline to allow compiler to optimize away constant numbers
 */
FIX16_RAMFUNC static inline fix16_t fix16_sub(fix16_t inArg0, fix16_t inArg1) { return (inArg0 - inArg1); }
#else

/**
 * @brief  Add two fix16_t numbers
 * @param  a Value a
 * @param  b Value b
 * @return sum
 */
extern FIX16_RAMFUNC fix16_t fix16_add(fix16_t a, fix16_t b) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Subtract two fix16_t numbers (a - b)
 * @param  a Value a
 * @param  b Value b
 * @return difference (a - b)
 */
extern FIX16_RAMFUNC fix16_t fix16_sub(fix16_t a, fix16_t b) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Saturated addition of two fix16_t numbers
 * @param  a Value a
 * @param  b Value b
 * @return Saturated sum
 */
extern FIX16_RAMFUNC fix16_t fix16_sadd(fix16_t a, fix16_t b) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Saturated subtraction of two fix16_t numbers (a - b)
 * @param  a Value a
 * @param  b Value b
 * @return Saturated difference (a - b)
 */
extern FIX16_RAMFUNC fix16_t fix16_ssub(fix16_t a, fix16_t b) FIXMATH_FUNC_ATTRS;

#endif

/**
 * @brief  Divides two fix16_t numbers (a / b)
 * @param  a Value a
 * @param  b Value b
 * @return Result (a / b)
 */
extern FIX16_RAMFUNC  fix16_t fix16_div(fix16_t inArg0, fix16_t inArg1) FIXMATH_FUNC_ATTRS;

#ifndef FIXMATH_NO_OVERFLOW

/**
 * @brief  Saturated multiplication of two fix16_t numbers
 * @param  inArg0 Value a
 * @param  inArg1 Value b
 * @return Result
 */
extern FIX16_RAMFUNC fix16_t fix16_smul(fix16_t inArg0, fix16_t inArg1) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Saturated division of two fix16_t numbers (a / b)
 * @param  inArg0 Value a
 * @param  inArg1 Value b
 * @return Result
 */
extern FIX16_RAMFUNC fix16_t fix16_sdiv(fix16_t inArg0, fix16_t inArg1) FIXMATH_FUNC_ATTRS;
#endif

/**
 * @brief  Returns the linear interpolation: (inArg0 * (1 - inFract)) + (inArg1 * inFract)
 * @param  inArg0 Value a
 * @param  inArg1 Value b
 * @param  inFract Fraction
 * @return Result
 */
extern FIX16_RAMFUNC fix16_t fix16_lerp8(fix16_t inArg0, fix16_t inArg1, uint8_t inFract) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Returns the linear interpolation: (inArg0 * (1 - inFract)) + (inArg1 * inFract)
 * @param  inArg0 Value a
 * @param  inArg1 Value b
 * @param  inFract Fraction
 * @return Result
 */
extern FIX16_RAMFUNC fix16_t fix16_lerp16(fix16_t inArg0, fix16_t inArg1, uint16_t inFract) FIXMATH_FUNC_ATTRS;
#ifndef FIXMATH_NO_64BIT
/**
 * @brief  Returns the linear interpolation: (inArg0 * (1 - inFract)) + (inArg1 * inFract)
 * @param  inArg0 Value a
 * @param  inArg1 Value b
 * @param  inFract Fraction
 * @return Result
 */
extern FIX16_RAMFUNC fix16_t fix16_lerp32(fix16_t inArg0, fix16_t inArg1, uint32_t inFract) FIXMATH_FUNC_ATTRS;
#endif

/**
 * @brief  Returns the sine parabola of the given value
 * @param  inAngle input angle
 * @return Sin(inAngle)
 */
extern FIX16_RAMFUNC fix16_t fix16_sin_parabola(fix16_t inAngle) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Returns the sine of the given value
 * @param  inAngle input angle
 * @return Sin(inAngle)
 */
extern FIX16_RAMFUNC fix16_t fix16_sin(fix16_t inAngle) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Returns the cosine of the given value
 * @param  inAngle input angle
 * @return Cos(inAngle)
 */
extern FIX16_RAMFUNC fix16_t fix16_cos(fix16_t inAngle) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Returns the tangent of the given value
 * @param  inAngle input angle
 * @return Tan(inAngle)
 */
extern FIX16_RAMFUNC fix16_t fix16_tan(fix16_t inAngle) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Returns the arc sine of the given value
 * @param  inAngle input angle
 * @return arcsin(inAngle)
 */
extern FIX16_RAMFUNC fix16_t fix16_asin(fix16_t inValue) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Returns the arc cosine of the given value
 * @param  inAngle input angle
 * @return arccos(inAngle)
 */
extern FIX16_RAMFUNC fix16_t fix16_acos(fix16_t inValue) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Returns the arc tangent of the given value
 * @param  inAngle input angle
 * @return arctan(inAngle)
 */
extern FIX16_RAMFUNC fix16_t fix16_atan(fix16_t inValue) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Returns the arc tangent 2 of the given value
 * @param  inAngle input angle
 * @return arctan2(inAngle)
 */
extern FIX16_RAMFUNC fix16_t fix16_atan2(fix16_t inY, fix16_t inX) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Returns the square root of the given value
 * @param  inValue input value
 * @return sqrt(inValue)
 */
extern FIX16_RAMFUNC fix16_t fix16_sqrt(fix16_t inValue) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Returns the exponent (e^) of the given value
 * @param  inValue input value
 * @return e^inValue
 */
extern FIX16_RAMFUNC fix16_t fix16_exp(fix16_t inValue) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Multiplies two fix16_t values
 * @param  inArg0 Value a
 * @param  inArg1 Value b
 * @return Result (a * b)
 */
extern FIX16_RAMFUNC fix16_t fix16_mul(fix16_t inArg0, fix16_t inArg1) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Multiplies two integers and returns a fix16_t
 * @param  inArg0 Value a
 * @param  inArg1 Value b
 * @return Result (a * b) in fix16_t format
 */
extern FIX16_RAMFUNC uint32_t fix16_mul_uint(uint32_t inArg0, uint32_t inArg1) FIXMATH_FUNC_ATTRS;

/**
 * @brief  Inline multiplication of two fix16_t numbers
 * @param  inArg0 Value a
 * @param  inArg1 Value b
 * @return Result (a * b) in fix16_t format
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC static inline fix16_t fix16_mul_inline(fix16_t inArg0, fix16_t inArg1)
{
      // Each argument is divided to 16-bit parts.
      // AB
      // * CD
      // -----------
      // BD 16 * 16 -> 32 bit products
      // CB
      // AD
      // AC
      // |----| 64 bit product
      int32_t A = (inArg0 >> 16), C = (inArg1 >> 16);
      uint32_t B = (inArg0 & 0xFFFF), D = (inArg1 & 0xFFFF);

      int32_t AC = A*C;
      int32_t AD_CB = A*D + C*B;
      uint32_t BD = B*D;

      int32_t product_hi = AC + (AD_CB >> 16);

      // Handle carry from lower 32 bits to upper part of result.
      uint32_t ad_cb_temp = AD_CB << 16;
      uint32_t product_lo = BD + ad_cb_temp;
      if (product_lo < BD)
        product_hi++;

#ifndef FIXMATH_NO_OVERFLOW
  // The upper 17 bits should all be the same (the sign).
  if (product_hi >> 31 != product_hi >> 15)
      PAC5XXX_GPIOE->OUT.b ^= 1;
//    return fix16_overflow;
#endif

      return (product_hi << 16) | (product_lo >> 16);
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#ifdef __cplusplus
}
#include "fix16.hpp"
#endif

#endif

