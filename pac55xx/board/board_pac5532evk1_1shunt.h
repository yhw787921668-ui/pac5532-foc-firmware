
//=============================================================================
// Copyright (C) 2018 - 2023, Qorvo, Inc.
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
 
#ifndef BOARD_PAC5532EVK1_1SHUNT_H
#define BOARD_PAC5532EVK1_1SHUNT_H

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
// Vbus = PF6

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
// Vbus                  : N0
// Motor Phase Current U : N2
// Motor Phase Current V : N4
// Motor Phase Current W : N6
// Motor Phase Voltage U : N2
// Motor Phase Voltage V : N4
// Motor Phase Voltage W : N6

// Special Function - Port Mapping
// Debug IO1 = PE0
// Debug IO2 = PE1
// PPM Port = NONE
// Debug DAC1 = PD0
// Debug DAC2 = PD1
// Debug DAC3 = PD2
// Debug DAS = PF4

// MODULE MISC CAFE REGISTER
#define HIB_BIT_DEFAULT             0                                                                   //0 = Normal; 1 = Hibernate
#define PBEN_BIT_DEFAULT            0                                                                   //0 = Push Button Disabled; 1 = Push Button Enabled (AIO6)
#define VREFSET_BIT_DEFAULT         0                                                                   //0 = ADC VREF is 2.5V; 1 = ADC VREF is 3.0V
#define CLKOUTEN_BIT_DEFAULT        0                                                                   //0 = Disabled; 1 = Enabled
#define MCUALIVE_BIT_DEFAULT        1                                                                   //0 = Disabled; 1 = Enabled
#define TPBD_BIT_DEFAULT            0                                                                   //0 = Disabled; 1 = Enabled
#define RFU_BIT_DEFAULT             0                                                                   //0 = Disabled; 1 = Enabled
#define ENSIG_BIT_DEFAULT           1                                                                   //0 = Disabled; 1 = Enabled
#define MODULE_MISC_BITS_DEFAULT    ((HIB_BIT_DEFAULT << 7) + (PBEN_BIT_DEFAULT << 6) + (VREFSET_BIT_DEFAULT << 5) + (CLKOUTEN_BIT_DEFAULT << 4) + (MCUALIVE_BIT_DEFAULT << 3) + (TPBD_BIT_DEFAULT << 2) + (RFU_BIT_DEFAULT << 1) + (ENSIG_BIT_DEFAULT << 0))

//==========================================================================
//
// Board Properties
//
//==========================================================================

//=====================================
// Board Configuration
//=====================================
#define DEVICE_SELECT                   DEVICE_5532
#define CAPABLE_VSIM                    TRUE
#define CAPABLE_IPD                     TRUE
#define CONFIG_SHUNT                    CONFIG_SHUNT_1SHUNT
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
#define R1_PLUS_R2_BY_R2_RATIO_VBUS     57.00f      // Resistor ratio for VDC bus voltage sensing
#define R1_PLUS_R2_BY_R2_RATIO_SIM      57.00f      // Resistor ratio for phase voltage sensing

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
// AIO Port Mapping
//=====================================
#define SELECT_AIO_ISHUNT               CHANNEL_AIO32

//=====================================
// SIM BEMF voltage - current mode minimum in-motion detection threshold
//=====================================
#define SIM_BEMF_DET_THRESHOLD_ADCCNT_V               20                      // Threshold for minimum phase-to-CT voltage in ADC counts

//==========================================================================
//
// ADC Offset Configuration
//
//==========================================================================

//=====================================
// ADC Offset Channel Selection
//=====================================
// Select all channels where offset must be subtracted in the firmware calculations. Offset can either be calibrated automatically or manually set.
#define CONFIG_OFFSET                   (CHANNEL_AIO32)
#define ENABLE_ADCCAL_AIO32

//=====================================
// ADC Offset Values
//=====================================
#define ADC_OFFSET_DEFAULT_DIFFAMP      512             // Midrail offset
#define ADC_OFFSET_MIN_DIFFAMP          495             // Minimum value of current offset
#define ADC_OFFSET_MAX_DIFFAMP          529             // Maximum value of current offset

//==========================================================================
//
// I/O Port Mapping
//
//==========================================================================
//=====================================
// HALL Input Port Definitions
//=====================================
#define PAC5XXX_GPIO_HALLA              PAC55XX_GPIOD
#define HALLA_PORTNUM                   P0

#define PAC5XXX_GPIO_HALLB              PAC55XX_GPIOD
#define HALLB_PORTNUM                   P1

#define PAC5XXX_GPIO_HALLC              PAC55XX_GPIOD
#define HALLC_PORTNUM                   P2

