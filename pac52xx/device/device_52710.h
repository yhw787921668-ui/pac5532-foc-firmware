
/**
 * @file device_52710.h
 * @brief Use for 52710 device definition
 */

//=============================================================================
// Copyright (C) 2017-2023, Qorvo, Inc.
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
 
#ifndef DEVICE_52710_H
#define DEVICE_52710_H

#include "pac52710.h"
#include "pac5xxx.h"
#include "device_options.h"
#include "family_52xx.h"
#include "pac5xxx_driver_timer.h"
#include "pac5xxx_driver_tile.h"
#include "pac5xxx_driver_socbridge.h"

//////////////////////////////////////////////////////////
// Resource: FOC Timer  FOC Timer Interrupt, ADC Interrupt
//////////////////////////////////////////////////////////
#define PWM_TIMER TIMER_A
#define FOC_PWM_Timer TimerA
#define FOC_PWM_Timer_Ptr PAC5XXX_TIMERA
#define FOC_PWM_Timer_IRQn TimerA_IRQn
#define ADC_IRQn ADC_IRQn
#define FOC_ASYM_SUPPLE_Timer TimerB
#define FOC_ASYM_SUPPLE_Timer_Ptr PAC5XXX_TIMERB
#define FOC_ASYM_SUPPLE_Timer_IRQn TimerB_IRQn

#define PUSER_VALUE_DIFFAMP_GAIN_FINAL PUSER_VALUE_DIFFAMP_GAIN
#define PUSER_VALUE_DIFFAMP_GAIN_FIXED                    FALSE

//////////////////////////////////////
// Gate Driver Configuration
//////////////////////////////////////
// Motor
#define MOTOR1_PWM_HS_PIN_PORT_A    0x38                                                                // Pin mask for HS gate drive PWM for Port A
#define MOTOR1_PWM_LS_PIN_PORT_A    0x07                                                                // Pin mask for LS gate drive PWM for Port A
#define MOTOR1_PWM_PIN_PORT_A       (MOTOR1_PWM_HS_PIN_PORT_A | MOTOR1_PWM_LS_PIN_PORT_A)
#define MOTOR1_PASEL_MASK           0x0595                                                              // Peripheral select mask for Port A
#define MOTOR1_PASEL_LS_MASK        0x0015                                                              // Peripheral select mask for Port A for low side only DTGA0LS, DTGA1LS and DTGA2LS

//==========================================
// Motor Driver Control Macro Functions
//==========================================
/**
 * @brief motor_psel_pwm, use to select port_a pins as PWM output
 */
static inline void motor_psel_pwm(void)
{
    // Configure Port PSELs to output PWMs to Drivers
    pac5xxx_timer_io_select_pwma4_pa3();                                                    // Motor-1: UH/PWMA4: PA3
    pac5xxx_timer_io_select_pwma5_pa4();                                                    // Motor-1: VH/PWMA5: PA4
    pac5xxx_timer_io_select_pwma6_pa5();                                                    // Motor-1: WH/PWMA6: PA5
    pac5xxx_timer_io_select_pwma0_pa0();                                                    // Motor-1: UL/PWMA0: PA0
    pac5xxx_timer_io_select_pwma1_pa1();                                                    // Motor-1: VL/PWMA1: PA1
    pac5xxx_timer_io_select_pwma2_pa2();                                                    // Motor-1: WL/PWMA2: PA2
}

/**
 * @brief motor_outen_enable, use to enable port_a output
 */
static inline void motor_outen_enable(void)
{
    PAC5XXX_GPIOA->OUTEN.b |= (MOTOR1_PWM_PIN_PORT_A);
}

/**
 * @brief motor_psel_gpio, use to select port_a as gpio
 */
static inline void motor_psel_gpio(void)
{
    // Revert PWM Driver outputs to GPIOs
    PAC5XXX_GPIOA->PSEL.w &= ~MOTOR1_PASEL_MASK;
}

/**
 * @brief motor_out_clear, use to clear port_a output
 */
static inline void motor_out_clear(void)
{
    PAC5XXX_GPIOA->OUT.b &= ~(MOTOR1_PWM_PIN_PORT_A);
}

/**
 * @brief motor_psel_pwm_LS, use to select port_a pa0,pa1,pa2 in PWM mode
 */
static inline void motor_psel_pwm_LS(void)
{
    // Configure Port PSELs to output Low Side PWMs to Drivers
    pac5xxx_timer_io_select_pwma0_pa0();                                                    // Motor-1: UL/PWMA0: PA0
    pac5xxx_timer_io_select_pwma1_pa1();                                                    // Motor-1: VL/PWMA1: PA1
    pac5xxx_timer_io_select_pwma2_pa2();                                                    // Motor-1: WL/PWMA2: PA2
}

