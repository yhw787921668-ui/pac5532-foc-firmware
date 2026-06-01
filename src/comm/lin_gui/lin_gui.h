/**
 * @file lin_gui.h
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
#ifndef LIN_GUI_H
#include "maths.h"
#include "comm_struct.h"
#define LIN_GUI_H

#define LIN_TABLE_SIZE_MAX 		16		// maximum number of ID table entries
#define LIN_FRAME_TIMEOUT			50		// milliseconds
#define LIN_DATA_LENGTH				8
#define VER13									0
#define VER20									1
#define LIN_VERSION				VER20
#define NULL2									0


typedef enum {
	start = 0,
	stop
} timer_state;

typedef enum {
	off = 0,
	on
} tx_state;

typedef enum {
	falling = 0,
	rising,
	rising_and_falling,
	disable
} timer_capture;

typedef enum {
	lin_slave_break = 0,
	lin_slave_sync,
	lin_slave_pid,
	lin_slave_rx_data,
	lin_slave_rx_cksum,
	lin_slave_tx_data,
	lin_slave_tx_cksum,
	lin_slave_trans_end
} lin_slave_state;

typedef enum {
	capture_edge_1 = 0, capture_edge_2, capture_done
} capture_state;

typedef enum {
	response_ignore = 0, response_send, response_read
} response_type;

typedef struct {
	uint16_t edge1_cap_val;
	uint16_t edge2_cap_val;
	uint16_t edge1_ms;
	uint16_t edge2_ms;
	capture_state capture;
} sync_str;

typedef struct {
	unsigned char *data;			// pointer to data to be sent/saved
	unsigned char pid;				// protected identifier
	unsigned char responseLength;	// response data length in bytes
	response_type responseType;	    // slave response behavior
} lin_id_entry;

typedef struct {
	uint8_t dataBuf[8];
	lin_id_entry* p_entry;
	uint8_t cksum;
	uint8_t dataIndex;
} frame_buffer_str;

typedef struct {
	sync_str sync;
	lin_id_entry* p_table;
	volatile lin_slave_state state;
	uint8_t currentTableSize;
	uint8_t newEntryPos;
} lin_slave_str;

static lin_id_entry* find_pid(uint8_t pid);
static void dataCopy(uint8_t* dst, uint8_t* src, uint8_t length);
static void lin_uart_clear(void);
static uint8_t lin_cksum_calc(frame_buffer_str* buffer);
static void lin_slave_state_reset(void);
void lin_table_add(uint8_t pid, response_type type, uint8_t size, uint8_t *data);
void lin_tx_buff_clear(void);

extern void config_lin(void);
extern void config_lin_timer(void);
extern void lin_handler(void);
CommStatus lin_message_process(void);
extern void lin_slave_init(void);
extern void lin_process_capture_timer_event(void);

/*static uint8_t lin_check_data_avail(void);
static void lin_tx_state(tx_state state);
static void lin_timer_state(timer_state state);
static void lin_timer_capture(timer_capture state);
static int lin_edge_capture_init(void);
static void lin_edge_capture_init_clear(void);
static uint16_t lin_edge_capture_get_value(void);
static int lin_timer_base_int(void);
static void lin_timer_base_int_clear(void);
static void lin_send (uint8_t data);
static uint8_t lin_recv (void);
static int lin_check_break(void);
static void lin_set_baud (uint32_t baud);*/

#endif

