
/**
 * @file can_gui.h
 * @brief Use for CAN parameter write, message processing
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

#ifndef CAN_GUI_H
#define CAN_GUI_H

#define CAN_ID_MASK ((0xFFFFU << 16 ) | (0x1FU << 8)) 
#define CAN_ID_CODE (((TARGET_CAN_ID & 0x7) << 13) | ((TARGET_CAN_ID >> 3) & 0xFF)) 

#define CAN_MESSAGE_SIZE   8U
/*
   | BYTE0 | BYTE1| BYTE2| BYTE3 | BYTE4 | BYTE5 | BYTE 6| BYTE 7| 
   | START | CODE |   D5 |    D4 |    D3 |    D2 |    D1 |    D0 |
*/
extern uint8_t can_rx_buffer[CAN_MESSAGE_SIZE];
extern uint8_t can_tx_buffer[CAN_MESSAGE_SIZE];


// CAN BUS Select BAUD Type
typedef enum
{
    CAN_BAUD_50KHz              = 0,
    CAN_BAUD_100KHz             = 1,
    CAN_BAUD_125KHz             = 2,
    CAN_BAUD_200KHz             = 3,
    CAN_BAUD_250KHz             = 4,
    CAN_BAUD_400KHz             = 5,
    CAN_BAUD_500KHz             = 6,
    CAN_BAUD_800KHz             = 7,
    CAN_BAUD_1000KHz            = 8,
} CAN_BAUD_TYPE;

// CAN IO Select Type
typedef enum 
{
    CAN_PC01                    = 0,
    CAN_PC45                    = 1,
    CAN_PD01                    = 2,
    CAN_PD56                    = 3,
    CAN_PD67                    = 4,
    CAN_PE23                    = 5,
    CAN_PE67                    = 6,
    CAN_PF67                    = 7,
    CAN_PG56                    = 8
} CAN_IO_TYPE;

// add by:owz
#if 1
typedef union
{
    volatile uint32_t w;
    struct {
            volatile uint32_t sid3           : 1; //bit00
            volatile uint32_t sid4           : 1; //bit01
            volatile uint32_t sid5           : 1; //bit02
            volatile uint32_t sid6           : 1; //bit03
            volatile uint32_t sid7           : 1; //bit04
            volatile uint32_t sid8           : 1; //bit05
            volatile uint32_t sid9           : 1; //bit06
            volatile uint32_t sid10          : 1; //bit07
                     uint32_t                : 1; //bit08
                     uint32_t                : 1; //bit09
                     uint32_t                : 1; //bit10
                     uint32_t                : 1; //bit11
            volatile uint32_t srtr           : 1; //bit12
            volatile uint32_t sid0           : 1; //bit13
            volatile uint32_t sid1           : 1; //bit14
            volatile uint32_t sid2           : 1; //bit15
                     uint32_t                : 1; //bit16
                     uint32_t                : 1; //bit17
                     uint32_t                : 1; //bit18
                     uint32_t                : 1; //bit19
                     uint32_t                : 1; //bit20
                     uint32_t                : 1; //bit21
                     uint32_t                : 1; //bit22
                     uint32_t                : 1; //bit23
                     uint32_t                : 1; //bit24
                     uint32_t                : 1; //bit25
                     uint32_t                : 1; //bit26
                     uint32_t                : 1; //bit27
                     uint32_t                : 1; //bit28
                     uint32_t                : 1; //bit29
                     uint32_t                : 1; //bit30
                     uint32_t                : 1; //bit31                                    
    };
    struct 
    {
            volatile uint32_t eid21           : 1; //bit00
            volatile uint32_t eid22           : 1; //bit01
            volatile uint32_t eid23           : 1; //bit02
            volatile uint32_t eid24           : 1; //bit03
            volatile uint32_t eid25           : 1; //bit04
            volatile uint32_t eid26           : 1; //bit05
            volatile uint32_t eid27           : 1; //bit06
            volatile uint32_t eid28           : 1; //bit07
            volatile uint32_t eid13           : 1; //bit08
            volatile uint32_t eid14           : 1; //bit09
            volatile uint32_t eid15           : 1; //bit10
            volatile uint32_t eid16           : 1; //bit11
            volatile uint32_t eid17           : 1; //bit12
            volatile uint32_t eid18           : 1; //bit13
            volatile uint32_t eid19           : 1; //bit14
            volatile uint32_t eid20           : 1; //bit15
            volatile uint32_t eid5            : 1; //bit16
            volatile uint32_t eid6            : 1; //bit17
            volatile uint32_t eid7            : 1; //bit18
            volatile uint32_t eid8            : 1; //bit19
            volatile uint32_t eid9            : 1; //bit20
            volatile uint32_t eid10           : 1; //bit21
            volatile uint32_t eid11           : 1; //bit22
            volatile uint32_t eid12           : 1; //bit23
                     uint32_t                 : 1; //bit24
                     uint32_t                 : 1; //bit25
            volatile uint32_t ertr            : 1; //bit26
            volatile uint32_t eid0            : 1; //bit27
            volatile uint32_t eid1            : 1; //bit28
            volatile uint32_t eid2            : 1; //bit29
            volatile uint32_t eid3            : 1; //bit30
            volatile uint32_t eid4            : 1; //bit31    
    };
}CAN_FILTER_TypeDef;


