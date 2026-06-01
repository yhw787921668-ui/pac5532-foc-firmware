
/**
 * @file family_52xx.c
 * @brief Use for 52xx family hardware resources initialization
 */

//=============================================================================
// Copyright (C) 2018 - 2023, Qorvo, Inc.
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
#include "param.h"
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
#include "debug_dac.h"

#ifdef ENABLE_DEBUG_PWMDAC
#include "debug_dac.h"
#endif

#if (ENABLE_WATCHDOG == TRUE)
#include "watchdog.h"
#endif

#if (ENABLE_STACK_CHECK == TRUE)
#include "stack_check.h"
#endif

// nIRQ1 inputs from CAFE: PB0 (fault interrupt)
//#define NIRQ1_PIN                           0                                                           // nIRQ1 pin bit number
//#define NIRQ1_PIN_MASK                      (1 << NIRQ1_PIN)                                            // nIRQ1 pin bit mask

/**
 * @brief init_system, use to setup PLL, system clock and flash wait state
 */
void init_system(void)
{
    // PLL 50MHZ, FCLK: 50MHz, PLLCLK: 50MHz, ACLK: 50MHz (/1), HCLK: 50MHz (/1)
	// PLL CLK and FLASH wait state is coming from config_app.h
    pac5xxx_sys_pll_config(HCLK_FREQ_HZ/1000000);
    pac5xxx_sys_ccs_config(CCSCTL_CLKIN_CLKREF, CCSCTL_ACLKDIV_DIV1, CCSCTL_HCLKDIV_DIV1);
    pac5xxx_memctl_wait_state(FLASH_WAIT_STATE);
}

/**
 * @brief configure_cafe_fault_interrupt, configure over current interrupt from AFE
 */
void configure_cafe_fault_interrupt(void)
{    
    // Configure nIRQ1, CAFE fault interrupt, as digital input - active low
    pac5xxx_gpio_configure_interrupt_b(NIRQ1_PIN_MASK, 0);
    #ifdef ENABLE_CBC_CURRENT_LIMIT
        NVIC_SetPriority(GpioB_IRQn,0);
    #else
        NVIC_SetPriority(GpioB_IRQn,3);
    #endif
}

/**
 * @brief configure_pwm_dac, configure PWM timer for DAC outputs
 */
void configure_pwm_dac (void)
{
#ifdef ENABLE_DEBUG_PWMDAC 
    pac5xxx_timer_clock_config(TimerB, TxCTL_CS_ACLK, TxCTL_PS_DIV1);                       // Configure timer clock input for ACLK (100MHz), /1 divider
    pac5xxx_timer_clock_config(TimerC, TxCTL_CS_ACLK, TxCTL_PS_DIV1);                       // Configure timer clock input for ACLK (100MHz), /1 divider (must match timer B)
    pac5xxx_timer_clock_config(TimerD, TxCTL_CS_ACLK, TxCTL_PS_DIV1);                       // Configure timer clock input for ACLK (100MHz), /1 divider (must match timer B)

    pac5xxx_timer_base_config(TimerB, PWMDAC_PERIOD_TICKS, 0, TxCTL_MODE_UPDOWN, 1);        // Configure Timer B as sync slave
    pac5xxx_timer_base_config(TimerC, PWMDAC_PERIOD_TICKS, 0, TxCTL_MODE_UPDOWN, 1);        // Configure Timer C as sync slave
    pac5xxx_timer_base_config(TimerD, PWMDAC_PERIOD_TICKS, 0, TxCTL_MODE_UPDOWN, 1);        // Configure Timer D as sync slave

    pwmdac_disable();
#endif
}

/**
 * @brief configure_adc_emux_engine, configure ADC EMUX for sending channel selection data
 */
void configure_adc_emux_engine(void)
{
    // EMUX configuration
	// ADCEMUXCTRL DIVIDER is coming from config_app.h
	pac5xxx_adc_emux_config(ADCEMUXCTL_EMUXC_SEQ, ADCEMUXCTRL_EMUXDIV);                 // Configure EMUX to do conversions from ADC sequencer, /2 EMUX divider (HCLK=50MHz/2 = 25MHz)
	pac5xxx_adc_config_emux_io();                                                       // Configure device IO for EMUX

    // ADC Configuration
    pac5xxx_adc_config_io(ADC_CHANNEL_MASK);                                            // Configure device IO for ADC conversions (as Analog inputs)
}

/**
 * @brief config_systick_timer, configure systick timer to call differnt FOC non time critical functions
 */
void config_systick_timer(void)
{
    SysTick_Config(SYSTICK_TICKS);
    NVIC_SetPriority(SysTick_IRQn, 2);
    SysTick->VAL = SYSTICK_TICKS;
}

#if (COMMSEL_UART == TRUE)

/**
 * @brief UART_IRQHandler, UART IRQ interrupt handler
 */
static int uart_rx_byte_count = 0;
static int uart_tx_byte_count = 0;

