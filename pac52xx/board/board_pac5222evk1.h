
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
 
#ifndef BOARD_PAC5222EVK1_H
#define BOARD_PAC5222EVK1_H

#include "board_options.h"
#include "pac5xxx.h"
#include "config_debug.h"

//==========================================================================
//
// Board Information
//
//==========================================================================

// Gate Driver - Pin Mapping
// Driver Phase U = DRL0/DRH3
// Driver Phase V = DRL1/DRH4
// Driver Phase W = DRL2/DRH5

// Phase Current - Port Mapping
// Phase Current U = AIO10
// Phase Current V = AIO32
// Phase Current W = AIO54

// Bus Voltage - Port Mapping
// Vbus = PC4

// Phase Voltage - Port Mapping
// Phase Voltage U = AIO7
// Phase Voltage V = AIO8
// Phase Voltage W = AIO9

// AIO Resource Assignment
// AIO10 = ADC sampling, OC & CBC protection (Iu)
// AIO32 = ADC sampling, OC & CBC protection (Iv)
// AIO54 = ADC sampling, OC & CBC protection (Iw)
// AIO6 = Input (Not Used)
// AIO7 = ADC sampling (Vu)
// AIO8 = ADC sampling (Vv)
// AIO9 = ADC sampling (Vw)

// Sequencer Entry Assignments
// Motor Phase Current U : Seq0/N0
// Motor Phase Current V : Seq0/N1
// Motor Phase Current W : Seq0/N2
// Vbus                  : Seq1/N0
// Motor Phase Voltage U : Seq1/N1
// Motor Phase Voltage V : Seq1/N2
// Motor Phase Voltage W : Seq1/N3

// Special Function - Port Mapping
// Debug IO1 = PE0
// Debug IO2 = PE3
// PPM Port = NONE
// Debug DAC1 = PD2
// Debug DAC2 = PD3
// Debug DAC3 = PD7

//==========================================================================
//
// Board Properties
//
//==========================================================================

//=====================================
// Board Configuration
//=====================================
#define DEVICE_SELECT                   DEVICE_5222
#define CAPABLE_VSIM                    TRUE
#define CAPABLE_IPD                     TRUE
#define CONFIG_SHUNT                    CONFIG_SHUNT_3SHUNT
#define CAPABLE_POWERMETHOD2            FALSE

//=====================================
// DC Bus Voltage
//=====================================
#define MIN_VBUS                        VDC_10V
#define MAX_VBUS                        VDC_60V

//=====================================
// Current Sensing Resistor Values
//=====================================
#define PCB_RSENSE_OC_OHMS              0.010f
#define PCB_RSENSE_IUVW_OHMS            0.010f

//=====================================
// Voltage Sensing Resistor Values
//=====================================
#define R1_PLUS_R2_BY_R2_RATIO_VBUS     24.26f      // Resistor ratio for VDC bus voltage sensing
#define R1_PLUS_R2_BY_R2_RATIO_SIM      24.26f      // Resistor ratio for phase voltage sensing

//=====================================
// AIO RC Filter Resistor Value
//=====================================
#define AIORES_VALUE                    100.0f      // Resistor value of diffamp AIO RC filter

//=====================================
// Deadtime
//=====================================
#define MOTOR1_LED_NSEC                 400         // Rising edge deadtime in nsec
#define MOTOR1_TED_NSEC                 400         // Falling edge deadtime in nsec

//=====================================
// SIM BEMF voltage - current mode minimum in-motion detection threshold
//=====================================
#define SIM_BEMF_DET_THRESHOLD_ADCCNT_V               25                      // Threshold for minimum phase-to-CT voltage in ADC counts
#define SIM_BEMF_DET_THRESHOLD_ADCCNT_I               30                      // Threshold for minimum phase current in ADC counts

//=====================================
// Inverter Section / Configuration (For MID1)
//=====================================
#define INVERTER_TYPE_SEL               INVERTER_TYPE_MOS
#define INVERTER_MOS_RDSON_OHMS_FLT     0.0016f
#define INVERTER_MOS_VDIODE_VOLTS_FLT   0.85f
//#define INVERTER_TYPE_SEL               INVERTER_TYPE_IGBT
//#define INVERTER_IGBT_VCE_VOLTS_FLT     0.2f
//#define INVERTER_IGBT_VDIODE_VOLTS_FLT  0.6f

//==========================================================================
//
// ADC Offset Configuration
//
//==========================================================================

//=====================================
// ADC Offset Channel Selection
//=====================================
// Select all channels where offset must be subtracted in the firmware calculations. Offset can either be calibrated automatically or manually set.
#define CONFIG_OFFSET                   (CHANNEL_AIO10 | CHANNEL_AIO32 | CHANNEL_AIO54)
#define ENABLE_ADCCAL_AIO10
#define ENABLE_ADCCAL_AIO32
#define ENABLE_ADCCAL_AIO54

//=====================================
// ADC Offset Values
//=====================================
#define ADC_OFFSET_DEFAULT_DIFFAMP      512             // Midrail offset
#define ADC_OFFSET_MIN_DIFFAMP          500             // Minimum value of current offset
#define ADC_OFFSET_MAX_DIFFAMP          524             // Maximum value of current offset