typedef volatile struct
{
    union 
    {
        volatile uint32_t m1;
        struct {
                volatile uint32_t dlc           : 4; //bit00:03¡£xuan¸Ä³ÉÓÐ·ûºÅ uint32_t
                         uint32_t               : 1; //bit04
                         uint32_t               : 1; //bit05
                volatile uint32_t rtr           : 1; //bit06
                volatile uint32_t ff            : 1; //bit07
                volatile uint32_t id03to10      : 8; //bit08:15
                         uint32_t               : 1; //bit16
                         uint32_t               : 1; //bit17
                         uint32_t               : 1; //bit18
                         uint32_t               : 1; //bit19
                         uint32_t               : 1; //bit20
                volatile uint32_t id00to02      : 3; //bit21:23
                volatile uint32_t data1         : 8; //bit24:31                                
        };        
    };
    union 
    {
        volatile uint32_t m2;
        struct {
                volatile uint32_t data2         : 8; //bit0:7
                volatile uint32_t data3         : 8; //bit8:15
                volatile uint32_t data4         : 8; //bit16:23
                volatile uint32_t data5         : 8; //bit24:31            
        };        
    };
    union 
    {
        volatile uint32_t m3;
        struct {
                volatile uint32_t data6         : 8; //bit0:7
                volatile uint32_t data7         : 8; //bit8:15
                volatile uint32_t data8         : 8; //bit16:23
                         uint32_t               : 8; //bit24:31                                
        };        
    };
}Struct_Message_s;

typedef volatile struct
{
    union 
    {
        volatile uint32_t m1;
        struct {
                volatile uint32_t dlc           : 4; //bit00:03
                         uint32_t               : 1; //bit04
                         uint32_t               : 1; //bit05
                volatile uint32_t rtr           : 1; //bit06
                volatile uint32_t ff            : 1; //bit07
                volatile uint32_t id21to28      : 8; //bit08:15
                volatile uint32_t id13to20      : 8; //bit16:23
                volatile uint32_t id05to12      : 8; //bit24:31                                        
        };        
    };
    union 
    {
        volatile uint32_t m2;
        struct {
                         uint32_t               : 1; //bit00
                         uint32_t               : 1; //bit01
                         uint32_t               : 1; //bit02
                volatile uint32_t id00to04      : 5; //bit03:07
                volatile uint32_t data1         : 8; //bit08:15
                volatile uint32_t data2         : 8; //bit16:23
                volatile uint32_t data3         : 8; //bit24:31                                
        };        
    };
    union 
    {
        volatile uint32_t m3;
        struct {
                volatile uint32_t data4          : 8; //bit0:7
                volatile uint32_t data5          : 8; //bit8:15
                volatile uint32_t data6          : 8; //bit16:23
                volatile uint32_t data7          : 8; //bit24:31                                    
        };        
    };
    union 
    {
        volatile uint32_t m4;
        struct {
                volatile uint32_t data8          : 8; //bit0:7
                         uint32_t                : 8; //bit8:15
                         uint32_t                : 8; //bit16:23
                         uint32_t                : 8; //bit24:31                                    
        };            
    };
}Struct_Message_e;

typedef struct
{
    volatile CAN_FILTER_TypeDef can_amr;    
    volatile CAN_FILTER_TypeDef can_acr;
    
    Struct_Message_s s_receive;
    Struct_Message_s s_transmit;
    Struct_Message_e e_receive;
    Struct_Message_e e_transmit;
    
} Struct_CAN;
#endif


extern void config_can(void);
extern void can_io_config(CAN_IO_TYPE can);
extern void can_baud(CAN_BAUD_TYPE baud);
extern void can_transmit( uint16_t id, uint8_t * data, uint8_t dataLen);
extern CommStatus can_message_process(void);
extern void CAN_Receive(void);  //add by:owz


#endif  //#ifndef CAN_GUI_H
