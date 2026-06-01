
//=============================================================================
// Copyright (C) 2020 - 2023, Qorvo, Inc.
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
 
#ifndef BOARD_PAC5526AEVK1_H
#define BOARD_PAC5526AEVK1_H

#include "board_options.h"
#include "pac5xxx.h"
#include "config_debug.h"
#include "maths.h"

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
// Debug IO1 = PF5
// Debug IO2 = NONE
// PPM Port = NONE
// Debug DAC1 = PD4
// Debug DAC2 = PD5
// Debug DAC3 = PD6
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
#define DEVICE_SELECT                   DEVICE_5526A
#define CAPABLE_VSIM                    TRUE
#define CAPABLE_IPD                     TRUE
#define CONFIG_SHUNT                    CONFIG_SHUNT_1SHUNT
#define CAPABLE_POWERMETHOD2            FALSE
#define INTERNAL_VMS                    TRUE

//=====================================
// DC Bus Voltage
//=====================================
#define MIN_VBUS                        VDC_10V
#define MAX_VBUS                        VDC_30V

#if (INTERNAL_VMS == TRUE)
//#define VMS_VALUE_EXCLUDE_BAND_LIMIT_3SHUNT  QCONST(0.60f, 16)
#define VMS_VALUE_EXCLUDE_BAND_LIMIT_SIGNLE_SHUNT  300
#endif

//=====================================
// Current Sensing Resistor Values
//=====================================
#define PCB_RSENSE_OC_OHMS              0.010f
#define PCB_RSENSE_IUVW_OHMS            0.010f

//=====================================
// Voltage Sensing Resistor Values
//=====================================
#if (INTERNAL_VMS != TRUE)
#define R1_PLUS_R2_BY_R2_RATIO_VBUS     25.68f        // Resistor ratio for VDC bus voltage sensing
#endif
#define R1_PLUS_R2_BY_R2_RATIO_SIM      24.2558f      // Resistor ratio for phase voltage sensing

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
#define SELECT_AIO_ISHUNT               CHANNEL_AIO54

//=====================================
// SIM BEMF voltage - current mode minimum in-motion detection threshold
//=====================================
#define SIM_BEMF_DET_THRESHOLD_ADCCNT_V               20                      // Threshold for minimum phase-to-CT voltage in ADC counts

//=====================================
// Inverter Section / Configuration (For MID1)
//=====================================
#define INVERTER_TYPE_SEL               INVERTER_TYPE_MOS
#define INVERTER_MOS_RDSON_OHMS_FLT     0.00145f
#define INVERTER_MOS_VDIODE_VOLTS_FLT   0.8f
//#define INVERTER_TYPE_SEL               INVERTER_TYPE_IGBT
//#define INVERTER_IGBT_VCE_VOLTS_FLT     0.2f
//#define INVERTER_IGBT_VDIODE_VOLTS_FLT  0.6f

//=====================================
// Set Application Specific Power Driver (ASPD) sink and source current for both low and high side drivers
// Driver source current setting: 0 = 250mA
// Driver source current setting: 1 = 350mA
// Driver source current setting: 2 = 450mA
// Driver source current setting: 3 = 550mA
// Driver source current setting: 4 = 650mA
// Driver source current setting: 5 = 750mA
// Driver source current setting: 6 = 850mA
// Driver source current setting: 7 = 1000mA
//=====================================
#define milliamps_250                   0
#define milliamps_350                   1
#define milliamps_450                   2
#define milliamps_550                   3
#define milliamps_650                   4
#define milliamps_750                   5
#define milliamps_850                   6
#define milliamps_1000                  7

#define ASPD_HSSOURCE                   milliamps_1000
#define ASPD_HSSINK                     ASPD_HSSOURCE
#define ASPD_LSSOURCE                   ASPD_HSSOURCE
#define ASPD_LSSINK                     ASPD_HSSOURCE

//==========================================================================
//
// ADC Offset Configuration
//
//==========================================================================

