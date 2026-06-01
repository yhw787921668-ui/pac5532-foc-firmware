/**
 * @file lin_gui.c
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

#include "lin_gui.h"
#include "pac_init.h"
#include "pac5xxx_driver_uart.h"
#include "motor_struct.h"
#include "comm_funcs.h"

#if (COMMSEL_LIN == TRUE)

lin_id_entry lin_pid_table[LIN_TABLE_SIZE_MAX];
lin_slave_str lin_slave;
uint16_t lin_timer_counter;
uint8_t lin_rx_buffer[LIN_DATA_LENGTH] = {0};
uint8_t lin_tx_buffer[LIN_DATA_LENGTH] = {0};


#if defined(FAMILY_52XX)
static uint8_t lin_check_data_avail(void){	return ((UART_IID_Type)PAC5XXX_UART->IIR.IID == UARTIIR_IID_RX_DATA_AVAIL)? 1: 0;};
static void lin_tx_state(tx_state state) { PAC5XXX_GPIOE->OUTEN.P1 = state; }
static void lin_timer_state(timer_state state) { PAC5XXX_TIMERC->CTL.CLR = state; }
static void lin_timer_capture(timer_capture state) { PAC5XXX_TIMERC->CCTRL1.CCEDGE = state; }
static int lin_edge_capture_init(void) { return PAC5XXX_TIMERC->CCTRL1.CCINT; }
static void lin_edge_capture_init_clear(void) { PAC5XXX_TIMERC->CCTRL1.CCINT = 1; }
static uint16_t lin_edge_capture_get_value(void) { return PAC5XXX_TIMERC->CTR1; }
static int lin_timer_base_int(void) { return PAC5XXX_TIMERC->CTL.INT;}
static void lin_timer_base_int_clear(void) { PAC5XXX_TIMERC->CTL.INT = 1;}
static void lin_send(uint8_t data) {PAC5XXX_UART->RXTX.VAL = data;};
static uint8_t lin_recv (void) {return (uint8_t)PAC5XXX_UART->RXTX.w; };
static int lin_check_break(void) {return PAC5XXX_UART->LSR.BI;};
static void lin_set_baud (uint32_t baud) { pac5xxx_uart_rx_fifo_reset_remapped();	pac5xxx_uart_config_divisor_latch_remapped(HCLK_FREQ_HZ/(16*baud)); }; 

#elif defined(FAMILY_55XX) 
static uint8_t lin_check_data_avail(void){	return (pac5xxx_uart_interrupt_identification2(PAC55XX_UARTC) == UARTIIR_INTID_RX_DATA_AVAIL)? 1: 0;};
static void lin_tx_state(tx_state state) { PAC55XX_GPIOE->MODE.P2  = state; }
static void lin_timer_state(timer_state state) { PAC55XX_TIMERC->CTL.CLR = state; }
static void lin_timer_capture(timer_capture state) { PAC55XX_TIMERC->CCTL6.CCINTEDGE = state;}
static int lin_edge_capture_init(void) { return PAC55XX_TIMERC->INT.CCR6IF; }
static void lin_edge_capture_init_clear(void) { PAC55XX_TIMERC->INT.w = 0x00000040; }
static uint16_t lin_edge_capture_get_value(void) { return PAC55XX_TIMERC->CCTR6.CTR; }
static int lin_timer_base_int(void) { return PAC55XX_TIMERC->INT.BASEIF;}
static void lin_timer_base_int_clear(void) { PAC55XX_TIMERC->INT.w = 0x00000100;}
static void lin_send(uint8_t data) {pac5xxx_uart_write2(PAC55XX_UARTC, data);};
static uint8_t lin_recv (void) {return pac5xxx_uart_read2(PAC55XX_UARTC);};
static uint8_t lin_check_break(void) {return PAC55XX_UARTC -> LSR.BI;};
static void lin_set_baud (uint32_t baud) {	pac5xxx_uart_config_divisor_latch2(PAC55XX_UARTC, HCLK_FREQ_HZ/(16*baud)); }; 
#endif



void lin_slave_init() {

	lin_slave.state = lin_slave_break;
	lin_slave.p_table = lin_pid_table;
	lin_table_add(LIN_PID_READ, response_read, LIN_DATA_LENGTH, lin_rx_buffer);
	lin_table_add(LIN_PID_RESPONSE, response_send, LIN_DATA_LENGTH, lin_tx_buffer);
}


void lin_handler(void) {

	if (!lin_check_data_avail()) {
		return;
	}
	static frame_buffer_str buffer;
	uint8_t rx_data = lin_recv();
	static uint8_t sync_retry ;
	switch (lin_slave.state) {
	case lin_slave_break:
			if(lin_check_break())
			{
				lin_slave.state = lin_slave_sync;
				// Start timer for baud rate measurement and timeout counter
				lin_timer_counter = 0;
				lin_timer_state(start);
				// Enable falling edge capture 
				lin_slave.sync.capture = capture_edge_1;
				lin_timer_capture(falling);
				sync_retry =0;
			}

		break;
	case lin_slave_sync:
		if (rx_data == 0x55) {
			//clock_synchronize();
			lin_slave.state = lin_slave_pid;
		} else {
			if (sync_retry >0) 
			{
				lin_slave.state = lin_slave_trans_end;
			}
			else // second check sync byte 
				sync_retry++;
		}
		break;
	case lin_slave_pid:
		buffer.p_entry = find_pid(rx_data);
		if (buffer.p_entry == NULL2) {
			lin_slave.state = lin_slave_trans_end;
		} else {
			buffer.dataIndex = 0;
			if (buffer.p_entry->responseType == response_send) {
				lin_slave.state = lin_slave_tx_data;

				__disable_irq();
				dataCopy(buffer.dataBuf, buffer.p_entry->data,
						buffer.p_entry->responseLength);
				__enable_irq();
				buffer.cksum = lin_cksum_calc(&buffer);
				lin_tx_state(on);
				lin_send(buffer.dataBuf[0]);
			} 
			else // responseType == response_send || responseType == response_ignore
			{
				lin_slave.state = lin_slave_rx_data;
				lin_tx_buff_clear();
			}
		}
		break;
	case lin_slave_rx_data:
		buffer.dataBuf[buffer.dataIndex] = rx_data;
		buffer.dataIndex++;
		if (buffer.dataIndex == buffer.p_entry->responseLength) {
			lin_slave.state = lin_slave_rx_cksum;
		}
		break;
	case lin_slave_rx_cksum:
		buffer.cksum = lin_cksum_calc(&buffer);
		if (buffer.cksum == rx_data) { // checksum and header uart byte are correct
			__disable_irq();
			dataCopy(lin_rx_buffer, buffer.dataBuf,
					buffer.p_entry->responseLength);
			__enable_irq();
			lin_message_process();
		}
		lin_slave.state = lin_slave_trans_end;
		break;
	case lin_slave_tx_data:
		// Received data byte must be the same as sent data byte // because echo byte
		if (buffer.dataBuf[buffer.dataIndex] != rx_data) 
		{
			lin_slave.state = lin_slave_trans_end;
		} 
		else 
		{
			buffer.dataIndex++;
			if (buffer.dataIndex < buffer.p_entry->responseLength) {
				lin_send(buffer.dataBuf[buffer.dataIndex]);
			} else {
				lin_slave.state = lin_slave_tx_cksum;
				lin_send(buffer.cksum);
			}
		}
		break;
	case lin_slave_tx_cksum:
		lin_slave.state = lin_slave_trans_end;
		break;
	default:
		break;
	}
	if (lin_slave.state == lin_slave_trans_end)
	{
			lin_slave_state_reset();
	}
}

void lin_process_capture_timer_event(void) // period = 1ms
{
	if((lin_slave.sync.capture != capture_done) && lin_edge_capture_init())
	{
		lin_edge_capture_init_clear();
		// First falling edge captured
		if(lin_slave.sync.capture == capture_edge_1)
		{
			lin_slave.sync.edge1_cap_val = lin_edge_capture_get_value();
			lin_slave.sync.edge1_ms = lin_timer_counter;
			lin_slave.sync.capture = capture_edge_2;
		}
		else // Second faling edge captured
		{
			lin_slave.sync.edge2_cap_val = lin_edge_capture_get_value();
			lin_slave.sync.edge2_ms = lin_timer_counter;
			lin_slave.sync.capture = capture_done;
			lin_timer_capture(disable);
		}
		return;
	}
	if (lin_timer_base_int()) {
		lin_timer_base_int_clear();
		lin_timer_counter++;
		if (lin_timer_counter >= LIN_FRAME_TIMEOUT) {
			lin_slave_state_reset();
		}
	}
}

static void clock_synchronize(void) {
	sync_str* pSync = &lin_slave.sync;
	uint16_t bit_ticks;
	bit_ticks = (pSync->edge2_ms - pSync->edge1_ms) * 50000
			+ pSync->edge2_cap_val - pSync->edge1_cap_val; // bit_ticks bwt 2 edge
	lin_set_baud(bit_ticks>>1);
}

static void dataCopy(uint8_t* dst, uint8_t* src, uint8_t length) {
	uint8_t i;
	for (i = 0; i < length; i++) {
		dst[i] = src[i];
	}
}

static void lin_slave_state_reset(void) {
	lin_slave.state = lin_slave_break;
	lin_tx_state(off);
	lin_uart_clear();
	lin_timer_state(stop);
	lin_timer_capture(disable);
}

static lin_id_entry* find_pid(uint8_t pid) {
	uint8_t i;
	for (i = 0; i < lin_slave.currentTableSize; i++) {
		if (pid == lin_slave.p_table[i].pid) {
			return &lin_slave.p_table[i];
		}
	}
	return NULL2;
}

static uint8_t lin_cksum_calc(frame_buffer_str* buffer) {
	uint16_t cksum = 0;
	if (LIN_VERSION == VER20)
		cksum = buffer->p_entry->pid;
	uint8_t i;
	for (i = 0; i < buffer->p_entry->responseLength; i++) {
		cksum += buffer->dataBuf[i];
		if (cksum > 255) {
			cksum &= 0xFF;
			cksum++;
		}
	}
	cksum ^= 0xFF;
	return (uint8_t) cksum;
}

void lin_table_add_entry(lin_id_entry* p_newEntry) {
	if (lin_slave.currentTableSize < LIN_TABLE_SIZE_MAX)
		lin_slave.currentTableSize++;

	lin_id_entry *p_entry = &lin_pid_table[lin_slave.newEntryPos];

	p_entry->pid = p_newEntry->pid;
	p_entry->responseType = p_newEntry->responseType;
	p_entry->responseLength = p_newEntry->responseLength;
	p_entry->data = p_newEntry->data;
	lin_slave.newEntryPos++;

	if (lin_slave.newEntryPos >= LIN_TABLE_SIZE_MAX)
		lin_slave.newEntryPos = 0;
}

void lin_table_clear(void) {
	lin_slave.currentTableSize = 0;
	lin_slave.newEntryPos = 0;
}

void lin_table_add(uint8_t pid, response_type type, uint8_t size, uint8_t *data) {
	if (lin_slave.currentTableSize < LIN_TABLE_SIZE_MAX)
		lin_slave.currentTableSize++;

	lin_id_entry *p_entry = &lin_pid_table[lin_slave.newEntryPos];

	p_entry->pid = pid;
	p_entry->responseType = type;
	p_entry->responseLength = size;
	p_entry->data = data;
	lin_slave.newEntryPos++;

	if (lin_slave.newEntryPos >= LIN_TABLE_SIZE_MAX)
		lin_slave.newEntryPos = 0;
}

void lin_uart_clear(void) {
	lin_set_baud(LIN_BR);
}
void lin_tx_buff_clear(void){
	uint8_t i;
	for(i =0; i< LIN_DATA_LENGTH; i++)
	{
		lin_tx_buffer[i] =0; 
	}
}



#if (COMMSEL_LIN == TRUE)
/**
 * @brief This function use for uart message processing
 */
