
/**
 * @file can_gui.c
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
 
#include "motor_struct.h"
#include "can_gui.h"
#include "pac5xxx_driver_can.h"
#include "comm_funcs.h"


#if (COMMSEL_CAN == TRUE)

uint8_t can_rx_buffer[CAN_MESSAGE_SIZE];
uint8_t can_tx_buffer[CAN_MESSAGE_SIZE];

void can_baud(CAN_BAUD_TYPE baud)
{
    // CAN bit rate = PCLK/(2x(BRP+1)x(TSEG1+TSEG2+3).
    // For example: 150/(2x(19+1)x(10+2+3)) = 0.250 Mbps
    switch (baud)
    {
    case CAN_BAUD_50KHz:
        PAC55XX_CAN->BTR0.BRP = 59;
        PAC55XX_CAN->BTR1.TSEG1 = 15;
        PAC55XX_CAN->BTR1.TSEG2 = 7;
        break;

    case CAN_BAUD_100KHz:
        PAC55XX_CAN->BTR0.BRP = 49;
        PAC55XX_CAN->BTR1.TSEG1 = 10;
        PAC55XX_CAN->BTR1.TSEG2 = 2;
        break;

    case CAN_BAUD_125KHz:
        PAC55XX_CAN->BTR0.BRP = 39;
        PAC55XX_CAN->BTR1.TSEG1 = 10;
        PAC55XX_CAN->BTR1.TSEG2 = 2;
        break;

    case CAN_BAUD_200KHz:
        PAC55XX_CAN->BTR0.BRP = 24;
        PAC55XX_CAN->BTR1.TSEG1 = 10;
        PAC55XX_CAN->BTR1.TSEG2 = 2;
        break;

    case CAN_BAUD_250KHz:
        PAC55XX_CAN->BTR0.BRP = 19;
        PAC55XX_CAN->BTR1.TSEG1 = 10;
        PAC55XX_CAN->BTR1.TSEG2 = 2;
        break;

    case CAN_BAUD_400KHz:
        PAC55XX_CAN->BTR0.BRP = 11;
        PAC55XX_CAN->BTR1.TSEG1 = 10;
        PAC55XX_CAN->BTR1.TSEG2 = 2;
        break;

    case CAN_BAUD_500KHz:
        PAC55XX_CAN->BTR0.BRP = 18;
        PAC55XX_CAN->BTR1.TSEG1 = 3;
        PAC55XX_CAN->BTR1.TSEG2 = 2;
        break;

    case CAN_BAUD_800KHz:
        PAC55XX_CAN->BTR0.BRP = 11;
        PAC55XX_CAN->BTR1.TSEG1 = 3;
        PAC55XX_CAN->BTR1.TSEG2 = 2;
        break;

    case CAN_BAUD_1000KHz:
        PAC55XX_CAN->BTR0.BRP = 14;
        PAC55XX_CAN->BTR1.TSEG1 = 1;
        PAC55XX_CAN->BTR1.TSEG2 = 1;
        break;

    default:
        PAC55XX_CAN->BTR0.BRP = 14;
        PAC55XX_CAN->BTR1.TSEG1 = 1;
        PAC55XX_CAN->BTR1.TSEG2 = 1;
        break;
    }
}

void can_io_config(CAN_IO_TYPE can)
{
    switch (can)
    {
    case CAN_PC01:
        // Select CAN peripheral on PC0 and PC1
        PAC55XX_GPIOC->MODE.P0 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOC->MODE.P1 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PCMUXSEL.P0 = 6;                   //CANRXD
        PAC55XX_SCC->PCMUXSEL.P1 = 6;                   //CANTXD
        break;

    case CAN_PC45:
        // Select CAN peripheral on PC4 and PC5
        PAC55XX_GPIOC->MODE.P4 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOC->MODE.P5 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PCMUXSEL.P4 = 6;                   //CANRXD
        PAC55XX_SCC->PCMUXSEL.P5 = 6;                   //CANTXD
        break;

    case CAN_PD01:
        // Select CAN peripheral on PD0 and PD1
        PAC55XX_GPIOD->MODE.P1 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOD->MODE.P0 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PDMUXSEL.P1 = 6;                   //CANRXD
        PAC55XX_SCC->PDMUXSEL.P0 = 6;                   //CANTXD
        break;

    case CAN_PD56:
        // Select CAN peripheral on PD5 and PD6
        PAC55XX_GPIOD->MODE.P5 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOD->MODE.P6 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PDMUXSEL.P5 = 6;                   //CANRXD
        PAC55XX_SCC->PDMUXSEL.P6 = 6;                   //CANTXD
        break;

    case CAN_PD67:
        // Select CAN peripheral on PD6 and PD7
        PAC55XX_GPIOD->MODE.P7 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOD->MODE.P6 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PDMUXSEL.P7 = 6;                   //CANRXD
        PAC55XX_SCC->PDMUXSEL.P6 = 6;                   //CANTXD
        break;

    case CAN_PE23:
        // Select CAN peripheral on PE2 and PE3
        PAC55XX_GPIOE->MODE.P2 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOE->MODE.P3 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PEMUXSEL.P2 = 6;                   //CANRXD
        PAC55XX_SCC->PEMUXSEL.P3 = 6;                   //CANTXD
        break;

    case CAN_PE67:
        // Select CAN peripheral on PE6 and PE7
        PAC55XX_GPIOE->MODE.P6 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOE->MODE.P7 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PEMUXSEL.P6 = 6;                   //CANRXD
        PAC55XX_SCC->PEMUXSEL.P7 = 6;                   //CANTXD
        break;

    case CAN_PF67:
        // Select CAN peripheral on PF6 and PF7
        PAC55XX_GPIOF->MODE.P6 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOF->MODE.P7 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PFMUXSEL.P6 = 6;                   //CANRXD
        PAC55XX_SCC->PFMUXSEL.P7 = 6;                   //CANTXD
        break;

    case CAN_PG56:
        // Select CAN peripheral on PG5 and PG6
        PAC55XX_GPIOG->MODE.P5 = 3;                     //RXD High-Impedance Input
        PAC55XX_GPIOG->MODE.P6 = 1;                     //TXD Pull-Pull Output
        PAC55XX_SCC->PGMUXSEL.P5 = 6;                   //CANRXD
        PAC55XX_SCC->PGMUXSEL.P6 = 6;                   //CANTXD
        break;
    }
}

/**
 * @brief This function use for uart message processing
 */
