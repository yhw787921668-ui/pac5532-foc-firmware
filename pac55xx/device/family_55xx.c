
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
#include "pac_init.h"
#include "driver_funcs.h"
#include "param_calc.h"
#include "sim_funcs.h"
#include "param_funcs.h"
#include "comm.h"
#include "fault.h"
#include "pac5xxx_driver_adc.h"
#include "pac5xxx_driver_timer.h"
#include "pac5xxx_driver_socbridge.h"
#include "pac5xxx_driver_tile.h"
#include "pac5xxx_driver_system.h"
#include "pac5xxx_driver_memory.h"
#include "pac5xxx_driver_uart.h"
#include "pac5xxx_driver_gpio.h"
#include "pac5xxx_driver_can.h"
#include "debug_dac.h"
#include "VSM_control.h"      // add  by:owz

// by:owz
extern int Debug_Test_Number1;  
extern int Debug_Test_Number2;
extern int Debug_Test_Number3;
#if (COMMSEL_CAN == TRUE)
Struct_CAN User_CAN;
extern uint32_t rx_dataa[8];
extern uint16_t rx_id_s;
extern uint32_t rx_id_e;
extern uint8_t id_frame_format;
extern uint8_t rx_dataLen;
extern uint16_t rx_id;
#endif

#ifdef ENABLE_DEBUG_PWMDAC
#include "debug_dac.h"
#endif

#if (ENABLE_WATCHDOG == TRUE)
#include "watchdog.h"
#endif

#if (ENABLE_STACK_CHECK == TRUE)
#include "stack_check.h"
#endif

// nIRQ1 inputs from CAFE: PA7 (fault interrupt)
#define NIRQ1_PIN                           7                                                           // nIRQ1 pin bit number
#define NIRQ1_PIN_MASK                      (1 << NIRQ1_PIN)                                            // nIRQ1 pin bit mask

void init_system()
{
    // Set Flash Lock to allow write access to MEMCTL register for configuring clocks
    PAC55XX_MEMCTL->FLASHLOCK = FLASH_LOCK_ALLOW_WRITE_MEMCTL;

    // Set safe clock defaults
    PAC55XX_SCC->CCSCTL.FRCLKMUXSEL = CCSCTL_CLKIN_ROSC;
    PAC55XX_SCC->CCSCTL.SCLKMUXSEL = CCSCTL_SCLK_FRCLK;
    PAC55XX_MEMCTL->MEMCTL.MCLKSEL = MEMCTL_MCLK_ROSCCLK;

    // Turn on Flache Cache
    PAC55XX_MEMCTL->MEMCTL.CACHEDIS = 0;                        // enable Flash CACHE

    // Select 4 MHz CLKREF for Free Running Clock FRCLK
    PAC55XX_SCC->CCSCTL.FRCLKMUXSEL = CCSCTL_CLKIN_CLKREF;
 
    pac5xxx_sys_pll_config_enable(4, 300, 0);                   // PLLCLK = 300 MHz = (4/4 * 300) /1

    // Configure SCLK=PLLCLK=300 MHz, HCLK=150 MHz, PCLK=150 MHz, ACLK=50 MHz and WaitStates;  Use default PCLKDIV=1
    PAC55XX_SCC->CCSCTL.HCLKDIV = CCSCTL_HCLKDIV_DIV2;          // HCLK = 150 MHz = SCLK/2; when SCLK = PLLCLK
    PAC55XX_SCC->CCSCTL.ACLKDIV = CCSCTL_ACLKDIV_DIV6;          // ACLK =  50 MHz = SCLK/6; when SCLK = PLLCLK
    PAC55XX_MEMCTL->MEMCTL.WSTATE = 5 + 1;                      // Flash = 150/25 = 6 clocks = 5 WS; So, need 5 + 1 Extra WS
    PAC55XX_SCC->CCSCTL.SCLKMUXSEL = CCSCTL_SCLK_PLLCLK;        // SCLK = PLLCLK

    // Set MCLK for Flash write & erase in addition to read
    PAC55XX_MEMCTL->MEMCTL.MCLKDIV = MEMCTL_MCLK_DIV5;          // MCLK will = HCLK/5 when MCLKSEL = MEMCTL_MCLK_HCLKDIV
    PAC55XX_MEMCTL->MEMCTL.MCLKSEL = MEMCTL_MCLK_HCLKDIV;       // MCLK = HCLK/5 = 30 MHz; allows reading and writing of Flash

    PAC55XX_MEMCTL->FLASHLOCK = 0;                              // Disallow write access to MEMCTL

    // Enable GPIO Input clock synchronization; should always be enabled, especially when using GPIO edge based interrupts
    PAC55XX_GPIOA->CLKSYNC.w = 0xFF;
    PAC55XX_GPIOB->CLKSYNC.w = 0xFF;
    PAC55XX_GPIOC->CLKSYNC.w = 0xFF;
    PAC55XX_GPIOD->CLKSYNC.w = 0xFF;
    PAC55XX_GPIOE->CLKSYNC.w = 0xFF;
    PAC55XX_GPIOF->CLKSYNC.w = 0xFF;
    PAC55XX_GPIOG->CLKSYNC.w = 0xFF;

#ifdef ENABLE_FLASH_CRC_TEST
    // Set USARTC to UART mode for scratch pad
    PAC55XX_SCC->CCSCTL.USCMODE = 1;                            // UART mode
#endif
}