// Empty function for pac52xx, just a place holder
/**
 * @brief motor_modesel_gpio, this is empty function as place holder
 */
static inline void motor_modesel_gpio(void)
{
}

/**
 * @brief enable_bbm, this is enable break before make functionality
 */
static inline void enable_bbm()
{
    pac5xxx_tile_register_write(ADDR_ENBBM, 1);
}

// CAFE specific definition
//=====================================
// Low OC Protection Signal (AIOxx_LPROT)
//=====================================
#define AIO50_LPROT_NONE        0
#define AIO50_LPROT_PR1         BIT_05
#define AIO50_LPROT_PR2         BIT_04
#define AIO50_LPROT_PR1PR2      (BIT_04 | BIT_05)

//=====================================
// High OC Protection Signal (AIOxx_HPROT)
//=====================================
#define AIO50_HPROT_NONE        0
#define AIO50_HPROT_PR1         BIT_07
#define AIO50_HPROT_PR2         BIT_06
#define AIO50_HPROT_PR1PR2      (BIT_06 | BIT_07)

//=====================================
// Driver protection while OC 
//=====================================
#define DRV_HIGHSIDE_PR1        BIT_07
#define DRV_HIGHSIDE_PR2        BIT_06
#define DRV_LOWSIDE_PR1         BIT_05
#define DRV_LOWSIDE_PR2         BIT_04

//=====================================
// Vp voltage selection 
//=====================================
#define VP_VOLTAGE_9V        (0x01 << 6)
#define VP_VOLTAGE_12V       (0x02 << 6)
#define VP_VOLTAGE_15V       (0x03 << 6)
#define VP_CONFIG_BITS_MASK         0x3F

//==========================================================
// Min duty cycle: Version A can be set as 0, otherwise as 1
//==========================================================
#define DUTY_CYCLE_MIN_TICK     0

//=====================================
// VDS sensing  Configuration 
//=====================================
/* ===========================  VDSCFG1(ADDR=0x40)  =========================== */
// ---------------------------------------------------------------------------- //
#define VDSCFG1_DISCBC52_Pos                7                                   //
#define VDSCFG1_DISCBC52_EN                 (0 << VDSCFG1_DISCBC52_Pos)         //
#define VDSCFG1_DISCBC52_DIS                (1 << VDSCFG1_DISCBC52_Pos)         //
// ---------------------------------------------------------------------------- //
#define VDSCFG1_DISCBC41_Pos                6                                   //
#define VDSCFG1_DISCBC41_EN                 (0 << VDSCFG1_DISCBC41_Pos)         //
#define VDSCFG1_DISCBC41_DIS                (1 << VDSCFG1_DISCBC41_Pos)         //
// ---------------------------------------------------------------------------- //
#define VDSCFG1_DISCBC30_Pos                5                                   //
#define VDSCFG1_DISCBC30_EN                 (0 << VDSCFG1_DISCBC30_Pos)         //
#define VDSCFG1_DISCBC30_DIS                (1 << VDSCFG1_DISCBC30_Pos)         //
// ---------------------------------------------------------------------------- //
#define VDSCFG1_VDEN_Pos                    4                                   //
#define VDSCFG1_VDEN_DIS                    (0 << VDSCFG1_VDEN_Pos)             //
#define VDSCFG1_VDEN_EN                     (1 << VDSCFG1_VDEN_Pos)             //
// ---------------------------------------------------------------------------- //
#define VDSCFG1_VDSBLNK_Pos                 0                                   //
#define VDSCFG1_VDSBLNK_BITS_Msk            ~(15 << VDSCFG1_VDSBLNK_Pos)
typedef enum
{
    VDSCFG1_VDSBLNK_250nS                   = (0 << VDSCFG1_VDSBLNK_Pos),       //
    VDSCFG1_VDSBLNK_500nS                   = (1 << VDSCFG1_VDSBLNK_Pos),       //
    VDSCFG1_VDSBLNK_750nS                   = (2 << VDSCFG1_VDSBLNK_Pos),       //
    VDSCFG1_VDSBLNK_1000nS                  = (3 << VDSCFG1_VDSBLNK_Pos),       //
    VDSCFG1_VDSBLNK_1250nS                  = (4 << VDSCFG1_VDSBLNK_Pos),       //
    VDSCFG1_VDSBLNK_1500nS                  = (5 << VDSCFG1_VDSBLNK_Pos),       //
    VDSCFG1_VDSBLNK_1750nS                  = (6 << VDSCFG1_VDSBLNK_Pos),       //
    VDSCFG1_VDSBLNK_2000nS                  = (7 << VDSCFG1_VDSBLNK_Pos),       //
    VDSCFG1_VDSBLNK_2250nS                  = (8 << VDSCFG1_VDSBLNK_Pos),       //
    VDSCFG1_VDSBLNK_2500nS                  = (9 << VDSCFG1_VDSBLNK_Pos),       //
    VDSCFG1_VDSBLNK_2750nS                  = (10 << VDSCFG1_VDSBLNK_Pos),      //
    VDSCFG1_VDSBLNK_3000nS                  = (11 << VDSCFG1_VDSBLNK_Pos),      //
    VDSCFG1_VDSBLNK_3250nS                  = (12 << VDSCFG1_VDSBLNK_Pos),      //
    VDSCFG1_VDSBLNK_3500nS                  = (13 << VDSCFG1_VDSBLNK_Pos),      //
    VDSCFG1_VDSBLNK_3750nS                  = (14 << VDSCFG1_VDSBLNK_Pos),      //
    VDSCFG1_VDSBLNK_4000nS                  = (15 << VDSCFG1_VDSBLNK_Pos)       //
} VDSCFG1_VDSBLNK_Type;
// ---------------------------------------------------------------------------- //


