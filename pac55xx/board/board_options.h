
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

#ifndef BOARD_OPTIONS_H
#define BOARD_OPTIONS_H

//=====================================
// Devices (DEVICE_SELECT)
//=====================================
#define DEVICE_55XX             (0x55 << 8) 
#define DEVICE_VERSION_NONE     (0x00 << 16)
#define DEVICE_VERSION_A        (0x0A << 16)
#define DEVICE_VERSION_B        (0x0B << 16)
#define DEVICE_5523             DEVICE_VERSION_NONE + DEVICE_55XX + (0x23)
#define DEVICE_5524             DEVICE_VERSION_NONE + DEVICE_55XX + (0x24)
#define DEVICE_5527             DEVICE_VERSION_NONE + DEVICE_55XX + (0x27)
#define DEVICE_5532             DEVICE_VERSION_NONE + DEVICE_55XX + (0x32)
#define DEVICE_5556             DEVICE_VERSION_NONE + DEVICE_55XX + (0x56)
#define DEVICE_5526             DEVICE_VERSION_NONE + DEVICE_55XX + (0x26)
#define DEVICE_5556A            DEVICE_VERSION_A    + DEVICE_55XX + (0x56)
#define DEVICE_5523A            DEVICE_VERSION_A    + DEVICE_55XX + (0x23)
#define DEVICE_5532B            DEVICE_VERSION_B    + DEVICE_55XX + (0x32)
#define DEVICE_5524A            DEVICE_VERSION_A    + DEVICE_55XX + (0x24)
#define DEVICE_5526A            DEVICE_VERSION_A    + DEVICE_55XX + (0x26)
#define DEVICE_5527A            DEVICE_VERSION_A    + DEVICE_55XX + (0x27)

//=====================================
// Shunt Configurations (CONFIG_SHUNT)
//=====================================
#define CONFIG_SHUNT_1SHUNT     1
#define CONFIG_SHUNT_3SHUNT     3

//=====================================
// Power Measure Configurations (POWER_MEASURE_SELECT)
//=====================================
#define POWER_MEASURE_METHOD_1   1
#define POWER_MEASURE_METHOD_2   2

//=====================================
// Inverter Types (INVERTER_TYPE_SEL)
//=====================================
#define INVERTER_TYPE_MOS       1
#define INVERTER_TYPE_IGBT      2

//==========================================================================
//
// Port Definitions
//
//==========================================================================

#define PORT_PA3                (1 << 3)
#define PORT_PA4                (1 << 4)
#define PORT_PA5                (1 << 5)

#define PORT_PD0                (1 << (8+0))
#define PORT_PD1                (1 << (8+1))
#define PORT_PD2                (1 << (8+2))
#define PORT_PD3                (1 << (8+3))
#define PORT_PD4                (1 << (8+4))
#define PORT_PD5                (1 << (8+5))
#define PORT_PD6                (1 << (8+6))
#define PORT_PD7                (1 << (8+7))

#define PORT_PE0                (1 << (16+0))
#define PORT_PE1                (1 << (16+1))
#define PORT_PE2                (1 << (16+2))
#define PORT_PE3                (1 << (16+3))
#define PORT_PE4                (1 << (16+4))
#define PORT_PE5                (1 << (16+5))

#define PORT_PF0                (1 << (24+0))
#define PORT_PF1                (1 << (24+1))
#define PORT_PF2                (1 << (24+2))
#define PORT_PF3                (1 << (24+3))
#define PORT_PF4                (1 << (24+4))
#define PORT_PF5                (1 << (24+5))

//==========================================================================
//
// AIO50 Option Definitions
//
//==========================================================================

//=====================================
// Analog IO Mode (AIOxx_MODE)
//=====================================
#define AIO50_MODE_IO           0
#define AIO50_MODE_DIFFAMP      1

//======================================================
// Definitions for Diffamp Mode
//======================================================

//=====================================
// Diffamp Gain (AIOxx_GAIN)
//=====================================
#define AIO50_GAIN_X1           1
#define AIO50_GAIN_X2           2
#define AIO50_GAIN_X4           3
#define AIO50_GAIN_X8           4
#define AIO50_GAIN_X16          5
#define AIO50_GAIN_X32          6
#define AIO50_GAIN_X48          7