void configure_cafe_fault_interrupt(void)
{    
    // Configure nIRQ1, CAFE fault interrupt, as digital input - active low
    PAC55XX_SCC->PAMUXSEL.w &= 0x0FFFFFFF;      // PA7 as input for n1RQ1, over current interrupt 

//    PAC55XX_GPIOA->CLKSYNC.P7 = 1;

    pac5xxx_gpio_configure_interrupt_a(NIRQ1_PIN_MASK, 0);
    #ifdef ENABLE_CBC_CURRENT_LIMIT
        NVIC_SetPriority(GpioA_IRQn,0);
    #else
        NVIC_SetPriority(GpioA_IRQn,3);
    #endif
}

void configure_pwm_dac (void)
{
#ifdef ENABLE_DEBUG_PWMDAC 
    pac5xxx_timer_clock_config(FOC_DAC_Timer, TXCTL_CS_ACLK, TXCTL_PS_DIV1);                  // Configure timer clock input for ACLK, /1 divider
    pac5xxx_timer_base_config(FOC_DAC_Timer, PWMDAC_PERIOD_TICKS, 0, TxCTL_MODE_UPDOWN, 0);   // Enable base timer

    switch (FOC_DAC_Timer)
    {
        case TimerB:
            pac5xxx_timer_b_ccctr0_value_set(PWMDAC_PERIOD_TICKS >> 1);
            pac5xxx_timer_b_ccctr1_value_set(PWMDAC_PERIOD_TICKS >> 1);
            pac5xxx_timer_b_ccctr2_value_set(PWMDAC_PERIOD_TICKS >> 1);
            break;
        case TimerC:
            pac5xxx_timer_c_ccctr4_value_set(PWMDAC_PERIOD_TICKS >> 1);
            pac5xxx_timer_c_ccctr5_value_set(PWMDAC_PERIOD_TICKS >> 1);
            pac5xxx_timer_c_ccctr6_value_set(PWMDAC_PERIOD_TICKS >> 1);
            break;
        default:
            break;
    }
#endif
}

void configure_adc_emux_engine(void)
{
    // EMUX configuration
    pac5xxx_adc_config_emux_io();

    pac5xxx_adc_emux_config(ADCEMUXCTL_DTSE_SEQ, ADCEMUXCTL_EMUXDIV_DIV16);
    pac5xxx_adc_config(ADCCTL_MODE_DTSE, ADCCTL_CLKDIV_DIV8, 0);
}

