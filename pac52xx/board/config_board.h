
/**
 * @file config_board.h
 * @brief Use for selecting different motor control EVK's boards
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

#ifndef CONFIG_BOARD_H
#define CONFIG_BOARD_H

//=============================================================================
//
// Board
//
//=============================================================================

//========================================
// Board Definitions for BOARD_SELECT
//========================================
#define BOARD_CUSTOM_1              0
#define BOARD_PAC5222EVK1           1
#define BOARD_EP5223LVM1_1          2
#define BOARD_PAC5232EVK1           3       // Also covers EP5232MVM1
#define BOARD_EP5250HPHVM1_2        4
#define BOARD_PAC5256EVK1_REV_AB    5
#define BOARD_PAC5256EVK1_REV_C     6
#define BOARD_EP5285LVM1_1          7
#define BOARD_PAC52710EVK1          8

//Non 3 shunt and Power Control Configurations
#define BOARD_EP5222LVM1_1_1SHUNT       9
#define BOARD_EP5223LVM1_1_1SHUNT       10
#define BOARD_EP5223LVM1_1_1SHUNT_PWR2  11
#define BOARD_EP5250HPHVM1_2_1SHUNT     12
#define BOARD_PAC5232EVK1_1SHUNT        13
#define BOARD_PAC5256EVK1_REV_AB_1SHUNT 14
#define BOARD_PAC5256EVK1_REV_C_1SHUNT  15
#define BOARD_PAC52711EVK1              16
#define BOARD_PAC52400EVK1              17

//========================================
// Board Selection
//========================================
//#define BOARD_SELECT                BOARD_CUSTOM_1
//#define BOARD_SELECT                BOARD_PAC5222EVK1
#define BOARD_SELECT                BOARD_EP5223LVM1_1
//#define BOARD_SELECT                BOARD_PAC5232EVK1            // Also covers EP5232MVM1
//#define BOARD_SELECT                BOARD_EP5250HPHVM1_2
//#define BOARD_SELECT                BOARD_PAC5256EVK1_REV_AB
//#define BOARD_SELECT                BOARD_PAC5256EVK1_REV_C
//#define BOARD_SELECT                BOARD_EP5285LVM1_1
//#define BOARD_SELECT                BOARD_PAC52710EVK1

//Non 3 shunt and Power Control Configurations
//#define BOARD_SELECT                BOARD_EP5222LVM1_1_1SHUNT
//#define BOARD_SELECT                BOARD_EP5223LVM1_1_1SHUNT
//#define BOARD_SELECT                BOARD_EP5223LVM1_1_1SHUNT_PWR2
//#define BOARD_SELECT                BOARD_EP5250HPHVM1_2_1SHUNT
//#define BOARD_SELECT                BOARD_PAC5232EVK1_1SHUNT
//#define BOARD_SELECT                BOARD_PAC5256EVK1_REV_AB_1SHUNT
//#define BOARD_SELECT                BOARD_PAC5256EVK1_REV_C_1SHUNT
//#define BOARD_SELECT                BOARD_PAC52711EVK1
//#define BOARD_SELECT                BOARD_PAC52400EVK1

#endif  //#ifndef CONFIG_BOARD_H

