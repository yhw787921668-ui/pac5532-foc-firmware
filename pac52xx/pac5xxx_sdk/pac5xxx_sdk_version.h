/**************************************************************************//**
 * @file     pac5xxx_sdk_version.h
 * @brief    SDK Version
 *
 * @note
 * Copyright (C) 2015-2023, Qorvo, Inc.
 *
 * @par
 * THIS SOFTWARE IS SUBJECT TO A SOURCE CODE LICENSE AGREEMENT WHICH PROVIDES,
 * AMONG OTHER THINGS:  (i) THAT IT CAN BE USED ONLY TO ADAPT THE LICENSEE'S
 * APPLICATION TO PAC PROCESSORS SUPPLIED BY QORVO, INC.;
 * (ii) THAT  IT IS PROVIDED "AS IS" WITHOUT WARRANTY;  (iii) THAT
 * QORVO, INC. IS NOT LIABLE FOR ANY INDIRECT DAMAGES OR FOR DIRECT
 * DAMAGES EXCEEDING US$1,500;  AND (iv) THAT IT CAN BE DISCLOSED TO AND USED
 * ONLY BY CERTAIN AUTHORIZED PERSONS.
 *
 ******************************************************************************/

#ifndef PAC5XXX_SDK_VERSION_H
#define PAC5XXX_SDK_VERSION_H

#define SDK_VERSION_MAJOR           2
#define SDK_VERSION_MINOR           8
#define SDK_VERSION_BUGFIX          1
#define SDK_VERSION_RELEASE_STATE   PAC52XX_RELEASED  //PAC52XX_RELEASED, PAC52XX_RC, PAC52XX_BETA, PAC52XX_ALPHA, PAC52XX_IN_DEVELOPMENT
#define SDK_VERSION_RELEASE_NUM     0
#define SDK_VERSION_STR             "v2.8.1"

#endif /* PAC5XXX_SDK_VERSION_H */