void config_systick_timer(void)
{
    SysTick_Config(SYSTICK_TICKS);
    NVIC_SetPriority(SysTick_IRQn, 2);
    SysTick->VAL = SYSTICK_TICKS;
}

#if (COMMSEL_UART == TRUE)

static int uart_rx_byte_count = 0;
static int uart_tx_byte_count = 0;

void USARTC_IRQHandler(void) 
{
    uint8_t int_type;
    uint8_t data;
    volatile int32_t var;

    for (var = 0; var < 6; ++var)
    {
        __NOP();
    }

    int_type = pac5xxx_uart_interrupt_identification2(PAC55XX_UARTC);

    if (int_type == UARTIIR_INTID_TX_HOLD_EMPTY)
    {
        pac5xxx_uart_write2(PAC55XX_UARTC, uart_tx_buffer[uart_tx_byte_count++]);

        if (uart_tx_byte_count == UART_MESSAGE_SIZE)                // Last byte of message to transmit
        {
            pac5xxx_uart_int_enable_THREI2(PAC55XX_UARTC, 0);        // Disable transmit interrupt
            pac5xxx_uart_int_enable_RDAI2(PAC55XX_UARTC, 1);         // Enable receive data interrupt for next incoming message
            uart_tx_byte_count = 0;
        }
    }
    else
    {
        data = pac5xxx_uart_read2(PAC55XX_UARTC);

        if (uart_rx_byte_count == 0)                                // For the first byte received, the start byte must be 0x89
        {
            if (data != 0x89)
                return;
        }

        uart_rx_buffer[uart_rx_byte_count++] = data;                // Store data in buffer and increment index

        if (uart_rx_byte_count == UART_MESSAGE_SIZE)                // Received all bytes of message
        {
            uart_message_process();                                 // Process incoming message in uart_rx_buffer and put response in uart_tx_buffer
            uart_rx_byte_count = 0;
            uart_tx_byte_count = 0;

            pac5xxx_uart_int_enable_RDAI2(PAC55XX_UARTC, 0);          // Disable receive data interrupt  
            pac5xxx_uart_rx_fifo_reset2(PAC55XX_UARTC);               // Reset RX FIFO, to clear RDAI interrupt
            pac5xxx_uart_int_enable_THREI2(PAC55XX_UARTC, 1);         // Enable transmit intrerrupt to send reponse to host                        
        }
    }
}


void config_uart(void)
{
    // Configure UART C (Depends on pclk input, Expects 150MHz PCLK)
    // 轮询模式：PE2=RX, PE3=TX, 115200 8N1
    PAC55XX_SCC->CCSCTL.USCMODE = 1;                                // UART mode (default 0=SPI)
    pac5xxx_uart_config_LCR2(PAC55XX_UARTC, 
                             UARTLCR_WL_BPC_8,
                             UART_STOP_BITS_1,
                             UART_PEN_DISABLE,
                             UART_PARITY_FORCE_STICK_1,
                             UART_BRKCTL_DISABLE);
    pac5xxx_uart_config_divisor_latch2(PAC55XX_UARTC, 81);           // 115kbps = 150MHz / (16 * 81)
    pac5xxx_uart_fifo_enable2(PAC55XX_UARTC, 1);
    pac5xxx_uart_rx_fifo_reset2(PAC55XX_UARTC);
    pac5xxx_uart_rx_fifo_threshold2(PAC55XX_UARTC, UARTFCR_TL_1B);
    pac5xxx_uart_tx_fifo_threshold2(PAC55XX_UARTC, UARTFCR_TL_1B);
    // 轮询模式：保持RDAI=1（否则IIR.INTID不更新），关THREI，不使能NVIC中断
    pac5xxx_uart_int_enable_RDAI2(PAC55XX_UARTC, 1); 
    pac5xxx_uart_int_enable_THREI2(PAC55XX_UARTC, 0);
    pac5xxx_uart_io_config2();
    NVIC_ClearPendingIRQ(USARTC_IRQn);    
    NVIC_DisableIRQ(USARTC_IRQn);     // 轮询模式：不使能NVIC中断
}