#define HALLA_PORT_BIT                  BIT_00
#define HALLB_PORT_BIT                  BIT_01
#define HALLC_PORT_BIT                  BIT_02
#define PAC5XXX_GPIO_HALL               PAC5XXX_GPIO_HALLA
#define Hall_Gpio_IRQn                  GpioD_IRQn

#define HALL_PUEN                       PDPUEN
#define HALL_MUXSEL                     PDMUXSEL

//=====================================
// QEP Input Port Definitions
//=====================================
#define PAC5XXX_GPIO_QEPPHA             PAC55XX_GPIOD
#define QEPPHB_PORTNUM                  P2

#define PAC5XXX_GPIO_QEPPHB             PAC55XX_GPIOD
#define QEPPHA_PORTNUM                  P1

#define PAC5XXX_GPIO_QEPIDX             PAC55XX_GPIOD
#define QEPIDX_PORTNUM                  P0

//=====================================
// Debug IO Port Definitions
//=====================================
#define DEBUGIO1_PORT                   PORT_PE0
#define DEBUGIO2_PORT                   PORT_PE1

//=====================================
// PPM Input Port (Optional)
//=====================================

//=====================================
// PWM DAC Port Definitions
//=====================================
#define PWMDAC1_PORT                    PORT_PD0
#define PWMDAC2_PORT                    PORT_PD1
#define PWMDAC3_PORT                    PORT_PD2

//=====================================
// Push Button Input Port Definitions
// Active Low, PullUp Internally
//=====================================
#define PAC5XXX_GPIO_PUSH_BUTTON        PAC55XX_GPIOF   // Port F
#define PUSH_BUTTON_PORTNUM             P2              // Port P2
#define PUSH_BUTTON_PULLUP_PORT         PFPUEN          // Port F Pull Up Enable
#define PUSH_BUTTON_PORT_MUX            PFMUXSEL        // PF MUX Selected
#define PUSH_BUTTON_SCHMITT_TRIG_PORT   PFDS            // PFDS (PF Drive Strength/Schmitt Trigger, 400D 0474h)
#define PUSH_BUTTON_SCHMITT_ENABLE      P2ST            // Pin 2 Schmitt Trigger enabled

//==========================================
// PWM DAC Driver Control Macro Functions
//==========================================
static inline void dac_psel_pwm(void)
{
#if defined (PWMDAC1_PORT)
    PAC55XX_SCC->PDMUXSEL.P0 = 1;
#endif

#if defined (PWMDAC2_PORT)
    PAC55XX_SCC->PDMUXSEL.P1 = 1;
#endif

#if defined (PWMDAC3_PORT)
    PAC55XX_SCC->PDMUXSEL.P2 = 1;
#endif
}

static inline void dac_psel_gpio(void)
{
#if defined (PWMDAC1_PORT)
    PAC55XX_SCC->PDMUXSEL.P0 = 0;
#endif

#if defined (PWMDAC2_PORT)
    PAC55XX_SCC->PDMUXSEL.P1 = 0;
#endif

#if defined (PWMDAC3_PORT)
    PAC55XX_SCC->PDMUXSEL.P2 = 0;
#endif
}

static inline void dac_out_clear(void)
{
#if defined (PWMDAC1_PORT)
    PAC55XX_GPIOD->OUT.P0 = 0;
#endif

#if defined (PWMDAC2_PORT)
    PAC55XX_GPIOD->OUT.P1 = 0;
#endif

#if defined (PWMDAC3_PORT)
    PAC55XX_GPIOD->OUT.P2 = 0;
#endif
}

static inline void dac_modesel_pp(void)
{
#if defined (PWMDAC1_PORT)
    PAC55XX_GPIOD->MODE.P0 = 1;
#endif

#if defined (PWMDAC2_PORT)
    PAC55XX_GPIOD->MODE.P1 = 1;
#endif

#if defined (PWMDAC3_PORT)
    PAC55XX_GPIOD->MODE.P2 = 1;
#endif
}

static inline void dac_outen_enable(void)
{
    dac_modesel_pp();
#if defined (PWMDAC1_PORT)
    PAC55XX_GPIOD->OUTMASK.P0 = 0;
#endif

#if defined (PWMDAC2_PORT)
    PAC55XX_GPIOD->OUTMASK.P1 = 0;
#endif

#if defined (PWMDAC3_PORT)
    PAC55XX_GPIOD->OUTMASK.P2 = 0;
#endif
}

PAC5XXX_RAMFUNC static inline void pwmdac_1_set_duty(uint16_t duty)         { PAC55XX_TIMERB->CCTR0.CTR = duty; }
PAC5XXX_RAMFUNC static inline void pwmdac_2_set_duty(uint16_t duty)         { PAC55XX_TIMERB->CCTR1.CTR = duty; }
PAC5XXX_RAMFUNC static inline void pwmdac_3_set_duty(uint16_t duty)         { PAC55XX_TIMERB->CCTR2.CTR = duty; }

