/**************************************************************************//**
 * @file     pac5xxx_tile_DriverManager.h
 * @brief    CMSIS Header File for the PAC5XXX System Manager Tile
 *
 * @note
 * Copyright (C) 2015-2023, Qorvo, Inc.
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

#ifndef PAC5XXX_TILE_DRIVERMANAGER_H
#define PAC5XXX_TILE_DRIVERMANAGER_H

/** @addtogroup PAC5XXX_Tile_DriverManager PAC5XXX Driver Manager Tile
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
/*!< Driver Manager Register Addresses */

typedef enum {
    ADDR_CFGDRV0 = 0x26,                    /*!< Driver Manager Configuration 0 Register                    */
    ADDR_CFGDRV1 = 0x27,                    /*!< Driver Manager Configuration 1 Register                    */
    ADDR_CFGDRV2 = 0x28,                    /*!< Driver Manager Configuration 2 Register                    */
    ADDR_CFGDRV3 = 0x29,                    /*!< Driver Manager Configuration 3 Register                    */
    ADDR_STATDRV = 0x2A,                    /*!< Driver Manager Status Register                             */
    #ifdef PAC5256
        ADDR_CFGDRV4 = 0x7B,                /*!< Driver Manager Configuration 4 Register                    */
        ADDR_DRV_FLT = 0x7C,                /*!< Driver Manager Driver Fault Flag Register                  */
    #endif
    ADDR_ENDRV = 0x7D,                      /*!< Driver Manager Enable Register                             */
    ADDR_WDT_PASSWD = 0x7E                  /*!< Driver Manager Watchdog Timer Password                     */
} Driver_Manager_Reg_Addr;
#endif	// CAFE_ARCH2


#ifdef CAFE_ARCH3
//================================
//========== CAFE_ARCH3 ==========
//================================
/*!< Driver Manager Register Addresses */

typedef enum {
    ADDR_LOWPOWER = 0x78,               /*!< Driver Manager Configuration For Low Power Mode                  */
    ADDR_DRV_FLT = 0x7C,                /*!< Driver Manager Driver Fault Flag Register                  */
    ADDR_ENDRV = 0x7D,                      /*!< Driver Manager Enable Register                             */
    ADDR_WDT_PASSWD = 0x7E                  /*!< Driver Manager Watchdog Timer Password                     */
} Driver_Manager_Reg_Addr;
#endif  // CAFE_ARCH2


#ifdef CAFE_ARCH1
//================================
//========== CAFE_ARCH1 ==========
//================================

/*!< Driver Manager Register Addresses */

typedef enum {
#if defined PAC52710  || defined PAC52711
    ADDR_CFGVDS1 = 0x40,
    ADDR_CFGVDS2 = 0x41,
    ADDR_CFGCBC = 0x42,
    ADDR_CBCFLAG = 0x43,
#endif
    ADDR_CFGDRV0 = 0x60,                    /*!< Driver Manager Configuration 0 Register                    */
    ADDR_CFGDRV1 = 0x61,                    /*!< Driver Manager Configuration 1 Register                    */
#if defined(PAC5222)
    ADDR_CLKOUTCFG = 0x62,                  /*!< CLKOUT Configuration                                           */
    ADDR_CFGDRV2 = 0x63,                    /*!< Driver Manager Configuration 2 Register                                            */
#else
    ADDR_CFGDRQ6 = 0x62,                    /*!< DRQ6 Configuration                                         */
    ADDR_CFGDRQ7 = 0x63,                    /*!< DRQ7 Configuration                                         */
#endif
#if !defined(PAC52710) || !defined PAC5211
    ADDR_DOUTDRV = 0x64,                    /*!< Driver Manager Data output                                 */
    ADDR_DINDRV = 0x65,                     /*!< Driver Manager Data input                                  */
#endif
    ADDR_ENDRV = 0x66,                      /*!< Enable Driver Manager Tile                                 */
#if defined(PAC5220) || defined(PAC5222) || defined(PAC5223) || defined(PAC5225) || defined(PAC52710)  || defined (PAC52711)  || defined (PAC52400)
    ADDR_ENBBM = 0x67,                      /*!< Enable BBM (break before make) function                    */
    ADDR_PROTCTL = 0x68                     /*!< Over Current Protection Control Signal Register           */
#else
    ADDR_PROTCTL = 0x67                     /*!< Over Current Protection Control Signal Register            */
#endif
} Driver_Manager_Reg_Addr;