//==========================================================================
//
// I/O Port Mapping
//
//==========================================================================

//=====================================
// HALL Input Port Definitions
//=====================================
#define HALLA_PORT                      PORT_PD2
#define HALLB_PORT                      PORT_PD3
#define HALLC_PORT                      PORT_PD7

#define PAC5XXX_GPIO_HALLA              PAC5XXX_GPIOD
#define HALLA_PORTNUM                   P2

#define PAC5XXX_GPIO_HALLB              PAC5XXX_GPIOD
#define HALLB_PORTNUM                   P3

#define PAC5XXX_GPIO_HALLC              PAC5XXX_GPIOD
#define HALLC_PORTNUM                   P7

#define HALLA_PORT_BIT                  BIT_02
#define HALLB_PORT_BIT                  BIT_03
#define HALLC_PORT_BIT                  BIT_07
#define PAC5XXX_GPIO_HALLA_POLARITY     PAC5XXX_GPIO_HALLA->INTP.HALLA_PORTNUM
#define PAC5XXX_GPIO_HALLB_POLARITY     PAC5XXX_GPIO_HALLB->INTP.HALLB_PORTNUM
#define PAC5XXX_GPIO_HALLC_POLARITY     PAC5XXX_GPIO_HALLC->INTP.HALLC_PORTNUM
#define PAC5XXX_GPIO_HALL               PAC5XXX_GPIO_HALLA
#define PAC5XXX_GPIO_INT_FLAG           PAC5XXX_GPIO_HALL->INTF.b
#define Hall_Gpio_IRQn                  GpioD_IRQn

//=====================================
// Debug IO Port Definitions
//=====================================
#define DEBUGIO1_PORT                   PORT_PE0
#define DEBUGIO2_PORT                   PORT_PE3

//=====================================
// PPM Input Port (Optional)
//=====================================

//=====================================
// Push Button Input Port Definitions
// Active Low, PullUp Internally
//=====================================
#define PAC5XXX_GPIO_PUSH_BUTTON        PAC5XXX_GPIOE   // Port E
#define PUSH_BUTTON_PORTNUM             P5              // Port P5

//=====================================
// PWM DAC Port Definitions
//=====================================
#define PWMDAC1_PORT                    PORT_PD2
#define PWMDAC2_PORT                    PORT_PD3
#define PWMDAC3_PORT                    PORT_PD7

#define PAC5XXX_GPIO_DAC1               PAC5XXX_GPIOD
#define PWMDAC1_PORTNUM                 P2
#define PWMDAC1_PSEL                    3
#define PWMDAC1_TIMER                   PAC5XXX_TIMERB
#define PWMDAC1_CTR                     CTR0

#define PAC5XXX_GPIO_DAC2               PAC5XXX_GPIOD
#define PWMDAC2_PORTNUM                 P3
#define PWMDAC2_PSEL                    3
#define PWMDAC2_TIMER                   PAC5XXX_TIMERB
#define PWMDAC2_CTR                     CTR1

#define PAC5XXX_GPIO_DAC3               PAC5XXX_GPIOD
#define PWMDAC3_PORTNUM                 P7
#define PWMDAC3_PSEL                    2
#define PWMDAC3_TIMER                   PAC5XXX_TIMERD
#define PWMDAC3_CTR                     CTR0

//=====================================
// DAS IO Port Definitions
//=====================================
#define DEBUGDASIO_PORT                 PORT_PE4
#define DEBUGDASIO_PORTNAME             PAC5XXX_GPIOE                   // Debug IO #1 port name
#define DEBUGDASIO_PORTNUM              P4                              // Debug IO #1 port number

//==========================================
// PWM DAC Driver Control Macro Functions
//==========================================
// PWM DAC Port Controls
static inline void dac_psel_pwm(void)
{
#if defined (PWMDAC1_PORT)
    PAC5XXX_GPIO_DAC1->PSEL.PWMDAC1_PORTNUM = PWMDAC1_PSEL;
#endif

#if defined (PWMDAC2_PORT)
    PAC5XXX_GPIO_DAC2->PSEL.PWMDAC2_PORTNUM = PWMDAC2_PSEL;
#endif

#if defined (PWMDAC3_PORT)
    PAC5XXX_GPIO_DAC3->PSEL.PWMDAC3_PORTNUM = PWMDAC3_PSEL;
#endif
}

static inline void dac_psel_gpio(void)
{
#if defined (PWMDAC1_PORT)
    PAC5XXX_GPIO_DAC1->PSEL.PWMDAC1_PORTNUM = 0;
#endif

#if defined (PWMDAC2_PORT)
    PAC5XXX_GPIO_DAC2->PSEL.PWMDAC2_PORTNUM = 0;
#endif

#if defined (PWMDAC3_PORT)
    PAC5XXX_GPIO_DAC3->PSEL.PWMDAC3_PORTNUM = 0;
#endif
}

