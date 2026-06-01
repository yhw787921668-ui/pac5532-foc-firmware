
/**
 * @file uart_gui.h
 * @brief Use for UART parameter write, checksum calculation, message processing
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

#ifndef UART_GUI_H
#define UART_GUI_H

#include "maths.h"
#include "comm_struct.h"

#define UART_MESSAGE_SIZE   9
#define UART_CHKSUM_BYTENUM (UART_MESSAGE_SIZE - 1)

extern uint8_t uart_rx_buffer[UART_MESSAGE_SIZE];
extern uint8_t uart_tx_buffer[UART_MESSAGE_SIZE];

extern void config_uart(void);
extern CommStatus uart_message_process(void);

#endif  //#ifndef UART_GUI_H