//
// Type Enumerations
//

/** PAC5XXX Propogation Delay Type Enumeration */
typedef enum
{
    PAC5XXX_PROP_DEL_0ns = 0,               /*!< Propagation Delay: 0ns                                     */
    PAC5XXX_PROP_DEL_50ns = 1,              /*!< Propagation Delay: 50ns                                    */
    PAC5XXX_PROP_DEL_100ns = 2,             /*!< Propagation Delay: 100ns                                   */
    PAC5XXX_PROP_DEL_200ns = 3              /*!< Propagation Delay: 200ns                                   */
} PAC5XXX_PropDelay_Type;

/** PAC5XXX Driver Option Setting Type Enumeration */
typedef enum
{
    PAC5XXX_DRVOPT_IN = 0,                  /*!< Driver Option: Input                                       */
    PAC5XXX_DRVOPT_ODSTRONG = 1,            /*!< Driver Option: Open-Drain Strong Driver                    */
    PAC5XXX_DRVOPT_OD = 2                   /*!< Driver Option: Open-Drain Output                           */
} PAC5XXX_DriverOption_Type;

/** PAC5XXX Driver Polarity Setting Type Enumeration */
typedef enum
{
    PAC5XXX_POLARITY_PAC5XXXHIGH = 0,       /*!< Driver Polarity Setting: active high                       */
    PAC5XXX_POLARITY_PAC5XXXLOW = 1         /*!< Driver Polarity Setting: active low                        */
} PAC5XXX_DriverPolarity_Type;

/** PAC5XXX Driver MUX Setting Type Enumeration */
typedef enum
{
    PAC5XXX_DRVMUX_DRQnIN = 0,              /*!< Driver MUX Setting: DRQ<n>IN                               */
    PAC5XXX_DRVMUX_DB1 = 1,                 /*!< Driver MUX Setting: DB[1]                                  */
    PAC5XXX_DRVMUX_DB2 = 2,                 /*!< Driver MUX Setting: DB[2]                                  */
    PAC5XXX_DRVMUX_DB3 = 3,                 /*!< Driver MUX Setting: DB[3]                                  */
    PAC5XXX_DRVMUX_DB4 = 4,                 /*!< Driver MUX Setting: DB[4]                                  */
    PAC5XXX_DRVMUX_DB5 = 5,                 /*!< Driver MUX Setting: DB[5]                                  */
    PAC5XXX_DRVMUX_DB6 = 6,                 /*!< Driver MUX Setting: DB[6]                                  */
    PAC5XXX_DRVMUX_DB7 = 7                  /*!< Driver MUX Setting: DB[7]                                  */
} PAC5XXX_DriverMux_Type;


//
// Bit Definitions
//

/*!< Driver Manager Tile Registers */

#if defined(PAC52710)  || defined PAC52711
/*!< CFGVDS1: Address: 0x40     VDS Sense Configuration 1 Register */
typedef union
{
    __IO uint8_t b;
    struct {
        __IO uint8_t VDSBLNK                : 4;        /*!< VDS Sense Blanking Time                        */
        __IO uint8_t VDSHEN                 : 1;        /*!< VDS Sense High-side MOSFETs Enable             */
        __IO uint8_t VDSLEN                 : 1;        /*!< VDS Sense Low-side MOSFETs Enable              */
             uint8_t PROP2                  : 2;
    };
} CFGVDS1;

/*!< CFGVDS2: Address: 0x41     VDS Sense Configuration 2 Register */
typedef union
{
    __IO uint8_t b;
    struct {
        __IO uint8_t VDSTLH                 : 4;        /*!< VDS Trip Level High-side                       */
        __IO uint8_t VDSTLL                 : 4;        /*!< VDS Trip Level High-side                       */
    };
} CFGVDS2;

