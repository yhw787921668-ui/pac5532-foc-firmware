
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

#ifndef QEP_TIMER_CONFIG_H
#define QEP_TIMER_CONFIG_H

#include "pac5xxx.h"
#include "config_board.h"
#include "config_app.h"
#include "pac5xxx_driver_timer.h"
#include "mpos_struct.h"


#if (MODULE_MPOS_QEP == ENABLED) && defined(FAMILY_55XX)
#define QEP_MAX_COUNTER_COUNT           (1 << 16)
#if (QEP_TIMER_SELECT == TIMER_C)
#define QEP_TIMER                       PAC55XX_TIMERC
#define QEP_TIMERX                      TimerC
#define QEP_TIMER_IRQn                  TimerC_Qep_IRQn
#elif (QEP_TIMER_SELECT == TIMER_D)
#define QEP_TIMER                       PAC55XX_TIMERD
#define QEP_TIMERX                      TimerD
#define QEP_TIMER_IRQn                  TimerD_Qep_IRQn
#endif

#if (QEP_TIMER_SELECT == TIMER_C) || (QEP_TIMER_SELECT == TIMER_D)  
// VER1 is for all PAC55XX devices except PAC55XXA and PAC55XXB
// To use QEP on PAC55XXA and PAC55XXB, please uncomment "#define VER1"
// More details about VER1 defined in pac5xxx_timers.h
#ifndef VER1
#define QEP_TIMER_COUNT                 QEP_TIMER->QEPCTR
#define QEP_TIMER_INTFR                 QEP_TIMER->QEPIFR
#define QEP_TIMER_INTENR                QEP_TIMER->QEPIER
#else
#define QEP_TIMER_COUNT                 QEP_TIMER->QEPCTL.TICKS
#define QEP_TIMER_INTFR                 QEP_TIMER->QEPCTL
#define QEP_TIMER_INTENR                QEP_TIMER->QEPCTL
#endif
#endif
PAC5XXX_RAMFUNC static inline void qep_write_counter_value(uint16_t counter_value)
{
// VER1 is for all PAC55XX devices except PAC55XXA and PAC55XXB
// To use QEP on PAC55XXA and PAC55XXB, please uncomment "#define VER1"
// More details about VER1 defined in pac5xxx_timers.h
#ifdef VER1
    uint32_t temp_value;
#endif

    __disable_irq();     // Disable Interrupts
#ifndef VER1
    QEP_TIMER->QEPCTR = counter_value;
#else
    temp_value = QEP_TIMER->QEPCTL.w;
    temp_value = (QEP_TIMER->QEPCTL.w) | (counter_value << 16);
    QEP_TIMER->QEPCTL.w = temp_value;
#endif
    __enable_irq();     // Disable Interrupts
}

PAC5XXX_RAMFUNC static inline uint16_t qep_get_counter_value(void)
{
    return  (uint16_t) QEP_TIMER_COUNT;
}


PAC5XXX_RAMFUNC static inline void qep_timer_disable(void)
{
    // Disable and clear hall timer when motor is disabled
    QEP_TIMER->QEPCTL.QEPEN = FALSE;
}


PAC5XXX_RAMFUNC static inline void qep_timer_enabled(void)
{
    // Enabled QEP timer
    QEP_TIMER->QEPCTL.QEPEN = TRUE;
}


static inline void qep_timer_config(void)
{
    pac5xxx_timer_clock_config(QEP_TIMERX, TXCTL_CS_PCLK, TXCTL_PS_DIV1);                  // Configure timer clock input for PCLK, 1 divider

    // Halt and clear Hall Timer and do not release until motor is started
    qep_timer_disable();

    // Attention: CAN NOT count on both edges and phases during Linearization method2 because only one phase is used
#if defined(ENABLE_QEP_QUAD_MODE) && !defined(ENABLE_QEP_LINEARIZATION_TUNING)
    // Count on rising and falling edge
    QEP_TIMER->QEPCTL.CNTEDGE = TRUE;
    // Count on both A and B inputs
    QEP_TIMER->QEPCTL.CNTAB = TRUE;
#else
    // Count on rising edge only
    QEP_TIMER->QEPCTL.CNTEDGE = FALSE;
    // Count on A input only
    QEP_TIMER->QEPCTL.CNTAB = FALSE;
#endif

    // Reset counter on Index
    QEP_TIMER->QEPCTL.IDXRST = TRUE;

    // Configure QEP interrupts
    QEP_TIMER_INTENR.IDXEVIE = TRUE;
    QEP_TIMER_INTENR.DIRIE = FALSE;
#ifndef ENABLE_QEP_LINEARIZATION_TUNING
    QEP_TIMER_INTENR.PHAIE = FALSE;
#else
    // Enable PHA interrupts for QEP linearization
    QEP_TIMER_INTENR.PHAIE = TRUE;
#endif

    QEP_TIMER_INTENR.PHBIE = FALSE;
#ifndef VER1
    QEP_TIMER->QEPMAX = QEP_MAX_COUNTER_COUNT;
#endif
    // Enable QEP timer
    qep_timer_enabled();

    // Enable NVIC interrupt and configure priority
    NVIC_EnableIRQ(QEP_TIMER_IRQn);
    #ifdef ENABLE_QEP_LINEARIZATION_TUNING
    NVIC_SetPriority(QEP_TIMER_IRQn,1);
    #else
    NVIC_SetPriority(QEP_TIMER_IRQn,2);
    #endif
}


PAC5XXX_RAMFUNC static inline void qep_timer_counter_overflow_clr_interrupt(void)
{
    // Clear QEP counter over flow interrupt bit
    QEP_TIMER_INTFR.WRIF = TRUE;
}


PAC5XXX_RAMFUNC static inline void qep_index_event_clr_interrupt(void)
{
    // Clear QEP counter over flow interrupt bit
    QEP_TIMER_INTFR.IDXEVIF = TRUE;
}


PAC5XXX_RAMFUNC static inline void qep_phase_a_event_clr_interrupt(void)
{
    // Clear QEP phase a event interrupt flag bit
    QEP_TIMER_INTFR.PHAIF = TRUE;
}


PAC5XXX_RAMFUNC static inline void qep_phase_b_event_clr_interrupt(void)
{
    // Clear QEP phase a event interrupt flag bit
    QEP_TIMER_INTFR.PHBIF = TRUE;
}

PAC5XXX_RAMFUNC static inline int32_t qep_check_index_event_interrupt_flag(void)
{
    // Clear QEP counter over flow interrupt bit
      return (QEP_TIMER_INTFR.IDXEVIF);
}

PAC5XXX_RAMFUNC static inline int32_t qep_check_phase_a_event_interrupt_flag(void)
{
    // Check QEP Phase A interrupt bit
    return (QEP_TIMER_INTFR.PHAIF);
}

PAC5XXX_RAMFUNC static inline int32_t qep_check_phase_b_event_interrupt_flag(void)
{
    // Check QEP Phase B interrupt bit
    return (QEP_TIMER_INTFR.PHBIF);    
}
#endif  //#if (MODULE_MPOS_QEP == ENABLED) && defined(FAMILY_55XX)

#endif  //#ifndef QEP_TIMER_CONFIG_H
