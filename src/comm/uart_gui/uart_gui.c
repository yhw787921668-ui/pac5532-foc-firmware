
/**
 * @file uart_gui.c
 * @brief Use for UART parameter write, checksum cal, message processing
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
#include "uart_gui.h"
#include "pac5xxx_driver_uart.h"
#include "comm_funcs.h"


#if (COMMSEL_UART == TRUE)

uint8_t uart_rx_buffer[UART_MESSAGE_SIZE];
uint8_t uart_tx_buffer[UART_MESSAGE_SIZE];


/**
 * @brief This function use for uart message processing
 */
CommStatus uart_message_process(void)
{
    Struct_Motor* motor_ptr = &motor;

    motor_ptr->comm.status = success;

    // Full message received in uart_rx_buffer, process and then queue up transmit (response) message
    motor_ptr->comm.cmd_code = (CommandCode) uart_rx_buffer[1];
//    uart_tx_buffer[0] = 0xEE;                                           // Response message type is always 0xEE
//    uart_tx_buffer[1] = motor_ptr->comm.cmd_code + 0x80;                       // Response message code is received message code + 0x80

    motor_ptr->comm.param_code = (ParameterCode) uart_rx_buffer[2];

    // Put data from uart_rx_buffer into word for processing (in case read)
    motor_ptr->comm.rx_data = (uart_rx_buffer[4] << 24);
    motor_ptr->comm.rx_data |= (uart_rx_buffer[5] << 16);	
    motor_ptr->comm.rx_data |= (uart_rx_buffer[6] << 8);
    motor_ptr->comm.rx_data |= uart_rx_buffer[7];

    // Validate checksum
    if (uart_checksum((uint8_t*)&uart_rx_buffer, UART_CHKSUM_BYTENUM) != uart_rx_buffer[8])
    {
        motor_ptr->comm.status = checksum_error;
    }
    else
    {
        comm_cmd(motor_ptr);
    }

    uart_tx_buffer[0] = 0xEE;                                           // Response message type is always 0xEE
    uart_tx_buffer[1] = motor_ptr->comm.cmd_code + 0x80;                       // Response message code is received message code + 0x80
    uart_tx_buffer[2] = motor_ptr->comm.status;
    uart_tx_buffer[4] = ((motor_ptr->comm.tx_data >> 24) & 0xFF);
    uart_tx_buffer[5] = ((motor_ptr->comm.tx_data >> 16) & 0xFF);
    uart_tx_buffer[6] = ((motor_ptr->comm.tx_data >> 8)  & 0xFF);
    uart_tx_buffer[7] = (motor_ptr->comm.tx_data & 0xFF);
    uart_tx_buffer[8] = uart_checksum((uint8_t*)&uart_tx_buffer, UART_CHKSUM_BYTENUM);               // Calculate checksum for outgoing message

    return motor_ptr->comm.status;
}

#endif  //#if (COMMSEL_UART == TRUE)
