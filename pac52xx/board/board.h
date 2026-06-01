
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

#ifndef BOARD_H
#define BOARD_H

#include "config_board.h"

#if (BOARD_SELECT == BOARD_PAC5222EVK1)           
    #include "board_pac5222evk1.h"
#elif (BOARD_SELECT == BOARD_EP5223LVM1_1)
    #include "board_ep5223lvm1_1.h"
#elif (BOARD_SELECT == BOARD_PAC5232EVK1)           // Also covers EP5232MVM1
    #include "board_pac5232evk1.h"
#elif (BOARD_SELECT == BOARD_EP5250HPHVM1_2)
    #include "board_ep5250hphvm1_2.h"
#elif (BOARD_SELECT == BOARD_PAC5256EVK1_REV_AB)
    #include "board_pac5256evk1.h"
#elif (BOARD_SELECT == BOARD_PAC5256EVK1_REV_C)
    #include "board_pac5256evk1.h"
#elif (BOARD_SELECT == BOARD_EP5285LVM1_1)
    #include "board_ep5285lvm1_1.h"
#elif (BOARD_SELECT == BOARD_CUSTOM_1)
    #include "board_custom_1.h"

#elif (BOARD_SELECT == BOARD_EP5223LVM1_1_2SHUNT_PWR2)
    #include "board_ep5223lvm1_1_2shunt_pwr2.h"
#elif (BOARD_SELECT == BOARD_EP5222LVM1_1_1SHUNT)
    #include "board_ep5222lvm1_1_1shunt.h"
#elif (BOARD_SELECT == BOARD_EP5223LVM1_1_1SHUNT)
    #include "board_ep5223lvm1_1_1shunt.h"
#elif (BOARD_SELECT == BOARD_EP5223LVM1_1_1SHUNT_PWR2)
    #include "board_ep5223lvm1_1_1shunt_pwr2.h"    
#elif (BOARD_SELECT == BOARD_EP5250HPHVM1_2_1SHUNT)
    #include "board_ep5250hphvm1_2_1shunt.h"
#elif (BOARD_SELECT == BOARD_PAC5232EVK1_1SHUNT)
    #include "board_pac5232evk1_1shunt.h"
#elif (BOARD_SELECT == BOARD_PAC5256EVK1_REV_AB_1SHUNT)
    #include "board_pac5256evk1_1shunt.h"
#elif (BOARD_SELECT == BOARD_PAC5256EVK1_REV_C_1SHUNT)
    #include "board_pac5256evk1_1shunt.h"
#elif (BOARD_SELECT == BOARD_PAC52710EVK1)
    #include "board_pac52710evk1.h"
#elif (BOARD_SELECT == BOARD_PAC52711EVK1)
    #include "board_pac52711evk1.h"
#elif (BOARD_SELECT == BOARD_PAC52400EVK1)
    #include "board_pac52400evk1.h"
#else
    #error "BOARD_SELECT has not been defined"
#endif


#if (DEVICE_SELECT == DEVICE_5220)
    #include "device_5220.h"
#elif (DEVICE_SELECT == DEVICE_5222)
    #include "device_5222.h"
#elif (DEVICE_SELECT == DEVICE_5223)
    #include "device_5223.h"
#elif (DEVICE_SELECT == DEVICE_5250)
    #include "device_5250.h"
#elif (DEVICE_SELECT == DEVICE_5253)
    #include "device_5253.h"
#elif (DEVICE_SELECT == DEVICE_5232)
   #include "device_5232.h"
#elif (DEVICE_SELECT == DEVICE_5256)
   #include "device_5256.h"
#elif (DEVICE_SELECT == DEVICE_5285)
   #include "device_5285.h"
#elif (DEVICE_SELECT == DEVICE_52710)
   #include "device_52710.h"
#elif (DEVICE_SELECT == DEVICE_52711)
   #include "device_52711.h"
#elif (DEVICE_SELECT == DEVICE_52400)
   #include "device_52400.h"
#else
    #error "No valid Device defined in board configuration."
#endif


#if (AIO6_GAIN == AIO6789_GAIN_X0)
    #define AMPGAIN_AIO6              0
#elif (AIO6_GAIN == AIO6789_GAIN_X1)
    #define AMPGAIN_AIO6              1
#elif (AIO6_GAIN == AIO6789_GAIN_X2)
    #define AMPGAIN_AIO6              2
#elif (AIO6_GAIN == AIO6789_GAIN_X4)
    #define AMPGAIN_AIO6              4
#elif (AIO6_GAIN == AIO6789_GAIN_X8)
    #define AMPGAIN_AIO6              8
#elif (AIO6_GAIN == AIO6789_GAIN_X16)
    #define AMPGAIN_AIO6              16
#elif (AIO6_GAIN == AIO6789_GAIN_X32)
    #define AMPGAIN_AIO6              32
#elif (AIO6_GAIN == AIO6789_GAIN_X48)
    #define AMPGAIN_AIO6              48
#else
    #error: Invalid value for AIO6_GAIN.
#endif

#endif  //#ifndef BOARD_H