/*!< CFGCBC: Address: 0x42     Cycle-by-cycle Configuration Register */
typedef union
{
    __IO uint8_t b;
    struct {
        __IO uint8_t CBCTYPE                : 2;        /*!< Cycle-by-cycle Type                            */
        __IO uint8_t CBCMSKLPROT            : 1;        /*!< Cycle-by-cycle LPROT Trigger Mask              */
        __IO uint8_t CBCMSKHPROT            : 1;        /*!< Cycle-by-cycle HPROT Trigger Mask              */
        __IO uint8_t CBCMSKVDSTL            : 1;        /*!< Cycle-by-cycle VDS TRIPL Trigger Mask          */
        __IO uint8_t CBCMSKVDSTH            : 1;        /*!< Cycle-by-cycle VDS TRIPH Trigger Mask          */
        __IO uint8_t VDSTLnMSK              : 1;        /*!< Cycle-by-cycle VDS TRIPL Interrupt Mask        */
        __IO uint8_t VDSTHnMSK              : 1;        /*!< Cycle-by-cycle VDS TRIPH Interrupt Mask        */
    };
} CFGCBC;

/*!< CBCFLAG: Address: 0x43     Cycle-by-cycle Status Flag Register */
typedef union
{
    __IO uint8_t b;
    struct {
        __IO uint8_t VDSTL                  : 1;        /*!< VDS TRIPL Interrupt Flag                       */
        __IO uint8_t VDSTH                  : 1;        /*!< VDS TRIPH Interrupt Flag                       */
             uint8_t                        : 6;
    };
} CBCFLAG;
#endif

/*!< CFGDRV0: Address: 0x60     Driver Manager Configuration 0 Register */
typedef union
{
    __IO uint8_t b;
    struct {
        __IO uint8_t PROP0                  : 2;        /*!< Prop Delay 0 (\a PAC5XXX_PropDelay_Type)       */
        __IO uint8_t PROP1                  : 2;        /*!< Prop Delay 1 (\a PAC5XXX_PropDelay_Type)       */
        __IO uint8_t PROP2                  : 2;        /*!< Prop Delay 2 (\a PAC5XXX_PropDelay_Type)       */
        __IO uint8_t PROP3                  : 2;        /*!< Prop Delay 3 (\a PAC5XXX_PropDelay_Type)       */
    };
} CFGDRV0;

/*!< CFGDRV1: Address: 0x61     Driver Manager Configuration 1 Register */
typedef union
{
    __IO uint8_t b;
    struct {
        __IO uint8_t PROP4                  : 2;        /*!< Prop Delay 4 (\a PAC5XXX_PropDelay_Type)       */
        __IO uint8_t PROP5                  : 2;        /*!< Prop Delay 5 (\a PAC5XXX_PropDelay_Type)       */
        __IO uint8_t nLSPR2M                : 1;        /*!< Low-Side PR2 Protection Mask (act low)         */
        __IO uint8_t nLSPR1M                : 1;        /*!< Low-Side PR1 Protection Mask (act low)         */
        __IO uint8_t nHSPR2M                : 1;        /*!< High-Side PR2 Protection Mask (act low)        */
        __IO uint8_t nHSPR1M                : 1;        /*!< High-Side PR1 Protection Mask (act low)        */
    };
} CFGDRV1;

#if defined(PAC5222)
/*!< CLKOUTCFG: Address: 0x62   CLKOUT Configuration */
typedef union
{
    __IO uint8_t b;
    struct {
        __IO uint8_t CLKOUTSEL              : 2;        /*!< CLKOUT Selection                               */
        __IO uint8_t EN_CLKOUT              : 1;        /*!< Enable CLKOUT                                  */
             uint8_t                        : 5;
    };
} CLKOUTCFG;