void UART_IRQHandler(void)
{
    uint8_t int_type = PAC5XXX_UART->IIR.IID;
    uint8_t data = pac5xxx_uart_read();

    if (int_type == UARTIIR_IID_TX_HOLD_EMPTY)
    {
        //debugio1_toggle();
			
		pac5xxx_uart_write(uart_tx_buffer[uart_tx_byte_count++]);   // Send next byte from uart_tx_buffer

        if (uart_tx_byte_count == UART_MESSAGE_SIZE)                // Last byte of message to transmit
        {
            pac5xxx_uart_int_enable_THREI(0);                       // Disable transmit interrupt
            pac5xxx_uart_int_enable_RDAI(1);                        // Enable receive data interrupt for next incoming message
            uart_tx_byte_count = 0;
        }
    }
    else
    {
        //debugio1_toggle();

        if (uart_rx_byte_count == 0)                                // For the first byte received, the start byte must be 0x89
        {
            //debugio1_toggle();
			if (data != 0x89)
			{
                //debugio1_toggle();  
			   return;
			}
        }

        uart_rx_buffer[uart_rx_byte_count++] = data;                // Store data in buffer and increment index

        if (uart_rx_byte_count == UART_MESSAGE_SIZE)                // Received all bytes of message
        {
            uart_message_process();                                 // Process incoming message in uart_rx_buffer and put response in uart_tx_buffer
            uart_rx_byte_count = 0;
            uart_tx_byte_count = 0;

            pac5xxx_uart_int_enable_RDAI(0);                        // Disable receive data interrupt
            pac5xxx_uart_rx_fifo_reset();                           // Reset RX FIFO, to clear RDAI interrupt

            pac5xxx_uart_int_enable_THREI(1);                       // Enable transmit intrerrupt to send reponse to host

        }
    }
}


/**
 * @brief config_uart, configure UART to handle GUI commands
 */
void config_uart(void)
{
    // Configure UART IO
    pac5xxx_uart_io_config();
    pac5xxx_gpio_out_pull_up_e(0x06);                      // UART: PE1/PE2
    // Configure UART peripheral
    pac5xxx_uart_config_LCR(UART_BPC_8,                    // Bits per character
                            UART_STOP_BITS_1,              // Stop Bits
                            0,                             // Parity Enabled
                            (UART_Parity_Type) 0,           // Parity Type
                            0,                             // Stick Parity
                            UART_BRKCTL_NORMAL,            // Break Control
                            (UART_DivLatchAccess_Type) 1); // Divisor Latch Access
    // Configure UART clock for baud rate
    pac5xxx_uart_config_divisor_latch(UART_DIVISOR);
    pac5xxx_uart_config_fractional_divisor(UART_FRACTIONAL_DIVISOR);

    // Configure UART peripheral for access to FIFO from this point on
    pac5xxx_uart_fifo_access();
    // Enable FIFOs, so that the interrupts will work properly
    pac5xxx_uart_fifo_enable(1);
    // Enable receive data available interrupt and NVIC for UART interrupts
    pac5xxx_uart_int_enable_RDAI(1);
    NVIC_EnableIRQ(UART_IRQn);
    NVIC_SetPriority(UART_IRQn,3);
}

#endif  //#if (COMMSEL_UART == TRUE)

#if  (COMMSEL_LIN == TRUE)
/**
 * @brief UART_IRQHandler, UART IRQ interrupt handler
 */

void UART_IRQHandler(void)
{
   lin_handler();
}


/**
 * @brief config_lin, configure LIN to handle GUI commands
 */
void config_lin(void)
{
    // Configure UART IO
    pac5xxx_uart_io_config();
    //Disable output PE1
    PAC5XXX_GPIOE->OUTEN.P1 = 0;                     // UART: PE1/PE2 0x06 = 110
    // Configure UART peripheral
    pac5xxx_uart_config_LCR(UART_BPC_8,                    // Bits per character
                            UART_STOP_BITS_1,              // Stop Bits
                            0,                             // Parity Enabled
                            (UART_Parity_Type) 0,          // Parity Type
                            0,                             // Stick Parity
                            UART_BRKCTL_NORMAL,            // Break Control
                            (UART_DivLatchAccess_Type) 0); // FIFO Access
    // Configure UART clock for baud rate
		pac5xxx_uart_config_divisor_latch_remapped(LIN_DIVISOR);
    //pac5xxx_uart_config_fractional_divisor(LIN_FRACTIONAL_DIVISOR);
    // Configure UART peripheral for access to FIFO from this point on
    pac5xxx_uart_fifo_access();
    // Enable FIFOs, so that the interrupts will work properly
    pac5xxx_uart_fifo_enable(1);
    // Configure Receive FIFO depth
		pac5xxx_uart_rx_fifo_threshold_remapped(UARTFCTL_RT_1B);
    // Enable receive data available interrupt and NVIC for UART interrupts
		pac5xxx_uart_int_enable_RDAI_remapped(1);
		NVIC_ClearPendingIRQ(UART_IRQn);
    NVIC_EnableIRQ(UART_IRQn);
    NVIC_SetPriority(UART_IRQn,3);
}