CommStatus lin_message_process(void)
{
    Struct_Motor* motor_ptr = &motor;

    motor_ptr->comm.status = success;

    // Full message received in uart_rx_buffer, process and then queue up transmit (response) message
    motor_ptr->comm.cmd_code = (CommandCode) lin_rx_buffer[1];
//    uart_tx_buffer[0] = 0xEE;                                           // Response message type is always 0xEE
//    uart_tx_buffer[1] = motor_ptr->comm.cmd_code + 0x80;                       // Response message code is received message code + 0x80

    motor_ptr->comm.param_code = (ParameterCode) lin_rx_buffer[2];

    // Put data from uart_rx_buffer into word for processing (in case read)
    motor_ptr->comm.rx_data = (lin_rx_buffer[4] << 24);
    motor_ptr->comm.rx_data |= (lin_rx_buffer[5] << 16);
    motor_ptr->comm.rx_data |= (lin_rx_buffer[6] << 8);
    motor_ptr->comm.rx_data |= lin_rx_buffer[7];

    comm_cmd(motor_ptr);

    lin_tx_buffer[0] = 0xEE;                                           // Response message type is always 0xEE
    lin_tx_buffer[1] = motor_ptr->comm.cmd_code + 0x80;                       // Response message code is received message code + 0x80
    lin_tx_buffer[2] = motor_ptr->comm.status;
    lin_tx_buffer[4] = ((motor_ptr->comm.tx_data >> 24) & 0xFF);
    lin_tx_buffer[5] = ((motor_ptr->comm.tx_data >> 16) & 0xFF);
    lin_tx_buffer[6] = ((motor_ptr->comm.tx_data >> 8)  & 0xFF);
    lin_tx_buffer[7] = (motor_ptr->comm.tx_data & 0xFF);
    return motor_ptr->comm.status;
}
#endif

#endif

