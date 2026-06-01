
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

#ifndef BOARD_H
#define BOARD_H

#include "config_board.h"

#if (BOARD_SELECT == BOARD_PAC5523EVK1)
    #include "board_pac5523evk1.h"
#elif (BOARD_SELECT == BOARD_PAC5523AEVK1)
    #include "board_pac5523Aevk1.h"
#elif (BOARD_SELECT == BOARD_PAC5532EVK1)
    #include "board_pac5532evk1.h"
#elif (BOARD_SELECT == BOARD_PAC5532BEVK1)
    #include "board_pac5532Bevk1.h"
#elif (BOARD_SELECT == BOARD_PAC5523EVK1_1_1SHUNT)
    #include "board_pac5523evk1_1_1shunt.h"
#elif (BOARD_SELECT == BOARD_PAC5523AEVK1_1_1SHUNT)
    #include "board_pac5523Aevk1_1_1shunt.h"
#elif (BOARD_SELECT == BOARD_PAC5523EVK1_1_1SHUNT_PWR2)
    #include "board_pac5523evk1_1_1shunt_pwr2.h"
#elif (BOARD_SELECT == BOARD_PAC5523AEVK1_1_1SHUNT_PWR2)
    #include "board_pac5523Aevk1_1_1shunt_pwr2.h"
#elif (BOARD_SELECT == BOARD_PAC5532EVK1_1SHUNT)
    #include "board_pac5532evk1_1shunt.h"
#elif (BOARD_SELECT == BOARD_PAC5532BEVK1_1SHUNT)
    #include "board_pac5532Bevk1_1shunt.h"
#elif (BOARD_SELECT == BOARD_PAC5524EVK1)
    #include "board_pac5524evk1.h"
#elif (BOARD_SELECT == BOARD_PAC5524EVK1_1_1SHUNT)
    #include "board_pac5524evk1_1_1shunt.h"
#elif (BOARD_SELECT == BOARD_PAC5524EVK1_1_1SHUNT_PWR2)
    #include "board_pac5524evk1_1_1shunt_pwr2.h"
#elif (BOARD_SELECT == BOARD_PAC5527EVK1)
    #include "board_pac5527evk1.h"
#elif (BOARD_SELECT == BOARD_PAC5556EVK1)
    #include "board_pac5556evk1.h"
#elif (BOARD_SELECT == BOARD_PAC5556AEVK1)
    #include "board_pac5556Aevk1.h"
#elif (BOARD_SELECT == BOARD_PAC5556EVK1_1SHUNT)
    #include "board_pac5556evk1_1shunt.h"
#elif (BOARD_SELECT == BOARD_PAC5556AEVK1_1SHUNT)
    #include "board_pac5556Aevk1_1shunt.h"
#elif (BOARD_SELECT == BOARD_PAC5527HCRD1_ENG)
    #include "board_pac5527hcrd1.h"
#elif (BOARD_SELECT == BOARD_PAC5524RDPS_ENG)
    #include "board_pac5524rdps.h"
#elif (BOARD_SELECT == BOARD_PAC5526EVK1)
    #include "board_pac5526evk1.h"
#elif (BOARD_SELECT == BOARD_PAC5524AEVK1)
    #include "board_pac5524Aevk1.h"
#elif (BOARD_SELECT == BOARD_PAC5526AEVK1)
    #include "board_pac5526Aevk1.h"
#elif (BOARD_SELECT == BOARD_PAC5527AEVK1)
    #include "board_pac5527Aevk1.h"
#elif (BOARD_SELECT == BOARD_CUSTOM_1)
    #include "board_custom_1.h"
#else
    #error "BOARD_SELECT has not been defined"
#endif


#if (DEVICE_SELECT == DEVICE_5523)
    #include "device_5523.h"
#elif (DEVICE_SELECT == DEVICE_5523A)
    #include "device_5523A.h"
#elif (DEVICE_SELECT == DEVICE_5527)
    #include "device_5527.h"
#elif (DEVICE_SELECT == DEVICE_5524)
    #include "device_5524.h"    
#elif (DEVICE_SELECT == DEVICE_5532)
    #include "device_5532.h"
#elif (DEVICE_SELECT == DEVICE_5532B)
    #include "device_5532B.h"
#elif (DEVICE_SELECT == DEVICE_5556)
    #include "device_5556.h"
#elif (DEVICE_SELECT == DEVICE_5526)
    #include "device_5526.h"
#elif (DEVICE_SELECT == DEVICE_5556A)
    #include "device_5556A.h"
#elif (DEVICE_SELECT == DEVICE_5524A)
    #include "device_5524A.h"
#elif (DEVICE_SELECT == DEVICE_5527A)
    #include "device_5527A.h"
#elif (DEVICE_SELECT == DEVICE_5526A)
    #include "device_5526A.h"
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
