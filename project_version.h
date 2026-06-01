
/**
 * @file project_version.h
 * @brief Use for FOC project version
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

#ifndef PROJECT_VERSION_H
#define PROJECT_VERSION_H
#include "pac5xxx.h"

// version release state
#define PROJECT_VERSION_MAJOR           4
#define PROJECT_VERSION_MINOR           4
#define PROJECT_VERSION_BUGFIX          2
#define PROJECT_VERSION_RELEASE_STATE   PAC5XXX_RELEASED        // PAC5XXX_RELEASED, PAC5XXX_RC, PAC5XXX_BETA, PAC5XXX_ALPHA, PAC5XXX_IN_DEVELOPMENT
#define PROJECT_VERSION_RELEASE_NUM     0
#define PROJECT_VERSION_STR             "v4.4.2"                // Use version or date Examples: v4.2.3, v4.3.0-beta1 or date 2019-09-10

// Include Library Build Information - Includes Build Date and Time
#if defined(__ECLIPSE__) && defined(__GNUC__)  // Eclipse and GNU C Compiler
#include "project_build_info_eclipse.h"
#elif defined(__COIDE__) && defined __GNUC__   // CoIDE and GNU C Compiler
#include "project_build_info_coide.h"
#elif defined __ICCARM__            // IAR C Compiler
#include "project_build_info_iar.h"
#elif defined __CC_ARM              // Keil RealView C Compiler
#include "project_build_info_keil.h"
#elif defined(__clang__) && defined(__GNUC__)    // Keil v6 Clang Compiler
#include "project_build_info_keil.h"
#endif

#endif

/*====== Release Change History ============================================================================================
==================================
PAC FOC v4.4.2
==================================

----- Enhancements -----
  - Supported New devices: PAC52710, PAC52711, PAC52400, PAC5524A, PAC5526A and PAC5527A. 
  - Added HALL fault tolerant opearation with intermittent HALL signals.
  - Supported OC protection with HPDAC.
  - Added the speed ref based stall detection.
  - Imported PAC52xx SDK v2.8.1.
  
  ----- Bug Fixes -----
  - Fixed multiple typos in the project.
  - Updated the format of files.

==================================
PAC FOC v4.4.1
==================================

----- Enhancements -----
  - Supported Eclipse v4.27. 
  - Supported Keil v5.38 Compiler v5 and v6.
  - Supported IAR v7 (v8) and v9.
  - Imported PAC52xx SDK v2.7.1.
  - Added a new mpos mode (Experimental) - Aligngo & Trap & Estimator.

  ----- Bug Fixes -----
  - Fixed the max measurable inductance limitation from using Auto-tuning.
  - Fixed multiple typos in the project.

==================================
PAC FOC v4.4.0
==================================
Additional Device Support : PAC5523A, PAC5532B, PAC5556A EVKs.

----- New Features -----
  - Added PLL for speed and angle calculation.
  - Added flash write/read feature : Added flash write/read functionality for both PAC52XX and PAC55XX.
  - Added CAN protocol support : Added CAN communication protocol support.
  - Added QEP signal calibration : Correct the non-linear issue caused by QEP hardware, particularly for contactless QEP.
  - Added Forced angle feature : Motor can be operated by the given angle. The rotor would be rotated to the given position.
  - Added Debug Tool 'Jscope' : Supported Jscope to display variables in real time.
  - Added Debug Tool 'Data capture' : Captured the data from device to save as .csv file.
  - Added Buzz support : Added Buzz feature (i.e., drones application) before starting motors.

----- Enhancements -----
  - Hall FOC Improvements : Improved the HALL tuning experience. 
    The detail please refer to Guide_MPOS_HallFOC.pdf: “\resources\guides\mpos (motor position)”.
  - Autotuning Improvements : Added a feature to identify each phase resistance and inductance.
  - IPD Improvement : Improved IPD performance with less salient PMSM.
  - PI Controller for Field weakening : Implemented PI controller for field weakening control.
  - Bus voltage compensation : Compensated the voltage ripple on the DC bus.
  - Device ID check : Checked the Device ID during power up.
  - HPDAC based OC protection : Implemented OC protection with HPDAC for PAC5523/5527 (Experimental).
  - Support 12-Bit ADC : Introduced the support for 12 Bits ADC for PAC55XX only

==================================
PAC FOC v4.3.1
==================================
----- Bug Fixes -----
  -Fixed QEP functionality for PAC5556.
  -Fixed issue with OC reporting.
  -Fixed issue with PDF documentation format.
  -Modified Autotune to independently support MID and ATPI.
  -Fixed erratic behavior due to AFE MUX configuration issue during initialization for PAC 55xx.

==================================
PAC FOC v4.3.0
==================================
Additional Device Support : PAC5285, PAC5526 EVKs.

----- Enhancements -----
  - Hall FOC Improvements : Added hall edge polarity interval compensation for better angle interval matching
    and tuning method. Added tuning method for hall interval error correction. Added Hall Assist which combines
    the sensorless estimator and the hall sensor information to provide improved angle tracking and smoother hall angle
    transitions.
  - Autotuning Improvements : Added current regulation during motor identification (MID1) to improve success
    rate of autotuning without user intervention.
  - Added DQ Decoupling : Improves performance and dynamic response of current controllers during fast
    speed or motor current changes.
  - Added Harmonic Injection (Experimental) : Reduces phase current distortion when motors that are have trapezoidal
    characteristics are being driven with sinusoidal voltages.
  - Added MTPA Control (Experimental) : Maximizes torque and speed for salient motors by making use of reluctance
    torque.


==================================
PAC FOC v4.2.0
==================================
----- Enhancements -----
Additional Device Support : Added support for PAC5527, PAC5556, PAC5524 EVKs.
 -Single Shunt Improvements : Added single shunt option to use asymmetric PWM for reduced phase current distortion
 -Estimator Improvements : Improved motor position estimator for better angle accuracy and simpler tuning process
 -SIM (Start-In-Motion) Improvements : Improved performance when starting in motion. Added SIM capability based on hall sensors.
 -OPD (Open-Phase Detection) : Improvements Added OPD option that provides faster detection.
 -Hall Sensor Engine Improvements : Improved handling of dynamic startup situations.
 -Field-Weakening Improvements : Improved transitions and settling response.
 -Added support for Doxygen documentation platform.

==================================
PAC FOC v4.2.0-beta6
==================================
----- Enhancements -----
Added support for Doxygen documentation platform

==================================
PAC FOC v4.2.0-beta2
==================================
----- Enhancements -----
Added PAC5527 Support	
    Added support for PAC5527 including board files for the PAC5527EVK1 configured for 3 shunt operation.
Auto-tune (PID, Motor ID) improvements 
SS improvements for vacuum (added asymmetric PWM, reduces distortion of phase current)
Estimator improvements (improve estimate angle accuracy and simplify tuning process)
SIM improvements related to SIM speed, align time etc

==================================
PAC FOC v4.1.2
==================================
----- Enhancements -----
Added PAC5524 Support	
    Added support for PAC5524 including board files for the PAC5524EVK1 configured for 3 shunt or single shunt operation.
    
==================================
PAC FOC v4.1.1
==================================
----- Enhancements -----
Added PAC5556 Support	
    Added support for PAC5556 including board files for the PAC5556EVK1 configured for 3 shunt or single shunt operation.
    
----- Bugs Fixed -----
Fixed bug associated with GUI export of Id/Iq Filter parameters = 0
    GUI incorrectly exported Id/Iq filter parameters with values of 0.  Although the Id/Iq filters are no longer used 
    in the firmware, when the parameters were used in the parameter file specified by PARAM_SELECT, then some calculations 
    caused divide by 0 and produced errors during compile time.
Fixed bug in which GUI doesn’t write estimator parameters	
    When “Write All Parameters” is clicked, the GUI writes the parameters over UART to the PAC.  But, the estimator 
    parameters in the Tuning->LBG1 window were not written correctly.   To write the parameters, the user had to click 
    the up and down arrows for each parameter to get the GUI to write each one. The affected parameters were: KpI, 
    FcSpeed, Angle Correction Offset, Angle Correction Factor.

==================================
PAC FOC v4.1.0
==================================
For using IAR v7 with PAC52xx, the IAR configuration must be updated with a new update as of 2019-04-30.  
    See .resources\ide_support\iar for instructions.

===== New Features =====
Hall Sensor FOC Support - Motor Position (MPOS) option using hall sensors for determining motor position. The algorithm
uses angle interpolation to minimize phase current distortion.  Hall sensor support is available in 2 modes: Hall Sensor
Only mode and Hall + Estimator.

QEP Sensor FOC Support - Motor Position (MPOS) option using QEP sensor for determining motor position. The algorithm
uses Align&Go as a startup method. Supported only for PAC 55xx Family.

Motor ID and PI Auto-Tuning - Allows the user to extract motor parameters such as Ld, Lq, R, Kt and load inertia.
These motor parameters can in turn be used to automatically calculate the best tuning parameters for the current and
speed PI controllers.

===== Other Enhancements =====
PAC55XX SDK
- Updated various files with new defines and functions for clearer DTSE/ADC understanding
- device folder added with device headers - include one of these before accessing AFE registers
- pac5xxx.h - corrected __NVIC_PRIO_BITS definition to be 3 bits, which gives 8 interrupt priority levels
- pac5xxx_adc.h - added back ADCCTL_ADMUX_Type enum and noted that is deprecated and should be replaced by ADC_CHAN_TYPE
- pac5xxx_gpio.h - replaced DEBOUNCE with new name CLKSYNC; These bits enable input clock synchronization, it is not a debounce function
- using __ASM for inline assembly in pac_delay_asm() to be compatible across gcc compilers
- updated pac5xxx_driver_timer.c to eliminate compiler warnings
- updated pac5xxx_timers.h with QEP interrupt flags as __IO
- Added pac_recovery() that allows recovery of a misconfigured device if PE3 is connected to ground

PAC52XX SDK
- pac5xxx_device folder added with device header files - include one of these before accessing AFE registers in pac5xxx_tile... headers
- CAFE_ARCH1 or CAFE_ARCH2 must now be defined to include pac5xxx_tile... headers.  Include one of the device headers to properly define
- In startup files for GCC, changed pac_delay_asm() to use __ASM instead of asm for compiler compatibility
- Added pac_recovery() that allows recovery of a misconfigured device if PE2 is connected to ground

===== Unsupported Items =====
WDT - Watch Dog Timer is currently not supported
SIM in QEP mode - SIM is not supported in QEP mode. To be added in future release.
SIM-i in 1-Shunt mode - Current-based SIM is not supported in 1-Shunt mode. To be added in future release.
QEP in 52xx Family - 52xx family does not have QEP peripheral.

===== Known Issues =====
PAC5532 Single Shunt - PAC5532 Single Shunt current measurements are not correct and therefore can’t properly operate.
CBC - Cycle by Cycle current limiting is not working properly

==================================
PAC FOC v4.0.2
==================================
===== Enhancements/Bug Fixes =====
Removed unused param PUSER_VALUE_SIM_THRESHOLD that was not exported by GUI parameters
----- pac52xx -----
- Added support for PAC5256 device
- Added pac52xx SDK v2.4.5
    - fixed GCC RAMFUNC definition to eliminate .data section changed warning
    - removed Keil startup CRP entry at fixed link location - not present on PAC devices

----- pac55xx -----
- Fixed UART bug where Rx was being overwritten - enabled FIFO
- Fixed bug where SysTick was running at the wrong frequency
- Keil projects: supressed warning about single precision conversion to double
- IAR projects: a single IAR Project now supports both v7 and v8 (once opened in v8, can't go back to v7)
- Added pac55xx SDK v1.0.1
    - Fixed GCC RAMFUNC definition to eliminate .data section changed warning
    - Corrected data structure entries for ADC DTSE_SEQ_CFG_TYPEDEF and modified pac5xxx_dtse_seq_config_table() function
    - Corrected Timer TXCCTL_CCLATCH_CAPTURE_TYPE enum
    - Updated INFO Memory Definitions: INFO1, INFO2, INFO3
    - Added CCSCTL->LDOEN bit, this bit defaults to 1 at reset and should be 1 when accessing FLASH
    - Added UART FIFO Enable function; changed static functions to static inline
    - removed Keil startup CRP entry at fixed link location - not present on PAC devices
    
================================
PAC FOC v4.0.1
================================
----- New Features -----
- Added support for PAC5222 device

----- Bugs Fixed -----
- FLASH CRC: file with special FLASH CRC reset handler was not being compiled in for some IDEs
- OVERMOD: defining ENABLE_OVERMOD to turn on Over Modulation caused IAR and GCC linker to fail 

----- Supported Tool Chains -----
The following tool chains are known to work:
Keil MDK (uVision)          v5.17.0 and v5.25.0
IAR Workbench               v7.50.2 and v8.20.1  
Eclipse with PAC support    v1.1.0 (Eclipse Neon with GCC arm-7.0-2017-q4-major)
CooCox CoIDE                v1.7.8 (with GCC arm-4.8-2013q4  or arm-7.0-2017-q4-major)

----- Unsupported Features -----
- Watch Dog Timer feature is currently not supported

----- Known Issues -----
For PAC5532 Single Shunt, phase currents are not measured correctly and therefore can't operate properly

================================
PAC FOC v4.0.0
================================
----- Supported Tool Chains -----
The following tool chains are known to work:
Keil MDK (uVision)          v5.17.0 and v5.25.0
IAR Workbench               v7.50.2 and v8.20.1  
Eclipse with PAC support    v1.0.3 (Eclipse Neon with GCC arm-7.0-2017-q4-major)
CooCox CoIDE                v1.7.8 (with GCC arm-4.8-2013q4 arm-7.0-2017-q4-major)

----- Unsupported Features -----
- Watch Dog Timer feature is currently not supported

----- Known Issues -----
For PAC5532 Single Shunt, phase currents are not measured correctly and therefore can't operate properly

===========================================================================================================================*/