#endif  //#if (COMMSEL_UART == TRUE)

#if (COMMSEL_CAN == TRUE)
/**
 * @brief can_transmit, can transmit function
 */
void can_transmit(uint16_t id, uint8_t *data, uint8_t dataLen)
{
	while(PAC55XX_CAN->SR.TBS == 0);	// wait for TX buffer free
	PAC55XX_CAN->TXBUF =  	(dataLen << 0)		|		// DLC - Data Length Code
							(0 << 6) 			|       // RTR = 0 Data Frame
							(0 << 7) 			|       // FF - Format Frame; 0=Std Frame
							((id>>3) << 8) 		|		// ID 10:3
							((id&0x07) << 21)	|     	// ID 2:0
							(data[0] << 24);     		// Data 0

	if(dataLen > 1)
	{
		PAC55XX_CAN->TXBUF =  	(data[1] << 0) |		// Data 1
								(data[2] << 8) |		// Data 2
								(data[3] << 16)|		// Data 3
								(data[4] << 24);		// Data 4
	}
	if(dataLen > 5)
	{
		PAC55XX_CAN->TXBUF =  	(data[5] << 0) |		// Data 5
								(data[6] << 8) |		// Data 6
								(data[7] << 16);		// Data 7
		
	}
	
	PAC55XX_CAN->CMR.TR = 1;	// Request transmit
}

/**
 * @brief CAN_Receive, by:owz
 */
void CAN_Receive(void)
{   
	Struct_Motor* motor_ptr = &motor;
    uint32_t bufferr;
    static Struct_CAN* can_ptr = &User_CAN;
    bufferr = PAC55XX_CAN->RXBUF;    //  read RX buffer, RX buffer bit order same as TX buffer 

    if(bufferr&0x80)    // ��չ֡��Extended Frame
    {
        id_frame_format = 1;
        can_ptr->e_receive.m1 = bufferr;
        bufferr = PAC55XX_CAN->RXBUF;
        can_ptr->e_receive.m2 = bufferr;        
        rx_dataLen = can_ptr->e_receive.dlc;
        rx_id_e = (can_ptr->e_receive.id00to04) + (can_ptr->e_receive.id05to12<<5) + (can_ptr->e_receive.id13to20<<13)  + (can_ptr->e_receive.id21to28<<21); 

        rx_dataa[0] = can_ptr->e_receive.data1;//uint32_t
        rx_dataa[1] = can_ptr->e_receive.data2;
        rx_dataa[2] = can_ptr->e_receive.data3;    
        if(rx_dataLen > 3)
        {
            bufferr = PAC55XX_CAN->RXBUF;
            can_ptr->e_receive.m3 = bufferr;    
            rx_dataa[3] = can_ptr->e_receive.data4;    
            rx_dataa[4] = can_ptr->e_receive.data5;    
            rx_dataa[5] = can_ptr->e_receive.data6;    
            rx_dataa[6] = can_ptr->e_receive.data7;    
            if(rx_dataLen > 7)
            {
                bufferr = PAC55XX_CAN->RXBUF;
                can_ptr->e_receive.m4 = bufferr;                
                rx_dataa[7] = can_ptr->e_receive.data8;
            }                
        }
    }
    else  // Standard Frame����׼֡
    {
        id_frame_format = 0;
        
        can_ptr->s_receive.m1 = bufferr;//��1��32λ
        
        rx_dataLen = can_ptr->s_receive.dlc;//���ݳ��ȣ�����DATA�����ֽ�
        rx_id_s = (can_ptr->s_receive.id00to02) + (can_ptr->s_receive.id03to10<<3);  //ID   
        
        rx_dataa[0] = can_ptr->s_receive.data1;   // DATA1     
        if(rx_dataLen > 1)
        {
            bufferr = PAC55XX_CAN->RXBUF;//��2��32λ
            can_ptr->s_receive.m2 = bufferr;    
            rx_dataa[1] = can_ptr->s_receive.data2;// DATA2 
            rx_dataa[2] = can_ptr->s_receive.data3;// DATA3 
            rx_dataa[3] = can_ptr->s_receive.data4;// DATA4    
            rx_dataa[4] = can_ptr->s_receive.data5;// DATA5 
            if(rx_dataLen > 5)
            {
                bufferr = PAC55XX_CAN->RXBUF;//��3��32λ
                can_ptr->s_receive.m3 = bufferr;                        
                rx_dataa[5] = can_ptr->s_receive.data6;// DATA6 
                rx_dataa[6] = can_ptr->s_receive.data7;// DATA7 
                rx_dataa[7] = can_ptr->s_receive.data8;// DATA8 
            }
        } 
       if(rx_id_s==0xA1)
       {
			   run_command.Assist_Rate = rx_dataa[0] & 0x0F;
			 }
	 
		}
}