/*!< CFGDRV2: Address: 0x63     Driver Manager Configuration 2 */
typedef union
{
    __IO uint8_t b;
    struct {
        __IO uint8_t                        : 1;
        __IO uint8_t PULSEEXTDIS            : 1;        /*!< Pulse Extension Disable                        */
             uint8_t                        : 6;
    };
} CFGDRV2;
#else
/*!< CFGDRQ6: Address: 0x62     DRQ6 Configuration */
typedef union
{
    __IO uint8_t b;
    struct {
        __IO uint8_t MUX6                   : 3;        /*!< DRQ6 MUX (\a PAC5XXX_DriverMux_Type)           */
        __IO uint8_t POL6                   : 1;        /*!< DRQ6 Polarity (\a PAC5XXX_DriverPolarity_Type) */
        __IO uint8_t OPT6                   : 2;        /*!< DRQ6 Option (\a PAC5XXX_DriverOption_Type)     */
             uint8_t                        : 2;
    };
} CFGDRQ6;

/*!< CFGDRQ7: Address: 0x63     DRQ7 Configuration */
typedef union
{
    __IO uint8_t b;
    struct {
        __IO uint8_t MUX7                   : 3;        /*!< DRQ7 MUX (\a PAC5XXX_DriverMux_Type)           */
        __IO uint8_t POL7                   : 1;        /*!< DRQ7 Polarity (\a PAC5XXX_DriverPolarity_Type) */
        __IO uint8_t OPT7                   : 2;        /*!< DRQ7 Option (\a PAC5XXX_DriverOption_Type)     */
             uint8_t                        : 2;
    };
} CFGDRQ7;
#endif

#if !defined(PAC52710)  || defined PAC52711
/*!< DOUTDRV: Address: 0x64     Driver Manager Output Data */
typedef union
{
    __IO uint8_t b;
    struct {
        __IO uint8_t DOUTS3                 : 1;        /*!< Output data for DRS3 open-drain driver         */
        __IO uint8_t DOUTB3                 : 1;        /*!< Output data for DRB3 open-drain driver         */
        __IO uint8_t DOUTS4                 : 1;        /*!< Output data for DRS4 open-drain driver         */
        __IO uint8_t DOUTB4                 : 1;        /*!< Output data for DRB4 open-drain driver         */
        __IO uint8_t DOUTS5                 : 1;        /*!< Output data for DRS5 open-drain driver         */
        __IO uint8_t DOUTB5                 : 1;        /*!< Output data for DRB5 open-drain driver         */
        __IO uint8_t DOUTQ6                 : 1;        /*!< Output data for DRQ6 in open-drain mode        */
        __IO uint8_t DOUTQ7                 : 1;        /*!< Output data for DRQ7 in open-drain mode        */
    };
} CFGDOUTDRV;

/*!< DINDRV: Address: 0x65      Driver Manager Input Data */
typedef union
{
    __IO uint8_t b;
    struct {
             uint8_t                        : 6;
        __IO uint8_t DINQ6                  : 1;        /*!< Input data from DRQ7 in input mode             */
        __IO uint8_t DINQ7                  : 1;        /*!< Input data from DRQ6 in input mode             */
    };
} DINDRV;
#endif

/*!< ENDRV: Address: 0x66       Driver Manager Tile Enable */
typedef union
{
    __IO uint8_t b;
    struct {
        __IO uint8_t EN                     : 1;        /*!< 0: Disable; 1: Enable                          */
             uint8_t                        : 7;
    };
} ENDRV;

#if defined(PAC52710)  || defined PAC52711
/*!< ENBBM: Address: 0x67       Driver Break Before Make Control */
typedef union
{
    __IO uint8_t b;
    struct {
        __IO uint8_t ENBBM                  : 1;        /*!< 0: Disable; 1: Enable                          */
             uint8_t                        : 7;
    };
} ENBBM;

/*!< PROTCTL: Address: 0x67/0x68    Driver Protection Control */
typedef union
{
    __IO uint8_t b;
    struct {
        __IO uint8_t PROTSEL                : 1;        /*!< 0: Disable; 1: Enable                          */
             uint8_t                        : 7;
    };
} PROTCTL;
#endif // #if defined(PAC52710)  || defined PAC52711

#endif // #ifdef CAFE_ARCH1


/*@}*/ /* end of group PAC5XXX_Tile_DriverManager */

#endif  // PAC5XXX_TILE_DRIVERMANAGER_H