//=====================================
// ADC Offset Channel Selection
//=====================================
// Select all channels where offset must be subtracted in the firmware calculations. Offset can either be calibrated automatically or manually set.
#define CONFIG_OFFSET                   (CHANNEL_AIO54)
#define ENABLE_ADCCAL_AIO54

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
#define HALLA_PORTNUM                   P4

#define PAC5XXX_GPIO_HALLB              PAC55XX_GPIOD
#define HALLB_PORTNUM                   P5

#define PAC5XXX_GPIO_HALLC              PAC55XX_GPIOD
#define HALLC_PORTNUM                   P6

#define HALLA_PORT_BIT                  BIT_04
#define HALLB_PORT_BIT                  BIT_05
#define HALLC_PORT_BIT                  BIT_06
#define PAC5XXX_GPIO_HALL               PAC5XXX_GPIO_HALLA
#define Hall_Gpio_IRQn                  GpioD_IRQn

#define HALL_PUEN                       PDPUEN
#define HALL_MUXSEL                     PDMUXSEL

//=====================================
// QEP Input Port Definitions
//=====================================
#define PAC5XXX_GPIO_QEPPHA             PAC55XX_GPIOD
#define QEPPHB_PORTNUM                  P6

#define PAC5XXX_GPIO_QEPPHB             PAC55XX_GPIOD
#define QEPPHA_PORTNUM                  P5

#define PAC5XXX_GPIO_QEPIDX             PAC55XX_GPIOD
#define QEPIDX_PORTNUM                  P4

//=====================================
// Debug IO Port Definitions
//=====================================
#define DEBUGIO1_PORT                   PORT_PF5

//=====================================
// PPM Input Port (Optional)
//=====================================

//=====================================
// PWM DAC Port Definitions
//=====================================
#define PWMDAC1_PORT                    PORT_PD4
#define PWMDAC2_PORT                    PORT_PD5
#define PWMDAC3_PORT                    PORT_PD6

//=====================================
// Push Button Input Port Definitions
// Active Low, PullUp Internally
//=====================================
#define PAC5XXX_GPIO_PUSH_BUTTON        PAC55XX_GPIOE   // Port E
#define PUSH_BUTTON_PORTNUM             P0              // Port P0
#define PUSH_BUTTON_PULLUP_PORT         PEPUEN          // Port E Pull Up Enable
#define PUSH_BUTTON_PORT_MUX            PEMUXSEL        // PE MUX Selected
#define PUSH_BUTTON_SCHMITT_TRIG_PORT   PEDS            // PEDS (PE Drive Strength/Schmitt Trigger, 400D 0474h)
#define PUSH_BUTTON_SCHMITT_ENABLE      P0ST            // Pin 0 Schmitt Trigger enabled

//==========================================
// PWM DAC Driver Control Macro Functions
//==========================================
static inline void dac_psel_pwm(void)
{
#if defined (PWMDAC1_PORT)
    PAC55XX_SCC->PDMUXSEL.P4 = 1;
#endif

#if defined (PWMDAC2_PORT)
    PAC55XX_SCC->PDMUXSEL.P5 = 1;
#endif

#if defined (PWMDAC3_PORT)
    PAC55XX_SCC->PDMUXSEL.P6 = 1;
#endif
}

static inline void dac_psel_gpio(void)
{
#if defined (PWMDAC1_PORT)
    PAC55XX_SCC->PDMUXSEL.P4 = 0;
#endif

#if defined (PWMDAC2_PORT)
    PAC55XX_SCC->PDMUXSEL.P5 = 0;
#endif

#if defined (PWMDAC3_PORT)
    PAC55XX_SCC->PDMUXSEL.P6 = 0;
#endif
}

static inline void dac_out_clear(void)
{
#if defined (PWMDAC1_PORT)
    PAC55XX_GPIOD->OUT.P4 = 0;
#endif

#if defined (PWMDAC2_PORT)
    PAC55XX_GPIOD->OUT.P5 = 0;
#endif

#if defined (PWMDAC3_PORT)
    PAC55XX_GPIOD->OUT.P6 = 0;
#endif
}