CommStatus can_message_process(void)
{
    Struct_Motor* motor_ptr = &motor;

    motor_ptr->comm.status = success;

    // Full message received in can_rx_buffer, process and then queue up transmit (response) message
    motor_ptr->comm.cmd_code = (CommandCode) can_rx_buffer[1];
    motor_ptr->comm.param_code = (ParameterCode) can_rx_buffer[2];

    // Put data from can_rx_buffer into word for processing (in case read)
    motor_ptr->comm.rx_data = (can_rx_buffer[4] << 24);
    motor_ptr->comm.rx_data |= (can_rx_buffer[5] << 16);
    motor_ptr->comm.rx_data |= (can_rx_buffer[6] << 8);
    motor_ptr->comm.rx_data |= can_rx_buffer[7];

    comm_cmd(motor_ptr);
    can_tx_buffer[0] = 0xEE;                                           // Response message type is always 0xEE
    can_tx_buffer[1] = motor_ptr->comm.cmd_code + 0x80;                       // Response message code is received message code + 0x80
    can_tx_buffer[2] = motor_ptr->comm.status;
    can_tx_buffer[4] = ((motor_ptr->comm.tx_data >> 24) & 0xFF);
    can_tx_buffer[5] = ((motor_ptr->comm.tx_data >> 16) & 0xFF);
    can_tx_buffer[6] = ((motor_ptr->comm.tx_data >> 8)  & 0xFF);
    can_tx_buffer[7] = (motor_ptr->comm.tx_data & 0xFF);

    return motor_ptr->comm.status;
}

#endif  //#if (COMMSEL_UART == TRUE)
