
/**
 * @file hall_timer_config.h
 * @brief Use for hall timer initialization and utility function
 */

//=============================================================================
// Copyright (C) 2019 - 2023, Qorvo, Inc.
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

#ifndef HALL_TIMER_CONFIG_H
#define HALL_TIMER_CONFIG_H

#include "pac5xxx.h"
#include "config_app.h"
#include "board.h"
#include "mpos_struct.h"

//#define HALL_TIMER_DIV1

#if ((MODULE_MPOS_HALL == ENABLED))

#if defined(FAMILY_52XX)

//==========================================
// Hall Speed Calculate Functions
//==========================================
#define HALL_TIMER          PAC5XXX_TIMERD
#define HALL_TIMER_COUNT    (HALL_TIMER->CTR)
#define HALL_TIMER_IRQN     TimerD_IRQn
#define HALL_TIMER_DIVIDER  128.0f
#define HALL_TIMER_FREQ     (HCLK_FREQ_HZ / HALL_TIMER_DIVIDER)
#define HALL_TIMER_PERIOD   QCONST((HALL_TIMER_FREQ / (CONFIG_HALL_TIMEOUT_EFREQ_HZ * 6)), Q00)
#define HALL_TIMER_MAX_PERIOD           FIX16_ONE - 5
#define HALL_TIMER_MIN_PERIOD           100


/**
 * @brief Use to get hall active period count
 */
PAC5XXX_RAMFUNC static inline uint16_t hall_get_counter_value(void)
{
    return  (uint16_t) (HALL_TIMER_COUNT);
}


/**
 * @brief Use to disable hall timer
 */
PAC5XXX_RAMFUNC static inline void hall_timer_disable(void)
{
    // Disable and clear hall timer when motor is disabled
    HALL_TIMER->CTL.CLR = 1;
}


/**
 * @brief Use to enable hall timer
 */
PAC5XXX_RAMFUNC static inline void hall_timer_enabled(void)
{
    // Enabled hall timer
    HALL_TIMER->CTL.CLR = 0;
}


/**
 * @brief Use to configure hall timer
 */
static inline void hall_timer_config(void)
{
    // Halt and clear Hall Timer and do not release until motor is started
    hall_timer_disable();

    // Configure timer for calculate the Speed
    HALL_TIMER->CTL.CLK = TxCTL_CS_HCLK ;
    HALL_TIMER->CTL.CLKDIV = TxCTL_PS_DIV128 ;
    HALL_TIMER->CTL.SYNC = 0; // Be sure to set sync bit first
    // Make sure we cover all conditions of hall timer value, 16 bit timer
    if ((HALL_TIMER_PERIOD > HALL_TIMER_MAX_PERIOD) ||
        (HALL_TIMER_PERIOD < HALL_TIMER_MIN_PERIOD))
    {
    	if (HALL_TIMER_PERIOD > HALL_TIMER_MAX_PERIOD)
    		HALL_TIMER->PRD = HALL_TIMER_MAX_PERIOD;
    	if (HALL_TIMER_PERIOD < HALL_TIMER_MIN_PERIOD)
    	    HALL_TIMER->PRD = HALL_TIMER_MIN_PERIOD;
        set_warning(WARN_HALL_TIM_PERD_INVALID);
    }
    else
    	HALL_TIMER->PRD = HALL_TIMER_PERIOD;

    HALL_TIMER->CTL.MODE = TxCTL_MODE_UP ;

    // Clear and enable hall timer base interrupt
    HALL_TIMER->CTL.INT = 1;
    HALL_TIMER->CTL.INTEN = 1;
    NVIC_EnableIRQ(HALL_TIMER_IRQN);
    NVIC_SetPriority(HALL_TIMER_IRQN,2);
}


/**
 * @brief Use to clear and re-enable hall timer
 */
PAC5XXX_RAMFUNC static inline void hall_timer_clear(void)
{
    // Clear and restart Timer D
    hall_timer_disable();
    hall_timer_enabled();
}


/**
 * @brief Use to clear hall timer interrupt
 */
PAC5XXX_RAMFUNC static inline void hall_timer_clr_interrupt(void)
{
    // Clear interrupt bit
    HALL_TIMER->CTL.INT = 1;
}
#endif  //#if defined(FAMILY_52XX)