void config_lin_timer()
{
    // Configure Timer C Base
    pac5xxx_timer_clock_config(TimerC, TxCTL_CS_ACLK, TxCTL_PS_DIV1);                       // Configure timer clock input for ACLK (50MHz), /1 divider                                                        // 07102015, Latch new TAPRD value when timerA counting down, TACTR value = 0x01
    pac5xxx_timer_base_config(TimerC, 49999, 0, TxCTL_MODE_UP, 0);

		pac5xxx_timer_c_clear_assert();
		pac5xxx_timer_base_int_enable(TimerC, 1);

    PAC5XXX_TIMERC->CCTRL1.w =	(3<<0)	|	// CCEDGE=RESERVED
    							(1<<2)	|	// CLEAR CCINT
    							(1<<3)	|	// CCINTEN ENABLED
    							(1<<4);		// CCMODE=CAPTURE

		NVIC_ClearPendingIRQ(TimerC_IRQn);
    NVIC_EnableIRQ(TimerC_IRQn);
		NVIC_SetPriority(TimerC_IRQn, 3);
}
#endif
/**
 * @brief uart_scratch_write, function to write to UART scratch register
 * @param int32_t value
 */
void uart_scratch_write(int32_t value)
{
    // Set the UART Scratch Pad Register value
    PAC5XXX_UART->SP.VAL = value;  // CRC Passed-value=0; CRC Failed-value=1 (i.e. non zero)
}


/**
 * @brief uart_scratch_read, function use to read data from UART scratch register
 * @returns int32_t 32 bit data
 */
int32_t uart_scratch_read(void)
{
    // Return the value stored in the UART Scratch Pad Register
    return PAC5XXX_UART->SP.VAL;
}

/**
 * @brief initialize_foc_timer_seq_trigger_value, use to initialize sequencer0  and 1 triggers
 * @param int32_t ccr_trig_value
 */
void initialize_foc_timer_seq_trigger_value (int32_t ccr_trig_value)
{
#if (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
    extern Struct_Motor motor;
#endif

#if (CONFIG_SHUNT == CONFIG_SHUNT_3SHUNT) || (CONFIG_SHUNT == CONFIG_SHUNT_2SHUNT)
    pac5xxx_timer_a_ccctr0_value_set(ccr_trig_value - 4);                        // Set value for AS0 trigger at triangle peak (PWM center)
#elif (CONFIG_SHUNT == CONFIG_SHUNT_1SHUNT)
    motor.oneshunt.taccr0_value = (ccr_trig_value >> 1);
    motor.oneshunt.taccr1_value = (ccr_trig_value >> 1);
    pac5xxx_timer_a_ccctr0_value_set(ccr_trig_value >> 1);                        // Set value for AS0 trigger at triangle peak (PWM center)
    pac5xxx_timer_a_ccctr1_value_set(ccr_trig_value >> 1);                        // Set value for AS0 trigger at triangle peak (PWM center)
#endif
}

/**
 * @brief initialize_foc_timer_seq_trigger_value_ccr0_ccr1, use to initialize sequencer0 and sequencer1 trigger
 * @param int32_t ccr_trig_value
 */
void initialize_foc_timer_seq_trigger_value_ccr0_ccr1 (int32_t ccr_trig_value)
{
    pac5xxx_timer_a_ccctr0_value_set(ccr_trig_value);                                                                         // Set value for AS0 trigger at triangle peak (PWM center)
    pac5xxx_timer_a_ccctr1_value_set(ccr_trig_value);                                                                         // Set value for AS0 trigger at triangle peak (PWM center)
}

#if defined(ENABLE_SIM) && (SEL_SIM_METHOD == SIM_METHOD_CURRENT)
/**
 * @brief sim_update_seq_trigger_point, use to set sequencer0 trigger point
 * @param int32_t valuee
 */
void sim_update_seq_trigger_point (int32_t value)
{
    pac5xxx_timer_a_ccctr0_value_set(value);    // There seems to be an issue with the sequencer trigger if the trigger is allowed to be <4
}
#endif

/**
 * @brief initialize_socbridge_clk_divider, use to set clock divider of SOC bridge
 * @param int32_t valuee
 */
void initialize_socbridge_clk_divider(void)
{
    PAC5XXX_SOCBRIDGE->SOCBCLKDIV.VAL = SOC_BRIDGE_CLK_DIVIDER;    // SPI CLK = 12.5 MHz = HCLK_FREQ_HZ/ ((CLKDIV + 1) * 2)
}

/**
 * @brief  This function starts an ADC Conversion and Sequencer
 *
 * @return none
 */
void pac5xxx_start_adc_sequencer(void)
{
    pac5xxx_adc_enable(1);
    pac5xxx_adc_start();
}


void deconfig_seq(void)
{
    pac5xxx_adc_int_enable_as0(0);
    pac5xxx_adc_int_enable_as1(0);
}