/**
 * @brief CAN_IRQHandler, CAN IRQ interrupt handler
 */
// ע�͵� by:owz
#if 1
void CAN_IRQHandler(void)
{
//	uint16_t rx_id; 
	uint32_t buffer;
	uint8_t rx_datalen = 0;
	buffer = PAC55XX_CAN->RXBUF;	//  read RX buffer, RX buffer bit order same as TX buffer 
	
	rx_datalen = buffer&0x0F;
//	rx_id = ((buffer&0xE00000)>>21) | ((buffer&0xFF00)>>5); 
	can_rx_buffer[0] = buffer>>24;	// data0

    if(rx_datalen > 1)
    {
        buffer = PAC55XX_CAN->RXBUF;	// buffer contains data1..data4
        can_rx_buffer[1] = buffer;
        can_rx_buffer[2] = buffer>>8;
        can_rx_buffer[3] = buffer>>16;
        can_rx_buffer[4] = buffer>>24;
        if(rx_datalen > 5)
        {
            buffer = PAC55XX_CAN->RXBUF;	//  buffer contains data7..data5
            can_rx_buffer[5] = buffer;
            can_rx_buffer[6] = buffer>>8;
            can_rx_buffer[7] = buffer>>16;
        }
    }

	if((rx_datalen == CAN_MESSAGE_SIZE) && (0x89 == can_rx_buffer[0]))
	{
        can_message_process(); 
        can_transmit(ADAPTER_CAN_ID, &can_tx_buffer[0], CAN_MESSAGE_SIZE);
	}
	
	
	PAC55XX_CAN->ISR_SR_CMR_MR = (PAC55XX_CAN->ISR_SR_CMR_MR & 0x00FFFFFF) | ISR_RI; // Clear RX interrupt
}
#endif

/**
 * @brief config_can, configure CAN to handle GUI commands
 */
