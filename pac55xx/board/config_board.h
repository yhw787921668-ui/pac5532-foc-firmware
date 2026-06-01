
//=============================================================================
// Copyright (C) 2018 - 2020, Qorvo, Inc.
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

#define BOARD_CUSTOM_1                   1
#define BOARD_PAC5523EVK1               10                      // Also covers EP5523LVM1
#define BOARD_PAC5524EVK1               11
#define BOARD_PAC5527EVK1               12
#define BOARD_PAC5532EVK1               13
#define BOARD_PAC5556EVK1               14
#define BOARD_PAC5556AEVK1              15
#define BOARD_PAC5523AEVK1              16
#define BOARD_PAC5532BEVK1              17
#define BOARD_PAC5524AEVK1              18
#define BOARD_PAC5527AEVK1              19

// Single Shunt
#define BOARD_PAC5523EVK1_1_1SHUNT      20
#define BOARD_PAC5524EVK1_1_1SHUNT      21
#define BOARD_PAC5532EVK1_1SHUNT        22
#define BOARD_PAC5556EVK1_1SHUNT        23
#define BOARD_PAC5556AEVK1_1SHUNT       24
#define BOARD_PAC5523AEVK1_1_1SHUNT     25
#define BOARD_PAC5532BEVK1_1SHUNT       26
#define BOARD_PAC5526EVK1               27
#define BOARD_PAC5526AEVK1              28

// Power Control Configurations
#define BOARD_PAC5523EVK1_1_1SHUNT_PWR2     30
#define BOARD_PAC5524EVK1_1_1SHUNT_PWR2     31
#define BOARD_PAC5523AEVK1_1_1SHUNT_PWR2    40

// Engineering Revision Boards
#define BOARD_PAC5527HCRD1_ENG        1000
#define BOARD_PAC5524RDPS_ENG         1001


//========================================
// Board Selection
//========================================
//#define BOARD_SELECT                BOARD_CUSTOM_1
//#define BOARD_SELECT                BOARD_PAC5523EVK1                       // Also covers EP5523LVM1
//#define BOARD_SELECT                BOARD_PAC5523AEVK1
//#define BOARD_SELECT                BOARD_PAC5524EVK1
//#define BOARD_SELECT                BOARD_PAC5526EVK1
//#define BOARD_SELECT                BOARD_PAC5527EVK1
#define BOARD_SELECT                BOARD_PAC5532EVK1
//#define BOARD_SELECT                BOARD_PAC5532BEVK1                    // Version B
//#define BOARD_SELECT                BOARD_PAC5556EVK1
//#define BOARD_SELECT                BOARD_PAC5556AEVK1
//#define BOARD_SELECT                BOARD_PAC5523EVK1_1_1SHUNT            // Also covers EP5523LVM1
//#define BOARD_SELECT                BOARD_PAC5523AEVK1_1_1SHUNT
//#define BOARD_SELECT                BOARD_PAC5524EVK1_1_1SHUNT
//#define BOARD_SELECT                BOARD_PAC5532EVK1_1SHUNT
//#define BOARD_SELECT                BOARD_PAC5532BEVK1_1SHUNT             // Version B
//#define BOARD_SELECT                BOARD_PAC5556EVK1_1SHUNT
//#define BOARD_SELECT                BOARD_PAC5556AEVK1_1SHUNT
//#define BOARD_SELECT                BOARD_PAC5523EVK1_1_1SHUNT_PWR2       // Also covers EP5523LVM1
//#define BOARD_SELECT                BOARD_PAC5523AEVK1_1_1SHUNT_PWR2
//#define BOARD_SELECT                BOARD_PAC5524EVK1_1_1SHUNT_PWR2
//#define BOARD_SELECT                BOARD_PAC5524AEVK1
//#define BOARD_SELECT                BOARD_PAC5526AEVK1
//#define BOARD_SELECT                BOARD_PAC5527AEVK1

//#define BOARD_SELECT                BOARD_PAC5527HCRD1_ENG
//#define BOARD_SELECT                BOARD_PAC5524RDPS_ENG


#endif  //#ifndef CONFIG_BOARD_H
