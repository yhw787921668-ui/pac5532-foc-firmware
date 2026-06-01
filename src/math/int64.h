
/**
 * @file int64.h
 * @brief Definitions for 64-bit math functions
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

#ifndef __libfixmath_int64_h__
#define __libfixmath_int64_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include "pac5xxx.h"

#define FIXMATH_NO_64BIT

#ifndef FIXMATH_NO_64BIT
#define lfm_int64   int64_t     // libfixmath int64 defintion
#define ulfm_uint64  uint64_t   // libfixmath uint64 definition

/**
 * @brief  Creates a 64-bit constant from two 32-bit numbers
 * @param  hi upper 32-bits
 * @param  lo lower 32-bits
 * @return 64-bit constant
 * @note   For machines with 64-bit data
 */
static inline  lfm_int64 int64_const(int32_t hi, uint32_t lo) { return (((lfm_int64)hi << 32) | lo); }

/**
 * @brief  Creates a 64-bit constant from a 32-bit number
 * @param  x 32-bit number
 * @return 64-bit number
 * @note   For machines with 64-bit data
 */
static inline  lfm_int64 int64_from_int32(int32_t x) { return (lfm_int64)x; }

/**
 * @brief  Creates a 32-bit number from the upper 32-bits of a 64-bit number
 * @param  x 64-bit number
 * @return 32-bit number
 * @note   For machines with 64-bit data
 */
static inline  int32_t int64_hi(lfm_int64 x) { return (x >> 32); }

/**
 * @brief  Creates a 32-bit number from the lower 32-bits of a 64-bit number
 * @param  x 64-bit number
 * @return 32-bit number
 * @note   For machines with 64-bit data
 */
static inline uint32_t int64_lo(lfm_int64 x) { return (x & ((1ULL << 32) - 1)); }

/**
 * @brief  Adds two 64-bit numbers
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 64-bit sum
 * @note   For machines with 64-bit data
 */
static inline lfm_int64 int64_add(lfm_int64 x, lfm_int64 y)   { return (x + y);  }

/**
 * @brief  Negate a 64-bit number
 * @param  x 64-bit number
 * @return negated 64-bit number
 * @note   For machines with 64-bit data
 */
static inline lfm_int64 int64_neg(lfm_int64 x)              { return (-x);     }

/**
 * @brief  Subtract two 64-bit numbers (x - y)
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 64-bit difference (x - y )
 * @note   For machines with 64-bit data
 */
static inline lfm_int64 int64_sub(lfm_int64 x, lfm_int64 y)   { return (x - y);  }

/**
 * @brief  Shift a 64-bit number by the given number of bits
 * @param  x 64-bit number
 * @param  y number of bits to shift
 * @return 64-bit shifted number
 * @note   For machines with 64-bit data
 */
static inline lfm_int64 int64_shift(lfm_int64 x, int8_t y)  { return (y < 0 ? (x >> -y) : (x << y)); }

/**
 * @brief  Multiply two 32-bit numbers into a 64-bit number
 * @param  x 32-bit number
 * @param  y 32-bit number
 * @return 64-bit product
 * @note   For machines with 64-bit data
 */
static inline lfm_int64 int64_mul_i32_i32(int32_t x, int32_t y) { return (x * y);  }

/**
 * @brief  Multiply a 64-bit and 32-bit number
 * @param  x 64-bit number
 * @param  y 32-bit number
 * @return 64-bit product
 * @note   For machines with 64-bit data
 */
static inline lfm_int64 int64_mul_i64_i32(lfm_int64 x, int32_t y) { return (x * y);  }

/**
 * @brief  Divide a 64-bit number by a 32-bit number (x / y)
 * @param  x 64-bit number
 * @param  y 32-bit number
 * @return 64-bit result (x / y)
 * @note   For machines with 64-bit data
 */
static inline lfm_int64 int64_div_i64_i32(lfm_int64 x, int32_t y) { return (x / y);  }

/**
 * @brief  Test two 64-bit numbers for equality
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 0 if not equal, non-zero if equal
 * @note   For machines with 64-bit data
 */
static inline int int64_cmp_eq(lfm_int64 x, lfm_int64 y) { return (x == y); }

/**
 * @brief  Test two 64-bit numbers for non-equality
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 0 if equal, non-zero if not equal
 * @note   For machines with 64-bit data
 */
static inline int int64_cmp_ne(lfm_int64 x, lfm_int64 y) { return (x != y); }

/**
 * @brief  Compare two 64-bit numbers for greater than (x > y)
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 1 if x > y; 0 otherwise
 * @note   For machines with 64-bit data
 */
static inline int int64_cmp_gt(lfm_int64 x, lfm_int64 y) { return (x >  y); }

/**
 * @brief  Compare two 64-bit numbers for greater than or equal to (x >= y)
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 1 if x >= y; 0 otherwise
 * @note   For machines with 64-bit data
 */
static inline int int64_cmp_ge(lfm_int64 x, lfm_int64 y) { return (x >= y); }

/**
 * @brief  Compare two 64-bit numbers for less than (x < y)
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 1 if x < y; 0 otherwise
 * @note   For machines with 64-bit data
 */
static inline int int64_cmp_lt(lfm_int64 x, lfm_int64 y) { return (x <  y); }

/**
 * @brief  Compare two 64-bit numbers for less than or equal to (x <= y)
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 1 if x <= y; 0 otherwise
 * @note   For machines with 64-bit data
 */
