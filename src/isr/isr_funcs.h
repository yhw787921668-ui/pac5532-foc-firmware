
/**
 * @file isr_funcs.h
 * @brief Use for FOC utility functions, IdIq current filter, Id & Iq PI, vqmax
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

#ifndef ISR_FUNCS_H
#define ISR_FUNCS_H

#include "board.h"
#include "isr_config.h"
#include "motor_struct.h"


extern void isr_fptr_init(Struct_Motor* motor_ptr);

#if defined(FAMILY_52XX)
////////////////////////
// ADC Interrupts
////////////////////////
PAC5XXX_RAMFUNC static inline void clear_adc_int_flag() { PAC5XXX_ADC->ADCINT.w = ((PAC5XXX_ADC->ADCINT.w & ~ADCINT_ALL) | ADCINT_AS0INT | ADCINT_AS1INT); }


static inline int32_t read_adc_int_flag(void)
{
    return ((PAC5XXX_ADC->ADCINT.AS0INT && PAC5XXX_ADC->ADCINT.AS0INT_EN) || (PAC5XXX_ADC->ADCINT.AS1INT && PAC5XXX_ADC->ADCINT.AS1INT_EN));
}


////////////////////////
// Timer A Interrupts
////////////////////////
static inline void clear_timer_a_interrupt_flags(void)
{
#ifdef ENABLE_INT_TA_CCR0
    PAC5XXX_TIMERA->CCTRL0.CCINT = 1;
#endif
#ifdef ENABLE_INT_TA_CCR1
    PAC5XXX_TIMERA->CCTRL1.CCINT = 1;
#endif
#ifdef ENABLE_INT_TA_CCR2
    PAC5XXX_TIMERA->CCTRL2.CCINT = 1;
#endif
#ifdef ENABLE_INT_TA_CCR3
    PAC5XXX_TIMERA->CCTRL3.CCINT = 1;
#endif
#ifdef ENABLE_INT_TA_CCR4
    PAC5XXX_TIMERA->CCTRL4.CCINT = 1;
#endif
#ifdef ENABLE_INT_TA_CCR5
    PAC5XXX_TIMERA->CCTRL5.CCINT = 1;
#endif
#ifdef ENABLE_INT_TA_CCR6
    PAC5XXX_TIMERA->CCTRL6.CCINT = 1;
#endif
#ifdef ENABLE_INT_TA_CCR7
    PAC5XXX_TIMERA->CCTRL7.CCINT = 1;
#endif
#ifdef ENABLE_INT_TA_BASE
    PAC5XXX_TIMERA->CTL.INT = 1;
#endif
}

static inline int32_t read_timer_a_interrupt_flags(void)
{
    int32_t intflags = 0;

#ifdef ENABLE_INT_TA_CCR0
    intflags |= (PAC5XXX_TIMERA->CCTRL0.CCINT << 0);
#endif
#ifdef ENABLE_INT_TA_CCR1
    intflags |= (PAC5XXX_TIMERA->CCTRL1.CCINT << 1);
#endif
#ifdef ENABLE_INT_TA_CCR2
    intflags |= (PAC5XXX_TIMERA->CCTRL2.CCINT << 2);
#endif
#ifdef ENABLE_INT_TA_CCR3
    intflags |= (PAC5XXX_TIMERA->CCTRL3.CCINT << 3);
#endif
#ifdef ENABLE_INT_TA_CCR4
    intflags |= (PAC5XXX_TIMERA->CCTRL4.CCINT << 4);
#endif
#ifdef ENABLE_INT_TA_CCR5
    intflags |= (PAC5XXX_TIMERA->CCTRL5.CCINT << 5);
#endif
#ifdef ENABLE_INT_TA_CCR6
    intflags |= (PAC5XXX_TIMERA->CCTRL6.CCINT << 6);
#endif
#ifdef ENABLE_INT_TA_CCR7
    intflags |= (PAC5XXX_TIMERA->CCTRL7.CCINT << 7);
#endif
#ifdef ENABLE_INT_TA_BASE
    intflags |= (PAC5XXX_TIMERA->CTL.INT << 8);
#endif

    return intflags;
}

static inline int32_t read_timer_a_inten_base(void)
{
    return (PAC5XXX_TIMERA->CTL.INTEN);
}

static inline int32_t read_timer_a_inten_ccr0(void)
{
    return (PAC5XXX_TIMERA->CCTRL0.CCINTEN);
}

static inline int32_t read_timer_a_inten_ccr2(void)
{
    return (PAC5XXX_TIMERA->CCTRL2.CCINTEN);
}


////////////////////////
// Timer B Interrupts
////////////////////////
static inline void clear_timer_b_interrupt_flags(void)
{
#ifdef ENABLE_INT_TB_CCR0
    PAC5XXX_TIMERB->CCTRL0.CCINT = 1;
#endif
#ifdef ENABLE_INT_TB_CCR1
    PAC5XXX_TIMERB->CCTRL1.CCINT = 1;
#endif
#ifdef ENABLE_INT_TB_CCR2
    PAC5XXX_TIMERB->CCTRL2.CCINT = 1;
#endif
#ifdef ENABLE_INT_TB_CCR3
    PAC5XXX_TIMERB->CCTRL3.CCINT = 1;
#endif
#ifdef ENABLE_INT_TB_BASE
    PAC5XXX_TIMERB->CTL.INT = 1;
#endif
}

static inline int32_t read_timer_b_interrupt_flags(void)
{
    int32_t intflags = 0;

#ifdef ENABLE_INT_TB_CCR0
    intflags |= (PAC5XXX_TIMERB->CCTRL0.CCINT << 0);
#endif
#ifdef ENABLE_INT_TB_CCR1
    intflags |= (PAC5XXX_TIMERB->CCTRL1.CCINT << 1);
#endif
#ifdef ENABLE_INT_TB_CCR2
    intflags |= (PAC5XXX_TIMERB->CCTRL2.CCINT << 2);
#endif
#ifdef ENABLE_INT_TB_CCR3
    intflags |= (PAC5XXX_TIMERB->CCTRL3.CCINT << 3);
#endif
#ifdef ENABLE_INT_TB_BASE
    intflags |= (PAC5XXX_TIMERB->CTL.INT << 8);
#endif

    return intflags;
}

static inline int32_t read_timer_b_inten_base(void)
{
    return (PAC5XXX_TIMERB->CTL.INTEN);
}

static inline int32_t read_timer_b_inten_ccr0(void)
{
    return (PAC5XXX_TIMERB->CCTRL0.CCINTEN);
}

static inline int32_t read_timer_b_inten_ccr2(void)
{
    return (PAC5XXX_TIMERB->CCTRL2.CCINTEN);
}

#elif defined(FAMILY_55XX)
////////////////////////
// ADC Interrupts
////////////////////////
static inline void clear_adc_int_flag()
{
    PAC55XX_ADC->ADCINT.ADCIRQ0IF = 1;
    PAC55XX_ADC->ADCINT.ADCIRQ1IF = 1;
}


static inline int32_t read_adc_int_flag(void)
{
    return (PAC55XX_ADC->ADCINT.ADCIRQ0IF | PAC55XX_ADC->ADCINT.ADCIRQ1IF);
}


////////////////////////
// Timer A Interrupts
////////////////////////
static inline void clear_timer_a_interrupt_flags(void)
{
    PAC5XXX_TIMERA->INT.w = 0xFFFF;
}

static inline int32_t read_timer_a_interrupt_flags(void)
{
    return (PAC5XXX_TIMERA->INT.w);
}

static inline int32_t read_timer_a_inten_base(void)
{
    return (PAC5XXX_TIMERA->CTL.BASEIE);
}

static inline int32_t read_timer_a_inten_ccr0(void)
{
    return (PAC5XXX_TIMERA->CCTL0.CCINTEN);
}

static inline int32_t read_timer_a_inten_ccr2(void)
{
    return (PAC5XXX_TIMERA->CCTL2.CCINTEN);
}


////////////////////////
// Timer B Interrupts
////////////////////////
static inline void clear_timer_b_interrupt_flags(void)
{
    PAC5XXX_TIMERB->INT.w = 0xFFFF;
}

static inline int32_t read_timer_b_interrupt_flags(void)
{
    return (PAC5XXX_TIMERB->INT.w);
}

static inline int32_t read_timer_b_inten_base(void)
{
    return (PAC5XXX_TIMERB->CTL.BASEIE);
}

static inline int32_t read_timer_b_inten_ccr0(void)
{
    return (PAC5XXX_TIMERB->CCTL0.CCINTEN);
}

static inline int32_t read_timer_b_inten_ccr2(void)
{
    return (PAC5XXX_TIMERB->CCTL2.CCINTEN);
}

#endif


#if defined ENABLE_FOCCONTROLTIME_OVERFLOW_CHECK
#if defined(ENABLE_INT_TA_BASE)
static inline int32_t read_timer_a_base_interrupt_flags(void)
{
    int32_t intflags = read_timer_a_interrupt_flags();

    return ((intflags & BIT_08) && read_timer_a_inten_base());
}
#endif


#ifdef ENABLE_INT_TA_CCR0
static inline int32_t read_timer_a_ccr0_interrupt_flag(void)
{    
    int32_t intflags = read_timer_a_interrupt_flags();

    return ((intflags & BIT_00) && read_timer_a_inten_ccr0());
}
#endif


#ifdef ENABLE_INT_TA_CCR2
static inline int32_t read_timer_a_ccr2_interrupt_flag(void)
{
    int32_t intflags = read_timer_a_interrupt_flags();

    return ((intflags & BIT_02) && read_timer_a_inten_ccr2());
}
#endif


#if defined(ENABLE_INT_TB_BASE)
static inline int32_t read_timer_b_base_interrupt_flags(void)
{
    int32_t intflags = read_timer_b_interrupt_flags();

    return ((intflags & BIT_08) && read_timer_b_inten_base());
}
#endif


#ifdef ENABLE_INT_TB_CCR0
static inline int32_t read_timer_b_ccr0_interrupt_flag(void)
{    
    int32_t intflags = read_timer_b_interrupt_flags();

    return ((intflags & BIT_00) && read_timer_b_inten_ccr0());
}
#endif


#ifdef ENABLE_INT_TB_CCR2
static inline int32_t read_timer_b_ccr2_interrupt_flag(void)
{
    int32_t intflags = read_timer_b_interrupt_flags();

    return ((intflags & BIT_02) && read_timer_b_inten_ccr2());
}
#endif
#endif

#endif  //#ifndef ISR_FUNCS_H