#if defined(FAMILY_55XX)

#define HALL_TIMER                      PAC55XX_GPTIMERA
#define HALL_TIMER_COUNT                HALL_TIMER->CTR.w
#define HALL_TIMER_IRQn                 GPTimerA_IRQn
#ifndef HALL_TIMER_DIV1
#define HALL_TIMER_DIVIDER              2.0f
#else
#define HALL_TIMER_DIVIDER              1.0f
#endif
#define HALL_TIMER_FREQ                 (HCLK_FREQ_HZ / HALL_TIMER_DIVIDER)
#define TIMER_COUNT_DOWN_INITIAL_VALUE  QCONST((HALL_TIMER_FREQ / (CONFIG_HALL_TIMEOUT_EFREQ_HZ * 6)), Q00)
#define HALL_TIMER_MAX_PERIOD           0xFFFFF0    // 24 Bit Timer
#define HALL_TIMER_MIN_PERIOD           100


/**
 * @brief Use to get hall active period count
 */
PAC5XXX_RAMFUNC static inline uint16_t hall_get_counter_value(void)
{
    // Scale it to 16 bit  value from 24 bit
    return  (uint16_t) ((TIMER_COUNT_DOWN_INITIAL_VALUE - HALL_TIMER_COUNT) >> 8);
}


/**
 * @brief Use to disable hall timer
 */
PAC5XXX_RAMFUNC static inline void hall_timer_disable(void)
{
    // Disable and clear hall timer when motor is disabled
    HALL_TIMER->CTL.EN = 0;
}


/**
 * @brief Use to enable hall timer
 */
PAC5XXX_RAMFUNC static inline void hall_timer_enabled(void)
{
    // Enabled hall timer
    HALL_TIMER->CTL.EN = 1;
}


/**
 * @brief Use to configure hall timer
 */
static inline void hall_timer_config(void)
{
    // Halt and clear Hall Timer and do not release until motor is started
    hall_timer_disable();
    // Configure timer for calculate the Speed
    // Configure Timer GPA Controls
    // Make sure we cover all conditions of hall timer value, 16 bit timer
    if ((TIMER_COUNT_DOWN_INITIAL_VALUE > HALL_TIMER_MAX_PERIOD) ||
        (TIMER_COUNT_DOWN_INITIAL_VALUE < HALL_TIMER_MIN_PERIOD))
    {
    	if (TIMER_COUNT_DOWN_INITIAL_VALUE > HALL_TIMER_MAX_PERIOD)
    	    HALL_TIMER->CTL.CDV = HALL_TIMER_MAX_PERIOD;
    	if (TIMER_COUNT_DOWN_INITIAL_VALUE < HALL_TIMER_MIN_PERIOD)
    		HALL_TIMER->CTL.CDV = HALL_TIMER_MIN_PERIOD;
        set_warning(WARN_HALL_TIM_PERD_INVALID);
    }
    else
    	HALL_TIMER->CTL.CDV = TIMER_COUNT_DOWN_INITIAL_VALUE;
#ifndef HALL_TIMER_DIV1
    HALL_TIMER->CTL.DIV = GPTCTL_PS_DIV2;
#else
    HALL_TIMER->CTL.DIV = GPTCTL_PS_DIV1;
#endif
    HALL_TIMER->CTL.IE = 1;
    HALL_TIMER->CTL.IF = 0;
    HALL_TIMER->CTL.EN = 1;
    NVIC_EnableIRQ(HALL_TIMER_IRQn);
    NVIC_SetPriority(HALL_TIMER_IRQn,2);
}


/**
 * @brief Use to clear and re-enable hall timer
 */
PAC5XXX_RAMFUNC static inline void hall_timer_clear(void)
{
    // Clear and restart Timer D
    hall_timer_disable();
    hall_timer_enabled();
}


/**
 * @brief Use to clear hall timer interrupt
 */
PAC5XXX_RAMFUNC static inline void hall_timer_clr_interrupt(void)
{
    // Clear interrupt bit
    HALL_TIMER->CTL.IF = 1;
}
#endif  //#if defined(FAMILY_55XX)

#endif  //#if ((MODULE_MPOS_HALL == ENABLED))

#endif  //#ifndef HALL_TIMER_CONFIG_H