/* ===========================  VDSCFG2(ADDR=0x41)  =========================== */
// ---------------------------------------------------------------------------- //
#define VDSCFG2_VDSTLL_Pos                  4                                   //
#define VDSCFG2_VDSTLL_BITS_Msk             ~(15 << VDSCFG2_VDSTLL_Pos)
typedef enum
{
    VDSCFG2_VDSTLL_80mV                     = (0 << VDSCFG2_VDSTLL_Pos),        //
    VDSCFG2_VDSTLL_120mV                    = (1 << VDSCFG2_VDSTLL_Pos),        //
    VDSCFG2_VDSTLL_160mV                    = (2 << VDSCFG2_VDSTLL_Pos),        //
    VDSCFG2_VDSTLL_200mV                    = (3 << VDSCFG2_VDSTLL_Pos),        //
    VDSCFG2_VDSTLL_240mV                    = (4 << VDSCFG2_VDSTLL_Pos),        //
    VDSCFG2_VDSTLL_280mV                    = (5 << VDSCFG2_VDSTLL_Pos),        //
    VDSCFG2_VDSTLL_320mV                    = (6 << VDSCFG2_VDSTLL_Pos),        //
    VDSCFG2_VDSTLL_360mV                    = (7 << VDSCFG2_VDSTLL_Pos),        //
    VDSCFG2_VDSTLL_400mV                    = (8 << VDSCFG2_VDSTLL_Pos),        //
    VDSCFG2_VDSTLL_500mV                    = (9 << VDSCFG2_VDSTLL_Pos),        //
    VDSCFG2_VDSTLL_600mV                    = (10 << VDSCFG2_VDSTLL_Pos),       //
    VDSCFG2_VDSTLL_700mV                    = (11 << VDSCFG2_VDSTLL_Pos),       //
    VDSCFG2_VDSTLL_800mV                    = (12 << VDSCFG2_VDSTLL_Pos),       //
    VDSCFG2_VDSTLL_900mV                    = (13 << VDSCFG2_VDSTLL_Pos),       //
    VDSCFG2_VDSTLL_1000mV                   = (14 << VDSCFG2_VDSTLL_Pos),       //
    VDSCFG2_VDSTLL_1100mV                   = (15 << VDSCFG2_VDSTLL_Pos)        //
} VDSCFG2_VDSTLL_Type;
// ---------------------------------------------------------------------------- //
#define VDSCFG2_VDSTLH_Pos                  0                                   //
#define VDSCFG2_VDSTLH_BITS_Msk             ~(15 << VDSCFG2_VDSTLH_Pos)
typedef enum
{
    VDSCFG2_VDSTLH_80mV                     = (0 << VDSCFG2_VDSTLH_Pos),        //
    VDSCFG2_VDSTLH_120mV                    = (1 << VDSCFG2_VDSTLH_Pos),        //
    VDSCFG2_VDSTLH_160mV                    = (2 << VDSCFG2_VDSTLH_Pos),        //
    VDSCFG2_VDSTLH_200mV                    = (3 << VDSCFG2_VDSTLH_Pos),        //
    VDSCFG2_VDSTLH_240mV                    = (4 << VDSCFG2_VDSTLH_Pos),        //
    VDSCFG2_VDSTLH_280mV                    = (5 << VDSCFG2_VDSTLH_Pos),        //
    VDSCFG2_VDSTLH_320mV                    = (6 << VDSCFG2_VDSTLH_Pos),        //
    VDSCFG2_VDSTLH_360mV                    = (7 << VDSCFG2_VDSTLH_Pos),        //
    VDSCFG2_VDSTLH_400mV                    = (8 << VDSCFG2_VDSTLH_Pos),        //
    VDSCFG2_VDSTLH_500mV                    = (9 << VDSCFG2_VDSTLH_Pos),        //
    VDSCFG2_VDSTLH_600mV                    = (10 << VDSCFG2_VDSTLH_Pos),       //
    VDSCFG2_VDSTLH_700mV                    = (11 << VDSCFG2_VDSTLH_Pos),       //
    VDSCFG2_VDSTLH_800mV                    = (12 << VDSCFG2_VDSTLH_Pos),       //
    VDSCFG2_VDSTLH_900mV                    = (13 << VDSCFG2_VDSTLH_Pos),       //
    VDSCFG2_VDSTLH_1000mV                   = (14 << VDSCFG2_VDSTLH_Pos),       //
    VDSCFG2_VDSTLH_1100mV                   = (15 << VDSCFG2_VDSTLH_Pos)        //
} VDSCFG2_VDSTLH_Type;
// ---------------------------------------------------------------------------- //