static inline void dac_modesel_pp(void)
{
#if defined (PWMDAC1_PORT)
    PAC55XX_GPIOD->MODE.P4 = 1;
#endif

#if defined (PWMDAC2_PORT)
    PAC55XX_GPIOD->MODE.P5 = 1;
#endif

#if defined (PWMDAC3_PORT)
    PAC55XX_GPIOD->MODE.P6 = 1;
#endif
}

static inline void dac_outen_enable(void)
{
    dac_modesel_pp();
#if defined (PWMDAC1_PORT)
    PAC55XX_GPIOD->OUTMASK.P4 = 0;
#endif

#if defined (PWMDAC2_PORT)
    PAC55XX_GPIOD->OUTMASK.P5 = 0;
#endif

#if defined (PWMDAC3_PORT)
    PAC55XX_GPIOD->OUTMASK.P6 = 0;
#endif
}

PAC5XXX_RAMFUNC static inline void pwmdac_1_set_duty(uint16_t duty)         { PAC55XX_TIMERB->CCTR4.CTR = duty; }
PAC5XXX_RAMFUNC static inline void pwmdac_2_set_duty(uint16_t duty)         { PAC55XX_TIMERB->CCTR5.CTR = duty; }
PAC5XXX_RAMFUNC static inline void pwmdac_3_set_duty(uint16_t duty)         { PAC55XX_TIMERB->CCTR6.CTR = duty; }

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
        static inline void debugio1_config(void)    { PAC55XX_GPIOF->MODE.P5 = 1; PAC55XX_GPIOF->OUTMASK.P5 = 0; }
        static inline void debugio1_on(void)        { PAC55XX_GPIOF->OUT.P5 = 1; }
        static inline void debugio1_off(void)       { PAC55XX_GPIOF->OUT.P5 = 0; }
        static inline void debugio1_toggle(void)    { PAC55XX_GPIOF->OUT.P5 ^= 1; }
    #else
        static inline void debugio1_config(void)    { }
        static inline void debugio1_on(void)        { }
        static inline void debugio1_off(void)       { }
        static inline void debugio1_toggle(void)    { }
    #endif

    static inline void debugio2_config(void)        { }
    static inline void debugio2_on(void)            { }
    static inline void debugio2_off(void)           { }
    static inline void debugio2_toggle(void)        { }

    static inline void config_debugio(void)         { debugio1_config(); }
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
static inline int16_t readseq_aio54_raw(void)                   { return PAC55XX_ADC->DTSERES1.VAL; }

static inline int16_t readseq_iphase_u_raw(void)                { return PAC55XX_ADC->DTSERES1.VAL; }
static inline int16_t readseq_iphase_v_raw(void)                { return PAC55XX_ADC->DTSERES1.VAL; }
static inline int16_t readseq_iphase_w_raw(void)                { return PAC55XX_ADC->DTSERES1.VAL; }

static inline int16_t readseq_vphase_u_raw(void)                { return PAC55XX_ADC->DTSERES2.VAL; }
static inline int16_t readseq_vphase_v_raw(void)                { return PAC55XX_ADC->DTSERES4.VAL; }
static inline int16_t readseq_vphase_w_raw(void)                { return PAC55XX_ADC->DTSERES6.VAL; }

//static inline int16_t readseq_vbus_raw(void)                    { return PAC55XX_ADC->DTSERES6.VAL; }
//static inline int16_t readseq_vbus_vms_raw(void)                { return PAC55XX_ADC->DTSERES9.VAL; }

// ADC Sequencer Readout Macro Functions
PAC5XXX_RAMFUNC static inline int16_t readseq_ssi1_raw(void)                { return PAC55XX_ADC->DTSERES1.VAL; }
PAC5XXX_RAMFUNC static inline int16_t readseq_ssi2_raw(void)                { return PAC55XX_ADC->DTSERES4.VAL; }

void config_seq_foc(void);
void config_seq_sim(void);
void config_seq_simbrake(void);

void init_emux(void);

void config_seq_ipd(uint32_t sequencer_trigger_value);

#endif  //#ifndef BOARD_PAC5526AEVK1_H