static inline int int64_cmp_le(lfm_int64 x, lfm_int64 y) { return (x <= y); }
#else

typedef struct {
         int32_t hi;
        uint32_t lo;
} lfm_int64;            

// libfixmath definition of int64

/**
 * @brief  Creates a 64-bit constant from two 32-bit numbers
 * @param  hi upper 32-bits
 * @param  lo lower 32-bits
 * @return 64-bit constant
 */
PAC5XXX_RAMFUNC static inline lfm_int64 int64_const(int32_t hi, uint32_t lo) { return (lfm_int64){ hi, lo }; }

/**
 * @brief  Creates a 64-bit constant from a 32-bit number
 * @param  x 32-bit number
 * @return 64-bit number
 */
PAC5XXX_RAMFUNC static inline lfm_int64 int64_from_int32(int32_t x) { return (lfm_int64){ (x < 0 ? -1 : 0), x }; }

/**
 * @brief  Creates a 32-bit number from the upper 32-bits of a 64-bit number
 * @param  x 64-bit number
 * @return 32-bit number
 */
PAC5XXX_RAMFUNC static inline   int32_t int64_hi(lfm_int64 x) { return x.hi; }

/**
 * @brief  Creates a 32-bit number from the lower 32-bits of a 64-bit number
 * @param  x 64-bit number
 * @return 32-bit number
 */
PAC5XXX_RAMFUNC static inline  uint32_t int64_lo(lfm_int64 x) { return x.lo; }

/**
 * @brief  Test two 64-bit numbers for equality
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 0 if not equal, non-zero if equal
 */
PAC5XXX_RAMFUNC static inline int int64_cmp_eq(lfm_int64 x, lfm_int64 y) { return ((x.hi == y.hi) && (x.lo == y.lo)); }

/**
 * @brief  Test two 64-bit numbers for non-equality
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 0 if equal, non-zero if not equal
 */
PAC5XXX_RAMFUNC static inline int int64_cmp_ne(lfm_int64 x, lfm_int64 y) { return ((x.hi != y.hi) || (x.lo != y.lo)); }

/**
 * @brief  Compare two 64-bit numbers for greater than (x > y)
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 1 if x > y; 0 otherwise
 */
PAC5XXX_RAMFUNC static inline int int64_cmp_gt(lfm_int64 x, lfm_int64 y) { return ((x.hi > y.hi) || ((x.hi == y.hi) && (x.lo >  y.lo))); }

/**
 * @brief  Compare two 64-bit numbers for greater than or equal to (x >= y)
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 1 if x >= y; 0 otherwise
 */
PAC5XXX_RAMFUNC static inline int int64_cmp_ge(lfm_int64 x, lfm_int64 y) { return ((x.hi > y.hi) || ((x.hi == y.hi) && (x.lo >= y.lo))); }

/**
 * @brief  Compare two 64-bit numbers for less than (x < y)
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 1 if x < y; 0 otherwise
 */
PAC5XXX_RAMFUNC static inline int int64_cmp_lt(lfm_int64 x, lfm_int64 y) { return ((x.hi < y.hi) || ((x.hi == y.hi) && (x.lo <  y.lo))); }

/**
 * @brief  Compare two 64-bit numbers for less than or equal to (x <= y)
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 1 if x <= y; 0 otherwise
 */
PAC5XXX_RAMFUNC static inline int int64_cmp_le(lfm_int64 x, lfm_int64 y) { return ((x.hi < y.hi) || ((x.hi == y.hi) && (x.lo <= y.lo))); }

/**
 * @brief  Adds two 64-bit numbers
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 64-bit sum
 */
extern PAC5XXX_RAMFUNC lfm_int64 int64_add(lfm_int64 x, lfm_int64 y);

/**
 * @brief  Negate a 64-bit number
 * @param  x 64-bit number
 * @return negated 64-bit number
 */
PAC5XXX_RAMFUNC lfm_int64 int64_neg(lfm_int64 x);

/**
 * @brief  Subtract two 64-bit numbers (x - y)
 * @param  x 64-bit number
 * @param  y 64-bit number
 * @return 64-bit difference (x - y )
 */
PAC5XXX_RAMFUNC lfm_int64 int64_sub(lfm_int64 x, lfm_int64 y);

/**
 * @brief  Shift a 64-bit number by the given number of bits
 * @param  x 64-bit number
 * @param  y number of bits to shift
 * @return 64-bit shifted number
 */
PAC5XXX_RAMFUNC lfm_int64 int64_shift(lfm_int64 x, int8_t y);

/**
 * @brief  Multiply two 32-bit numbers into a 64-bit number
 * @param  x 32-bit number
 * @param  y 32-bit number
 * @return 64-bit product
 */
PAC5XXX_RAMFUNC lfm_int64 int64_mul_i32_i32(int32_t x, int32_t y);

/**
 * @brief  Multiply a 64-bit and 32-bit number
 * @param  x 64-bit number
 * @param  y 32-bit number
 * @return 64-bit product
 */
PAC5XXX_RAMFUNC lfm_int64 int64_mul_i64_i32(lfm_int64 x, int32_t y);

/**
 * @brief  Divide a 64-bit number by a 32-bit number (x / y)
 * @param  x 64-bit number
 * @param  y 32-bit number
 * @return 64-bit result (x / y)
 */
PAC5XXX_RAMFUNC lfm_int64 int64_div_i64_i32(lfm_int64 x, int32_t y);


#endif

#ifdef __cplusplus
}
#endif

#endif