void config_can(void)
{
	static Struct_CAN* can_ptr = &User_CAN; //Add by:owz
    // Config CAN IO
    can_io_config(CAN_IO);
    
	PAC55XX_CAN->MR.RM = 1;			// CAN in reset mode, in order to configure CAN module
	PAC55XX_CAN->MR.AFM = 1;		// Single filter scheme
	
	// Config baud rate */
	can_baud(CAN_BR);

	PAC55XX_CAN->BTR0.SJW = 1;		// Synchronization jump width
	PAC55XX_CAN->BTR1.SAM = 0;		// Bus is sampled once 
	
	//Add by:owz
	    can_ptr->can_amr.w = 0xFFFFFFFF;
    can_ptr->can_amr.srtr = 1;
    can_ptr->can_amr.sid0 = 1;
    can_ptr->can_amr.sid1 = 1;
    can_ptr->can_amr.sid2 = 1;
    can_ptr->can_amr.sid3 = 1;
    can_ptr->can_amr.sid4 = 1;
    can_ptr->can_amr.sid5 = 1;
    can_ptr->can_amr.sid6 = 1;
    can_ptr->can_amr.sid7 = 1;
    can_ptr->can_amr.sid8 = 1;
    can_ptr->can_amr.sid9 = 1;
    can_ptr->can_amr.sid10 = 1;
    
    can_ptr->can_acr.w = 0x00000000;    
    can_ptr->can_acr.srtr = 0;
    can_ptr->can_acr.sid0 = 0;
    can_ptr->can_acr.sid1 = 0;
    can_ptr->can_acr.sid2 = 0;
    can_ptr->can_acr.sid3 = 0;
    can_ptr->can_acr.sid4 = 0;
    can_ptr->can_acr.sid5 = 0;
    can_ptr->can_acr.sid6 = 0;
    can_ptr->can_acr.sid7 = 0;
    can_ptr->can_acr.sid8 = 0;
    can_ptr->can_acr.sid9 = 0;
    can_ptr->can_acr.sid10 = 0;    
		
		PAC55XX_CAN->AMR = can_ptr->can_amr.w;
    PAC55XX_CAN->ACR = can_ptr->can_acr.w;

// ע�͵� by:owz
//	// Filter bit7:0 => ID10:3
//	// Filter bit15:13 => ID2:0
//	PAC55XX_CAN->AMR = CAN_ID_MASK;	// CAN ID Mask
//	PAC55XX_CAN->ACR = CAN_ID_CODE;	// CAN Acceptance code 
// ע�͵� by:owz
//    // Config Can interrupt
////	PAC55XX_CAN->IMR.TIM = 1;		// Transmit Interrupt
//	PAC55XX_CAN->IMR.RIM = 1;		// Receive Interrupt
//    NVIC_SetPriority(CAN_IRQn, 3);   // CAN Interrupt priority is 3
//	NVIC_EnableIRQ(CAN_IRQn);
	
	PAC55XX_CAN->MR.RM = 0;			// CAN reset mode inactive
}
#endif // #if (COMMSEL_CAN == TRUE) */


#if  (COMMSEL_LIN == TRUE)
/**
 * @brief UART_IRQHandler, UART IRQ interrupt handler
 */

void USARTC_IRQHandler(void)
{
   lin_handler();
}

/**
 * @brief config_lin_uart, configure LIN to handle GUI commands
 */
void config_lin(void)
{
    // Configure UART C (Depends on pclk input, Expects 150MHz PCLK)
    PAC55XX_SCC->CCSCTL.USCMODE = 1;                                // UART mode
    pac5xxx_uart_config_LCR2(PAC55XX_UARTC, 
                             UARTLCR_WL_BPC_8,
                             UART_STOP_BITS_1,
                             UART_PEN_DISABLE,
                             UART_PARITY_ODD,
                             UART_BRKCTL_DISABLE);
    pac5xxx_uart_config_divisor_latch2(PAC55XX_UARTC, LIN_DIVISOR);           // DLR = PCLK / (16 * LIN_BAUD_RATE)
    pac5xxx_uart_int_enable_RDAI2(PAC55XX_UARTC, 1); 
	
    pac5xxx_uart_io_config2();
    NVIC_ClearPendingIRQ(USARTC_IRQn);    
    NVIC_SetPriority(USARTC_IRQn,3);
    NVIC_EnableIRQ(USARTC_IRQn);	
}

