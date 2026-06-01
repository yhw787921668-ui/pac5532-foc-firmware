/**************************************************************************//**
 * @file     pac5xxx_tile_SystemManager.h
 * @brief    CMSIS Header File for the PAC5XXX System Manager Tile
 *
 * @note
 * Copyright (C) 2015-2020, Qorvo, Inc.
 *
 * THIS SOFTWARE IS SUBJECT TO A SOURCE CODE LICENSE AGREEMENT WHICH PROVIDES,
 * AMONG OTHER THINGS:  (i) THAT IT CAN BE USED ONLY TO ADAPT THE LICENSEE'S
 * APPLICATION TO PAC PROCESSORS SUPPLIED BY QORVO, INC.;
 * (ii) THAT  IT IS PROVIDED "AS IS" WITHOUT WARRANTY;  (iii) THAT
 * QORVO, INC. IS NOT LIABLE FOR ANY INDIRECT DAMAGES OR FOR DIRECT
 * DAMAGES EXCEEDING US$1,500;  AND (iv) THAT IT CAN BE DISCLOSED TO AND USED
 * ONLY BY CERTAIN AUTHORIZED PERSONS.
 *
 ******************************************************************************/

#ifndef PAC5XXX_TILE_SYSTEMMANAGER_H
#define PAC5XXX_TILE_SYSTEMMANAGER_H

/** @addtogroup PAC5XXX_Tile_SystemManager_INT PAC5XXX System Manager Tile
  @{
*/

#ifdef __CC_ARM  // KEIL Compiler
#pragma anon_unions
#endif

#if !defined(CAFE_ARCH1) && !defined(CAFE_ARCH2) && !defined(CAFE_ARCH3)
    #error Must define CAFE Architecture; include appropriate PAC52xx device header before including this file (Example #include "pac5223.h")
#endif

#ifdef CAFE_ARCH2
//================================
//========== CAFE_ARCH2 ==========
//================================
/*!< System Manager Tile Register Addresses */

//typedef enum {
//} System_Manager_Reg_Addr;
#endif  // CAFE_ARCH2


#ifdef CAFE_ARCH3
//================================
//========== CAFE_ARCH3 ==========
//================================
/*!< System Manager Tile Register Addresses */

//typedef enum {
//} System_Manager_Reg_Addr;
#endif  // CAFE_ARCH3


#ifdef CAFE_ARCH1
//================================
//========== CAFE_ARCH1 ==========
//================================

/*!< System Manager Tile Register Addresses */

typedef enum {
    ADDR_SYSSTAT = 0x00,                    /*!< System Manager Status Register                             */

#if !defined(PAC5222)
    ADDR_DEVID = 0x08,                      /*!< Device Identification Register                             */
    ADDR_VERID = 0x09,                      /*!< Version Identification Register                            */
#endif

#if defined(PAC52710)  || defined PAC52711
    ADDR_WWDTCTL = 0x50,
    ADDR_WWDTCTR = 0x51,
    ADDR_WWDTCDV = 0x52,
    ADDR_WWDTWIN = 0x53,
    ADDR_WWDTRST = 0x54,
    ADDR_USER = 0x55
#endif
} System_Manager_Reg_Addr;

//
// Type Enumerations
//

/** PAC5XXX Threshold Voltage for Comparator AIO9-6 Type Enumeration */
typedef enum
{
    PAC5XXX_VTHRESH_0p1V = 0,               /*!< Threshold voltage for comparator for AIO9-6: 0.1V          */
    PAC5XXX_VTHRESH_0p2V = 1,               /*!< Threshold voltage for comparator for AIO9-6: 0.2V          */
    PAC5XXX_VTHRESH_0p5V = 2,               /*!< Threshold voltage for comparator for AIO9-6: 0.5V          */
    PAC5XXX_VTHRESH_1p25V = 3,              /*!< Threshold voltage for comparator for AIO9-6: 1.25V         */
} PAC5XXX_Vthresh_Comp_Type;

//
// Bit Definitions
//

/*!< System Manager Tile Registers */

#if defined(PAC52710)  || defined PAC52711
/*!< WWDTCTL: Address: 0x50     WWDT Control Register */
typedef union
{
    __IO uint8_t b                          : 8;
    struct {
        __IO uint8_t WWDTEN                 : 1;        /*!< WWDT Enable                                */
        __IO uint8_t CLKDIV                 : 2;        /*!< WWDT Clock Divider                         */
        __IO uint8_t KEY                    : 5;        /*!< WWDT Unlock Key                            */
    };
} WWDTCTL;

/*!< WWDTCTR: Address: 0x51     WWDT Counter Register */
typedef union
{
    __I  uint8_t b                          : 8;
    struct {
        __I  uint8_t CTR                    : 8;        /*!< WWDT Current Counter Value                 */
    };
} WWDTCTR;

/*!< WWDTCDV: Address: 0x52     WWDT Count-Down Value Register */
typedef union
{
    __IO uint8_t b                          : 8;
    struct {
        __IO uint8_t CDV                    : 8;        /*!< WWDT Count-Down Value                      */
    };
} WWDTCDV;

/*!< WWDTWIN: Address: 0x53     WWDT Window Value Register */
typedef union
{
    __IO uint8_t b                          : 8;
    struct {
        __IO uint8_t WINDOW                 : 8;        /*!< WWDT Window Value                          */
    };
} WWDTWIN;

/*!< WWDTRST: Address: 0x54     WWDT Reset Register */
typedef union
{
    __IO uint8_t b                          : 8;
    struct {
        __IO uint8_t RESET                  : 8;        /*!< WWDT Reset                                 */
    };
} WWDTRST;

/*!< USER: Address: 0x55     User Register */
typedef union
{
    __IO uint8_t b                          : 8;
    struct {
        __IO uint8_t USER0                  : 1;        /*!< User Bit 0                                 */
        __IO uint8_t USER1                  : 1;        /*!< User Bit 1                                 */
        __IO uint8_t USER2                  : 1;        /*!< User Bit 2                                 */
        __IO uint8_t USER3                  : 1;        /*!< User Bit 3                                 */
        __IO uint8_t USER4                  : 1;        /*!< User Bit 4                                 */
        __IO uint8_t USER5                  : 1;        /*!< User Bit 5                                 */
        __IO uint8_t USER6                  : 1;        /*!< User Bit 6                                 */
        __IO uint8_t USER7                  : 1;        /*!< User Bit 7                                 */
    };
} USER;
#endif

/*!< SYSSTAT: Address: 0x00     System Manager Register */
typedef union
{
    __IO uint8_t b                          : 8;
    struct {
        __IO uint8_t nINTM                  : 1;        /*!< Interrupt mask: 0: masked; 1: not masked   */
        __IO uint8_t FLTM                   : 1;        /*!< Fault mask: 0: not masked; 1: masked       */
             uint8_t                        : 1;
        __IO uint8_t REFBP                  : 1;        /*!< Set external reference via AB5             */
        __IO uint8_t VTHREF                 : 2;        /*!< Threshold voltage for AIO<9:6> (00b: 0.1V, 01b: 0.2V, 10b: 0.5V, 11b: 1.25V    */
        __I  uint8_t TMPWARN                : 1;        /*!< Temperature warning. Clear on read.        */
        __IO uint8_t PBSTAT                 : 1;        /*!< Push button status. Write 1 to clear.      */
    };
} SYSSTAT;

#endif // #ifdef CAFE_ARCH1


/*@}*/ /* end of group PAC5XXX_Tile_SystemManager */

#endif  // PAC5XXX_TILE_SYSTEMMANAGER_H