//=====================================
// Low OC Protection Comparator Config (AIOxx_LPCOMP)
//=====================================
#define AIO50_LPCOMP_DISABLE    0
#define AIO50_LPCOMP_1US        1
#define AIO50_LPCOMP_2US        2
#define AIO50_LPCOMP_4US        3

//=====================================
// Low OC Protection Mid-Ref Offset (AIOxx_OFFSET)
//=====================================
#define AIO50_OFFSET_DISABLED   0
#define AIO50_OFFSET_ENABLED    1

//=====================================
// High OC Protection Comparator Config (AIOxx_HPCOMP)
//=====================================
#define AIO50_HPCOMP_DISABLE    0
#define AIO50_HPCOMP_1US        1
#define AIO50_HPCOMP_2US        2
#define AIO50_HPCOMP_4US        3

//======================================================
// Definitions for IO Mode
//======================================================

//=====================================
// IO Mode (AIOx_OPT)
//=====================================
#define AIO50_OPT_INPUT         0        // Input mode
#define AIO50_OPT_OD            2        // Output mode, Open-Drain

//=====================================
// IO Polarity (AIOx_POL)
//=====================================
#define AIO50_POL_AH            0           // Polarity = Active High
#define AIO50_POL_AL            1           // Polarity = Active Low

//=====================================
// IO Digital Bus Signal (AIOx_MUX)
//=====================================
#define AIO50_MUX_NONE          0
#define AIO50_MUX_DB1           1
#define AIO50_MUX_DB2           2
#define AIO50_MUX_DB3           3
#define AIO50_MUX_DB4           4
#define AIO50_MUX_DB5           5
#define AIO50_MUX_DB6           6
#define AIO50_MUX_DB7           7


//==========================================================================
//
// AIO6789 Option Definitions
//
//==========================================================================

//=====================================
// Analog IO Mode (AIOx_MODE)
//=====================================
#define AIO6789_MODE_IO         0
#define AIO6789_MODE_GAIN       1
#define AIO6789_MODE_COMP       2
#define AIO6789_MODE_SPECIAL    3

//======================================================
// Definitions for Diffamp Mode
//======================================================

//=====================================
// Amplifier Gain (AIOx_GAIN)
//=====================================
#define AIO6789_GAIN_DIRECT     0
#define AIO6789_GAIN_X1         1
#define AIO6789_GAIN_X2         2
#define AIO6789_GAIN_X4         3
#define AIO6789_GAIN_X8         4
#define AIO6789_GAIN_X16        5
#define AIO6789_GAIN_X32        6
#define AIO6789_GAIN_X48        7

//=====================================
// IO Analog Bus Signal (AIOx_MUX)
//=====================================
#define AIO6789_MUX_DEFAULT     0
#define AIO6789_MUX_AB1         1
#define AIO6789_MUX_AB2         2
#define AIO6789_MUX_AB3         3
#define AIO6789_MUX_AB4         4
#define AIO6789_MUX_AB5         5
#define AIO6789_MUX_AB6         6

//======================================================
// Definitions for IO Mode
//======================================================

//=====================================
// IO Mode (AIOx_OPT)
//=====================================
#define AIO6789_OPT_INPUT       0        // Input mode
#define AIO6789_OPT_OD          2        // Output mode, Open-Drain

//=====================================
// IO Polarity (AIOx_POL)
//=====================================
#define AIO6789_POL_AH          0           // Polarity = Active High
#define AIO6789_POL_AL          1           // Polarity = Active Low

//======================================================
// Definitions for Port Types
//======================================================
#define PORT_OTHER              0
#define PORT_VPHASE             1
#define PORT_IPHASE             2

//======================================================
// Definitions for AIO Channels
//======================================================
#define CHANNEL_NONE            0
#define CHANNEL_AIO10           (1 << 0)
#define CHANNEL_AIO32           (1 << 1)
#define CHANNEL_AIO54           (1 << 2)
#define CHANNEL_AIO6            (1 << 3)
#define CHANNEL_AIO7            (1 << 4)
#define CHANNEL_AIO8            (1 << 5)
#define CHANNEL_AIO9            (1 << 6)

//======================================================
// Definitions for ADC Gain and Offset Compensation
//======================================================
#define VMS_INPUT               (CHANNEL_AIO9 << 1)
#define EXT_VBUS                (VMS_INPUT << 1)

#endif  //#ifndef PORT_H