void config_lin_timer(void)
{
    // Configure Timer C Controls
    pac5xxx_timer_clock_config(TimerC, TXCTL_CS_ACLK, TXCTL_PS_DIV1);                        // Configure timer clock input for ACLK, /1 divider
    pac5xxx_timer_base_config(TimerC, 49999, 0, TxCTL_MODE_UP, 0);   						 // Enable base timer
    // Update duty cycle when TACTR counts from 1 to 0 
    PAC55XX_TIMERC->CTL.PRDLATCH = 0;
    
		PAC55XX_TIMERC->CTL.CLR = 1;
    PAC55XX_TIMERC->CTL.BASEIE = 1;     // Enable timer base interrupt
	
		
		// Configure PC6 as falling edge capture pin for bit time measurement
		PAC55XX_GPIOC->MODE.P6 = 3;		  // PC6 as input
		PAC55XX_SCC->PCMUXSEL.P6 = 2;	  // PWMC6
		PAC55XX_TIMERC->CCTL6.w	=(1<<0) | // MODE=CAPTURE
							 (1<<1) | // CCINTEN=ENABLE
							 (3<<2) | // EDGE=RESERVED
							 (0<<4) | // Not Invert
							 (1<<5) | // Latch CTR on falling edge
							 (0<<7) | // FORCE=NOT EFFECT
							 (0<<8);  // DON'T SKIP CCR MATCHES BEFORE INTERRUPT
		
		NVIC_ClearPendingIRQ(TimerC_IRQn);
		NVIC_SetPriority(TimerC_IRQn,3);
    NVIC_EnableIRQ(TimerC_IRQn);

}
#endif
void uart_scratch_write(int32_t value)
{
    // Set the UART Scratch Pad Register value
    PAC55XX_UARTC->SCR.PAD = value;
}

int32_t uart_scratch_read(void)
{
    // Return the value stored in the UART Scratch Pad Register
    return PAC55XX_UARTC->SCR.PAD;
}

//#if defined(ENABLE_SIM) && (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
//void sim_update_seq_trigger_point (int32_t value)
//{
//    switch (FOC_PWM_Timer)
//    {
//        case TIMER_A:
//            pac5xxx_timer_a_ccctr0_value_set(value);    // There seems to be an issue with the sequencer trigger if the trigger is allowed to be <4
//            break;
//        case TIMER_B:
//            pac5xxx_timer_b_ccctr0_value_set(value);    // There seems to be an issue with the sequencer trigger if the trigger is allowed to be <4
//            break;
//        default:
//            break;
//    }
//}
//#endif

/**
 * @brief  This function starts an ADC Conversion and Sequencer
 *
 * @return none
 */
void pac5xxx_start_adc_sequencer(void)
{
    pac5xxx_adc_enable(1);
}


void deconfig_seq(void)
{
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG0EDGE = 0;       // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG1EDGE = 0;       // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG2EDGE = 0;       // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT0TO3.TRIG3EDGE = 0;       // Trigger falling edge

    PAC55XX_ADC->DTSETRIGENT4TO7.TRIG4EDGE = 0;       // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT4TO7.TRIG5EDGE = 0;       // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT4TO7.TRIG6EDGE = 0;       // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT4TO7.TRIG7EDGE = 0;       // Trigger falling edge

    PAC55XX_ADC->DTSETRIGENT8TO11.TRIG8EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT8TO11.TRIG9EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT8TO11.TRIG10EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT8TO11.TRIG11EDGE = 0;         // Trigger falling edge

    PAC55XX_ADC->DTSETRIGENT12TO15.TRIG12EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT12TO15.TRIG13EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT12TO15.TRIG14EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT12TO15.TRIG15EDGE = 0;         // Trigger falling edge

    PAC55XX_ADC->DTSETRIGENT16TO19.TRIG16EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT16TO19.TRIG17EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT16TO19.TRIG18EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT16TO19.TRIG19EDGE = 0;         // Trigger falling edge

    PAC55XX_ADC->DTSETRIGENT20TO23.TRIG20EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT20TO23.TRIG21EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT20TO23.TRIG22EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT20TO23.TRIG23EDGE = 0;         // Trigger falling edge

    PAC55XX_ADC->DTSETRIGENT24TO27.TRIG24EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT24TO27.TRIG25EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT24TO27.TRIG26EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT24TO27.TRIG27EDGE = 0;         // Trigger falling edge

    PAC55XX_ADC->DTSETRIGENT28TO31.TRIG28EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT28TO31.TRIG29EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT28TO31.TRIG30EDGE = 0;         // Trigger falling edge
    PAC55XX_ADC->DTSETRIGENT28TO31.TRIG31EDGE = 0;         // Trigger falling edge
}