#ifdef ENABLE_DEBUG_DAS
    PAC5XXX_RAMFUNC static inline void config_debugDASio(void)  { PAC55XX_GPIOF->MODE.P4 = 1; PAC55XX_GPIOF->OUTMASK.P4 = 0; }
    PAC5XXX_RAMFUNC static inline void debugDASio_on(void)      { PAC55XX_GPIOF->DOSET.w = (1 << 4); }
    PAC5XXX_RAMFUNC static inline void debugDASio_off(void)     { PAC55XX_GPIOF->DOCLEAR.w = (1 << 4); }
    PAC5XXX_RAMFUNC static inline void debugDASio_toggle(void)  { PAC55XX_GPIOF->OUT.P4 ^= 1; }
#else
    PAC5XXX_RAMFUNC static inline void config_debugDASio(void)  { }
    PAC5XXX_RAMFUNC static inline void debugDASio_on(void)      { }
    PAC5XXX_RAMFUNC static inline void debugDASio_off(void)     { }
    PAC5XXX_RAMFUNC static inline void debugDASio_toggle(void)  { }
#endif

#ifdef ENABLE_DEBUG_IO
    #ifdef DEBUGIO1_PORT
        static inline void debugio1_config(void)    { PAC55XX_GPIOE->MODE.P0 = 1; PAC55XX_GPIOE->OUTMASK.P0 = 0; }
        static inline void debugio1_on(void)        { PAC55XX_GPIOE->OUT.P0 = 1; }
        static inline void debugio1_off(void)       { PAC55XX_GPIOE->OUT.P0 = 0; }
        static inline void debugio1_toggle(void)    { PAC55XX_GPIOE->OUT.P0 ^= 1; }
    #else
        static inline void debugio1_config(void)    { }
        static inline void debugio1_on(void)        { }
        static inline void debugio1_off(void)       { }
        static inline void debugio1_toggle(void)    { }
    #endif

    #ifdef DEBUGIO2_PORT
        static inline void debugio2_config(void)    { PAC55XX_GPIOE->MODE.P1 = 1; PAC55XX_GPIOE->OUTMASK.P1 = 0; }
        static inline void debugio2_on(void)        { PAC55XX_GPIOE->OUT.P1 = 1; }
        static inline void debugio2_off(void)       { PAC55XX_GPIOE->OUT.P1 = 0; }
        static inline void debugio2_toggle(void)    { PAC55XX_GPIOE->OUT.P1 ^= 1; }
    #else
        static inline void debugio1_config(void)    { }
        static inline void debugio1_on(void)        { }
        static inline void debugio1_off(void)       { }
        static inline void debugio1_toggle(void)    { }
    #endif

    static inline void config_debugio(void)         { debugio1_config(); debugio2_config(); }
#else
    static inline void config_debugio(void)         { }
    static inline void debugio1_on(void)            { }
    static inline void debugio1_off(void)           { }
    static inline void debugio1_toggle(void)        { }
    static inline void debugio2_on(void)            { }
    static inline void debugio2_off(void)           { }
    static inline void debugio2_toggle(void)        { }
#endif

// ADC Sequencer Readout Macro Functions
static inline int16_t readseq_aio32_raw(void)               { return PAC55XX_ADC->DTSERES1.VAL; }

static inline int16_t readseq_iphase_u_raw(void)            { return PAC55XX_ADC->DTSERES1.VAL; }
static inline int16_t readseq_iphase_v_raw(void)            { return PAC55XX_ADC->DTSERES1.VAL; }
static inline int16_t readseq_iphase_w_raw(void)            { return PAC55XX_ADC->DTSERES1.VAL; }

static inline int16_t readseq_vphase_u_raw(void)            { return PAC55XX_ADC->DTSERES2.VAL; }
static inline int16_t readseq_vphase_v_raw(void)            { return PAC55XX_ADC->DTSERES4.VAL; }
static inline int16_t readseq_vphase_w_raw(void)            { return PAC55XX_ADC->DTSERES6.VAL; }

//static inline int16_t readseq_vbus_raw(void)                { return PAC55XX_ADC->DTSERES6.VAL; }

// ADC Sequencer Readout Macro Functions
PAC5XXX_RAMFUNC static inline int16_t readseq_ssi1_raw(void)                { return PAC55XX_ADC->DTSERES1.VAL; }
PAC5XXX_RAMFUNC static inline int16_t readseq_ssi2_raw(void)                { return PAC55XX_ADC->DTSERES4.VAL; }

void config_seq_foc(void);
void config_seq_sim(void);
void config_seq_simbrake(void);

void init_emux(void);

void config_seq_ipd(uint32_t sequencer_trigger_value);

#endif  //#ifndef BOARD_PAC5532EVK1_1SHUNT_H