static inline void dac_out_clear(void)
{
#if defined (PWMDAC1_PORT)
    PAC5XXX_GPIO_DAC1->OUT.PWMDAC1_PORTNUM = 0;
#endif

#if defined (PWMDAC2_PORT)
    PAC5XXX_GPIO_DAC2->OUT.PWMDAC2_PORTNUM = 0;
#endif

#if defined (PWMDAC3_PORT)
    PAC5XXX_GPIO_DAC3->OUT.PWMDAC3_PORTNUM = 0;
#endif
}

static inline void dac_outen_enable(void)
{
#if defined (PWMDAC1_PORT)
    PAC5XXX_GPIO_DAC1->OUTEN.PWMDAC1_PORTNUM = 1;
#endif

#if defined (PWMDAC2_PORT)
    PAC5XXX_GPIO_DAC2->OUTEN.PWMDAC2_PORTNUM = 1;
#endif

#if defined (PWMDAC3_PORT)
    PAC5XXX_GPIO_DAC3->OUTEN.PWMDAC3_PORTNUM = 1;
#endif
}

#ifdef ENABLE_DEBUG_DAS
    PAC5XXX_RAMFUNC static inline void config_debugDASio(void)  { DEBUGDASIO_PORTNAME->OUTEN.DEBUGDASIO_PORTNUM = 1; }
    PAC5XXX_RAMFUNC static inline void debugDASio_on(void)      { DEBUGDASIO_PORTNAME->OUT.DEBUGDASIO_PORTNUM = 1; }
    PAC5XXX_RAMFUNC static inline void debugDASio_off(void)     { DEBUGDASIO_PORTNAME->OUT.DEBUGDASIO_PORTNUM = 0; }
    PAC5XXX_RAMFUNC static inline void debugDASio_toggle(void)  { DEBUGDASIO_PORTNAME->OUT.DEBUGDASIO_PORTNUM ^= 1; }
#else
    PAC5XXX_RAMFUNC static inline void config_debugDASio(void)  { }
    PAC5XXX_RAMFUNC static inline void debugDASio_on(void)      { }
    PAC5XXX_RAMFUNC static inline void debugDASio_off(void)     { }
    PAC5XXX_RAMFUNC static inline void debugDASio_toggle(void)  { }
#endif

PAC5XXX_RAMFUNC static inline void pwmdac_1_set_duty(uint16_t duty)             { PWMDAC1_TIMER->PWMDAC1_CTR = duty; }
PAC5XXX_RAMFUNC static inline void pwmdac_2_set_duty(uint16_t duty)             { PWMDAC2_TIMER->PWMDAC2_CTR = duty; }
PAC5XXX_RAMFUNC static inline void pwmdac_3_set_duty(uint16_t duty)             { PWMDAC3_TIMER->PWMDAC3_CTR = duty; }

//=============================================
// Motor Driver Duty Cycle Macro Functions
//=============================================
PAC5XXX_RAMFUNC static inline void m1_u_set_duty(uint16_t duty)             { PAC5XXX_TIMERA->CTR4 = duty; }
PAC5XXX_RAMFUNC static inline void m1_v_set_duty(uint16_t duty)             { PAC5XXX_TIMERA->CTR5 = duty; }
PAC5XXX_RAMFUNC static inline void m1_w_set_duty(uint16_t duty)             { PAC5XXX_TIMERA->CTR6 = duty; }
//=====================================
// Gate Driver Phase Mapping
// Note:    CTRL4 = DRL0/DRH3
//          CTRL5 = DRL1/DRH4
//          CTRL6 = DRL2/DRH5
//=====================================

// ADC Sequencer Readout Macro Functions
static inline int16_t readseq_aio10_raw(void)           { return PAC5XXX_ADC->AS0R0.VAL; }
static inline int16_t readseq_aio32_raw(void)           { return PAC5XXX_ADC->AS0R1.VAL; }
static inline int16_t readseq_aio54_raw(void)           { return PAC5XXX_ADC->AS0R2.VAL; }

PAC5XXX_RAMFUNC static inline int16_t readseq_iphase_u_raw(void)           { return PAC5XXX_ADC->AS0R0.VAL; }
PAC5XXX_RAMFUNC static inline int16_t readseq_iphase_v_raw(void)           { return PAC5XXX_ADC->AS0R1.VAL; }
PAC5XXX_RAMFUNC static inline int16_t readseq_iphase_w_raw(void)           { return PAC5XXX_ADC->AS0R2.VAL; }

// ADC Sequencer Readout Macro Functions
static inline int16_t readseq_vphase_u_raw(void)               { return PAC5XXX_ADC->AS0R0.VAL; }
static inline int16_t readseq_vphase_v_raw(void)               { return PAC5XXX_ADC->AS0R1.VAL; }
static inline int16_t readseq_vphase_w_raw(void)               { return PAC5XXX_ADC->AS0R2.VAL; }

// ADC Sequencer Readout Macro Functions
//static inline int16_t readseq_vbus_raw(void)                            { return PAC5XXX_ADC->AS1R0.VAL; }

void config_seq_foc(void);
void config_seq_sim(void);
void config_seq_simbrake(void);

void init_emux(void);

void config_seq_ipd(uint32_t sequencer_trigger_value);

#endif  //#ifndef BOARD_PAC5222EVK1_H
