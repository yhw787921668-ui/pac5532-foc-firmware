
/**
 * @file  fix16_sqrt.c
 * @brief fix16_t square root functions
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

#include "fix16.h"
#include "int64.h"

#define FIXMATH_NO_CACHE

#ifndef FIXMATH_NO_CACHE
fix16_t _fix16_sqrt_cache_index[4096] = { 0 };
fix16_t _fix16_sqrt_cache_value[4096] = { 0 };
#endif



fix16_t fix16_sqrt(fix16_t inValue) {
        int neg = (inValue < 0);
        if(neg)
                inValue = -inValue;

        #ifndef FIXMATH_NO_CACHE
        fix16_t tempIndex = (((inValue >> 16) ^ (inValue >> 4)) & 0x00000FFF);
        if(_fix16_sqrt_cache_index[tempIndex] == inValue)
                return (neg ? -_fix16_sqrt_cache_value[tempIndex] : _fix16_sqrt_cache_value[tempIndex]);
        #endif

        lfm_int64 tempOp  = int64_const((inValue >> 16), (inValue << 16));
        lfm_int64 tempOut = int64_const(0, 0);
        lfm_int64 tempOne = int64_const(0x40000000UL, 0x00000000UL);

        while(int64_cmp_gt(tempOne, tempOp))
                tempOne = int64_shift(tempOne, -2);

        while(int64_cmp_ne(tempOne, int64_const(0, 0))) {
                if(int64_cmp_ge(tempOp, int64_add(tempOut, tempOne))) {


                        tempOp  = int64_sub(tempOp, int64_add(tempOut, tempOne));


                        tempOut = int64_add(tempOut, int64_shift(tempOne, 1));
                }
                tempOut = int64_shift(tempOut, -1);
                tempOne = int64_shift(tempOne, -2);
        }

        #ifndef FIXMATH_NO_CACHE
        _fix16_sqrt_cache_index[tempIndex] = inValue;
        _fix16_sqrt_cache_value[tempIndex] = int64_lo(tempOut);
        #endif

        return (neg ? -int64_lo(tempOut) : int64_lo(tempOut));
}