/* ===========================  CFGCBC(ADDR=0x42)  ============================ */
// ---------------------------------------------------------------------------- //
#define CFGCBC_VDSTHIEN_Pos                 7                                   //
#define CFGCBC_VDSTHIEN_Msk                 (0 << CFGCBC_VDSTHIEN_Pos)          //
#define CFGCBC_VDSTHIEN_EN                  (1 << CFGCBC_VDSTHIEN_Pos)          //
// ---------------------------------------------------------------------------- //
#define CFGCBC_VDSTLIEN_Pos                 6                                   //
#define CFGCBC_VDSTLIEN_Msk                 (0 << CFGCBC_VDSTLIEN_Pos)          //
#define CFGCBC_VDSTLIEN_EN                  (1 << CFGCBC_VDSTLIEN_Pos)          //
// ---------------------------------------------------------------------------- //
#define CFGCBC_CBCEN_VDSTH_Pos              5                                   //
#define CFGCBC_CBCEN_VDSTH_DIS              (0 << CFGCBC_CBCEN_VDSTH_Pos)       //
#define CFGCBC_CBCEN_VDSTH_EN               (1 << CFGCBC_CBCEN_VDSTH_Pos)       //
// ---------------------------------------------------------------------------- //
#define CFGCBC_CBCEN_VDSTL_Pos              4                                   //
#define CFGCBC_CBCEN_VDSTL_DIS              (0 << CFGCBC_CBCEN_VDSTL_Pos)       //
#define CFGCBC_CBCEN_VDSTL_EN               (1 << CFGCBC_CBCEN_VDSTL_Pos)       //
// ---------------------------------------------------------------------------- //
#define CFGCBC_CBCEN_HPROT_Pos              3                                   //
#define CFGCBC_CBCEN_HPROT_DIS              (0 << CFGCBC_CBCEN_HPROT_Pos)       //
#define CFGCBC_CBCEN_HPROT_EN               (1 << CFGCBC_CBCEN_HPROT_Pos)       //
// ---------------------------------------------------------------------------- //
#define CFGCBC_CBCEN_LPROT_Pos              2                                   //
#define CFGCBC_CBCEN_LPROT_DIS              (0 << CFGCBC_CBCEN_LPROT_Pos)       //
#define CFGCBC_CBCEN_LPROT_EN               (1 << CFGCBC_CBCEN_LPROT_Pos)       //
// ---------------------------------------------------------------------------- //
#define CFGCBC_CBCTYPE_Pos                  0                                   //
#define CFGCBC_CBCTYPE_BITS_Msk             ~(3 << CFGCBC_CBCTYPE_Pos)
typedef enum
{
    CFGCBC_CBCTYPE_ALL_OFF                  = (0 << VDSCFG2_VDSTLH_Pos),        //
    CFGCBC_CBCTYPE_HS_OFF                   = (1 << VDSCFG2_VDSTLH_Pos),        //
    CFGCBC_CBCTYPE_LS_OFF                   = (2 << VDSCFG2_VDSTLH_Pos),        //
    CFGCBC_CBCTYPE_RESERVED                 = (3 << VDSCFG2_VDSTLH_Pos)         //
} CFGCBC_CBCTYPE_Type;
// ---------------------------------------------------------------------------- //


/* ===========================  CFGFLAG(ADDR=0x43)  =========================== */
// ---------------------------------------------------------------------------- //
#define CFGFLAG_VDSTH_Pos                   1                                   //
#define CFGFLAG_VDSTH                       (1 << CFGFLAG_VDSTH_Pos)            //
// ---------------------------------------------------------------------------- //
#define CFGFLAG_VDSTL_Pos                   0                                   //
#define CFGFLAG_VDSTL                       (1 << CFGFLAG_VDSTL_Pos)            //
// ---------------------------------------------------------------------------- //

#endif  //#ifndef DEVICE_52710_H