/*******************************************************************************
Change Log
===========================
PAC52XX SDK v2.8.1
===========================
Improvements
------------
- Added PAC52400 device

===========================
PAC52XX SDK v2.8.0
===========================
Improvements
------------
- Added PAC52710 and PAC52711 devices

===========================
PAC52XX SDK v2.7.1
===========================
Fixed Bugs
------------
- pac5xxx_driver_system_pll_config_mhz.c: Modified 'PLLOUTDIV' from 1 to 2 and 'PLLFBDIV' from 50 to 100 to meet PLL design specification

===========================
PAC52XX SDK v2.7.0
===========================
Improvements
------------
- Added PAC5285 device
- Modified pac_recovery() delay time to be 6 seconds
- in pac5xxx_adc_drivers.h, added pac5xxx_adc_as0_enable() and pac5xxx_adc_as1_enable()
- Updated headers to replace Active-Semi with Qorvo

===========================
PAC52XX SDK v2.6.1
===========================
Improvements
------------
- corrected readme.txt file; all pac55xx references should have been pac52xx

===========================
PAC52XX SDK v2.6.0
===========================
Improvements
------------
- pac5xxx_device folder added with device header files - include one of these before accessing AFE registers in pac5xxx_tile... headers
- CAFE_ARCH1 or CAFE_ARCH2 must now be defined to include pac5xxx_tile... headers.  Include one of the device headers to properly define

===========================
PAC52XX SDK v2.5.1
===========================
Improvements
------------
- In startup files for GCC, changed pac_delay_asm() to use __ASM instead of asm for compiler compatibility

===========================
PAC52XX SDK v2.5.0
===========================
Improvements
------------
- Added pac_recovery() that allows recovery of a misconfigured device if PE2 is connected to ground

===========================
PAC52XX SDK v2.4.5
===========================
CAFE_ARCH1 Devices: PAC5210, PAC5220, PAC5222, PAC5223, PAC5225, PAC5250, PAC5253
CAFE_ARCH2 Devices: PAC5232, PAC5256

Improvements
------------
- added PAC5256
- fixed GCC RAMFUNC definition to eliminate .data section changed warning
- removed Keil startup CRP entry at fixed link location - not present on PAC devices

===========================
PAC52XX SDK v2.4.4
===========================
CAFE_ARCH1 Devices: PAC5210, PAC5220, PAC5222, PAC5223, PAC5225, PAC5250, PAC5253
CAFE_ARCH2 Devices: PAC5232

Improvements
------------
- added PAC5222 with appropriate new registers and enums; #define PAC5222 and CAFE_ARCH1 to use PAC5222

Fixed Bugs
----------
- pac5xxx_tile_SignalManager.h: Corrected ENSIG Enum with the correct bit name SMEN instead of ENSIG



===========================
PAC52XX SDK v2.4.3
===========================
Improvements
------------
- In pac5xxx_tile_SignalManager.h, added new #defines useful for ADC sequencer config for devices using
  CAFE_ARCH2 like PAC5232.
- In startup_pac5xxx_iar.s, added a default Reset_Handler instead of calling __iar_program_start directly at reset.
     This Reset_Handler is defined with the PUBWEAK attribute so that it can be overridden by a Reset_Handler
     in another file.
- In startup_pac5xxx_keil.s, now exports __initial_sp for use in other files


===========================
PAC52XX SDK v2.4.2
===========================
Fixed Bugs
----------
- CAFE Addresses for ADDR_ENBBM and ADDR_PROTCTL registers are now device dependent
     Define the appropriate PAC52XX device to access these registers

===========================
PAC52XX SDK v2.4.1
===========================
Fixed Bugs
----------
- Addressed Keil anonymous unions issue with CAFE header files

===========================
PAC52XX SDK v2.4.0
===========================
New Features, Improvements
--------------------------
- Added support in CAFE header files for CAFE Architecture 2 - define CAFE_ARCH2 to use
    The following headers are no longer included in pac52XX.h and must be included separately:
        pac5xxx_tile_DriverManager.h
        pac5xxx_tile_SignalManager.h
        pac5xxx_tile_SystemManager.h
        pac5xxx_tile_PowerManager.h

Fixed Bugs
----------
- Fixed UART IER (Interrupt Enable Register) bit locations

===========================
PAC52XX SDK v2.3.1
===========================
Fixed Bugs
----------
- In pac5xxx_tile_PowerManager.h, fixed FSWM ENUM associated with the Power Switcher Configuration register (SOC.SCFG 0x14)

===========================
PAC52XX SDK v2.3.0
===========================
New Features, Improvements
--------------------------
- Added pac5xxx_boot_gcc folder for use with IDEs supporting GCC

===========================
PAC52XX SDK v2.2.2
===========================
New Features, Improvements
--------------------------
- SOCBridge: Added 16-bit writes/reads; now addr, R/W, and data are written with one 16-bit value
- SOCBridge: Modified the clock to run at 12.5 Mhz when HCLK is 50 MHz; previously it was running at 2.27 MHz
- pac52XX.h: PAC52XX_RELEASE_STATE enum changed to #defines

Fixed Bugs
----------
- SPI: pac5xxx_spi_slave_config2(): corrected registers for clock polarity and phase
- I2C: pac5xxx_i2c_master_write() and pac5xxx_i2c_master_write_byte()
    Corrected MCTRL writes to XFER_TYPE and AUTONO_XFER bits
- I2C: removed unecessary function declarations
    pac5xxx_i2c_int_enable_MST_A_NACK_ACK()
    pac5xxx_i2c_int_enable_MST_D_NACK_ACK()

===========================
PAC52XX SDK v2.2.1
===========================
New Features, Improvements
--------------------------
- cleaned up file headers
- Added #ifndef HEADER_FILE_H for .h files that didn't have them
- Restored default STACK_SIZE to 0x90
- Removed Start Up delay in SystemInit()
- SocBridge_IRQHandler label from CoIDE now used for IAR and Keil also
- Removed pac5xxx_sys_pll_config_100mhz()
    for proper ADC operation over temperature, the PLL should be <= 80 MHz

Fixed Bugs
----------
- pac5xxx_UART.h: fixed bug re-introduced in SDK v2.2 that affects the Receive FIFO threshold
- pac52XX.h: fixed issue with IAR #define PAC5XXX_OPTIMIZE_SPEED_ON
- restored pac5xxx_rtc_config(int ie_en, uint16_t cdv, RTCCTL_PS_Type prescale);
        to pac5xxx_rtc_config(int ie_en, uint32_t cdv, RTCCTL_PS_Type prescale);
        the cdv is a 24-bit number and requires uint32_t
- set correct value for __MPU_PRESENT = 0

===========================
PAC52XX SDK v2.2
===========================
New Features, Improvements
--------------------------
- created unified SDK for CoIDE, IAR, and KEIL
- restructured folders and created separate boot folders for each IDE
- renamed pac5xxx.h to pac52XX.h
----- Changes from CoIDE SDK v2.1-----
    - removed Software SOC Bridge functions
    - pac5xxx_driver_socbridge.h: function pac5xxx_socbridge_config()
        fixed issue where enable variable wasn't being applied to SOCBCTL.SSEN
    - pac5xxx_driver_spi.h: corrected pac5xxx_SPI_master_config() to pac5xxx_spi_master_config()
    - pac5xxx_driver_timer.h: added pac5xxx_timer_X_set_mode() functions for timers a-d
    - pac5xxx_UART.h added FD_CTL register structure
    - slight update to file headers
----- Changes from IAR SDK v2.1-----
    - pac5xxx_driver_adc.h - Removed duplicate functions:
        pac5xxx_adc_as0_seq_get_result
        pac5xxx_adc_as1_seq_get_result
    - pac5xxx_driver_config.h - commented out all RAM Func #defines so nothing in RAM by default
    - pac5xxx_driver_rtc.h -
        pac5xxx_rtc_config(int ie_en, uint32_t cdv, RTCCTL_PS_Type prescale); is now
        pac5xxx_rtc_config(int ie_en, uint16_t cdv, RTCCTL_PS_Type prescale);
    - pac5xxx_driver_rtc_config.c -
        pac5xxx_rtc_config(int ie_en, uint32_t cdv, RTCCTL_PS_Type pre_scaler) is now
        pac5xxx_rtc_config(int ie_en, uint16_t cdv, RTCCTL_PS_Type pre_scaler)
    - pac5xxx_driver_system_ccs_config.c fixed FCLK bug in pac5xxx_sys_ccs_config()
    - pac5xxx_driver_system_pll_config_100mhz.c - pac5xxx_sys_pll_config_100mhz() now
        uses 100 for PLLCTL_FDBK_DIV(100) instead of 125.
    - pac5xxx_UART.h
        added MCR struct offset 0x10
        added MSR struct offset 0x18
----- Changes from Keil SDK v1.0.0 -----
    - Added RAMFUNCs to all functions
    - pac5xxx_driver_rtc.h -
        pac5xxx_rtc_config(int ie_en, uint32_t cdv, RTCCTL_PS_Type prescale); is now
        pac5xxx_rtc_config(int ie_en, uint16_t cdv, RTCCTL_PS_Type prescale);
    - pac5xxx_driver_rtc_config.c -
        pac5xxx_rtc_config(int ie_en, uint32_t cdv, RTCCTL_PS_Type pre_scaler) is now
        pac5xxx_rtc_config(int ie_en, uint16_t cdv, RTCCTL_PS_Type pre_scaler)
    - pac5xxx_driver_socbridge.h: function pac5xxx_socbridge_config()
        fixed issue where enable variable wasn't being applied to SOCBCTL.SSEN
    - pac5xxx_driver_system_pll_config_100mhz.c - pac5xxx_sys_pll_config_100mhz() now
        uses 100 for PLLCTL_FDBK_DIV(100) instead of 125.
    - pac5xxx_driver_uart.h:
        added pac5xxx_uart_msr_X() functions for configuring Modem Status Register
        added scratchpad data read and write functions
    - pac5xxx_driver_uart_config.c: added pac5xxx_uart_config_MCR() function
    - pac5xxx_UART.h - added
        added MCR struct offset 0x10
        added MSR struct offset 0x18


*****************************************************************************************/