
/**
 * @file hall_io_config.h
 * @brief Use for hall port and interrupt initialization and utility function
 */

//=============================================================================
// Copyright (C) 2019 - 2020, Qorvo, Inc.
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

#ifndef HALL_IO_CONFIG_H
#define HALL_IO_CONFIG_H

#include "pac5xxx.h"
#include "config_app.h"
#include "board.h"
#include "mpos_struct.h"

#if ((MODULE_MPOS_HALL == ENABLED))

//=====================================
// HALL Input Port Definitions
//=====================================
// get the hall value

/**
 * @brief Use to get port hall status
 */
#define GET_HALL_VALUE  ((PAC5XXX_GPIO_HALLC->IN.HALLC_PORTNUM << 2) + (PAC5XXX_GPIO_HALLB->IN.HALLB_PORTNUM << 1) + (PAC5XXX_GPIO_HALLA->IN.HALLA_PORTNUM << 0))

#if defined(FAMILY_52XX)

//==========================================
// Hall Control Macro Functions
//==========================================
// HALL Port Controls
/**
 * @brief Use for hall port initialization
 */
static inline void hall_io_config(void)
{
    // config as input
    PAC5XXX_GPIO_HALLA->OUTEN.HALLA_PORTNUM = 0;
    PAC5XXX_GPIO_HALLB->OUTEN.HALLB_PORTNUM = 0;
    PAC5XXX_GPIO_HALLC->OUTEN.HALLC_PORTNUM = 0;

    // config pull up
    PAC5XXX_GPIO_HALLA->PU.HALLA_PORTNUM = 1;
    PAC5XXX_GPIO_HALLB->PU.HALLB_PORTNUM = 1;
    PAC5XXX_GPIO_HALLC->PU.HALLC_PORTNUM = 1;
}
#endif  //#if defined(FAMILY_52XX)


//==========================================
// Hall Control Macro Functions
//==========================================
// HALL Port Controls
#if defined(FAMILY_55XX)
/**
 * @brief Use for hall port initialization
 */
static inline void hall_io_config(void)
{
    // select GPIO
    PAC55XX_SCC->HALL_MUXSEL.HALLA_PORTNUM = 0;
    PAC55XX_SCC->HALL_MUXSEL.HALLB_PORTNUM = 0;
    PAC55XX_SCC->HALL_MUXSEL.HALLC_PORTNUM = 0;
    // configuration as input
    PAC5XXX_GPIO_HALLA->MODE.HALLA_PORTNUM = 3;
    PAC5XXX_GPIO_HALLB->MODE.HALLB_PORTNUM = 3;
    PAC5XXX_GPIO_HALLC->MODE.HALLC_PORTNUM = 3;
    // configuration pull up
    PAC55XX_SCC->HALL_PUEN.HALLA_PORTNUM = 1;
    PAC55XX_SCC->HALL_PUEN.HALLB_PORTNUM = 1;
    PAC55XX_SCC->HALL_PUEN.HALLC_PORTNUM = 1;
}

#if defined(ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION)
PAC5XXX_OPTIMIZE_SPEED_ON
static inline void pac5xxx_gpio_int_polarity_hallport_bit_set_value(uint8_t port_mask, uint8_t value)
{
    switch (port_mask)
    {
        case HALLA_PORT_BIT:
            if (value)
                PAC5XXX_GPIO_HALLA->INTCFG.HALLA_PORTNUM = 0;
            else
                PAC5XXX_GPIO_HALLA->INTCFG.HALLA_PORTNUM = 1;
            break;
        case HALLB_PORT_BIT:
            if (value)
                PAC5XXX_GPIO_HALLB->INTCFG.HALLB_PORTNUM = 0;
            else
                PAC5XXX_GPIO_HALLB->INTCFG.HALLB_PORTNUM = 1;
            break;
        case HALLC_PORT_BIT:
            if (value)
                PAC5XXX_GPIO_HALLC->INTCFG.HALLC_PORTNUM = 0;
            else
                PAC5XXX_GPIO_HALLC->INTCFG.HALLC_PORTNUM = 1;
            break;
        default:
            break;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif  //#if defined(ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION)
#endif  //#if defined(FAMILY_55XX)


#if (HALL_METHOD_SELECT == HALL_METHOD_INTERRUPT)
// Input (Hall A interrupt)
#define NIRQA_PIN_MASK        HALLA_PORT_BIT    // Hall A pin bit mask
// Input (Hall B interrupt)
#define NIRQB_PIN_MASK        HALLB_PORT_BIT    // Hall B pin bit mask
// Input (Hall C interrupt)
#define NIRQC_PIN_MASK        HALLC_PORT_BIT    // nIRQ1 pin bit mask


#if defined(FAMILY_52XX)
/**
 * @brief Use to set hall port mask bit
 * @param uint8_t port_bit_mask
 */
PAC5XXX_OPTIMIZE_SPEED_ON
static inline void pac5xxx_gpio_int_mask_hallport_bit_set(uint8_t port_bit_mask) { PAC5XXX_GPIO_HALL->INTM.b |= port_bit_mask; }
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
/**
 * @brief Use to clear hall interrupt port mask bit
 * @param uint8_t port_bit_mask
 */
static inline void pac5xxx_gpio_int_mask_hallport_bit_clr(uint8_t port_bit_mask) { PAC5XXX_GPIO_HALL->INTM.b &= (~port_bit_mask);}
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
/**
 * @brief Use to set hall port polarity mask bit
 * @param uint8_t port_bit_mask
 */
static inline void pac5xxx_gpio_int_polarity_hallport_bit_low_high(uint8_t polarity_bit_mask) { PAC5XXX_GPIO_HALL->INTP.b |= polarity_bit_mask; }
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use to clear hall port polarity mask bit
 * @param uint8_t port_bit_mask
 */
PAC5XXX_OPTIMIZE_SPEED_ON
static inline void pac5xxx_gpio_int_polarity_hallport_bit_high_low(uint8_t polarity_bit_mask) { PAC5XXX_GPIO_HALL->INTP.b &= (~polarity_bit_mask); }
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
/**
 * @brief Use to enable hall port interrupt enable bit
 * @param uint8_t port_bit_mask
 */
static inline void pac5xxx_gpio_int_enable_hallport_bit(uint8_t enable_bit_mask) { PAC5XXX_GPIO_HALL->INTE.b |= enable_bit_mask; }
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
/**
 * @brief Use to set hall port interrupt status flag
 * @param uint8_t port_bit_mask
 */
static inline void pac5xxx_gpio_int_flag_clear_hallport_bit(uint8_t port_bit_mask) { PAC5XXX_GPIO_HALL->INTF.b |=  port_bit_mask; }
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
/**
 * @brief Use to clear hall port interrupt flags
 * @param uint8_t port_bits
 */
static inline void pac5xxx_gpio_int_flag_clear_hallport(uint8_t port_bits)
{
    pac5xxx_gpio_int_flag_clear_d(port_bits);                                  // Clear interrupt flags
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use to invert hall port interrupt polarity bit
 * @param uint8_t port_mask
 */
PAC5XXX_OPTIMIZE_SPEED_ON
static inline void pac5xxx_gpio_int_polarity_hallport_bit_set_value(uint8_t port_mask, uint8_t value)
{
    switch (port_mask)
    {
        case HALLA_PORT_BIT:
            if (value)
                PAC5XXX_GPIO_HALLA_POLARITY = 0;
            else
                PAC5XXX_GPIO_HALLA_POLARITY = 1;
            break;
        case HALLB_PORT_BIT:
            if (value)
                PAC5XXX_GPIO_HALLB_POLARITY = 0;
            else
                PAC5XXX_GPIO_HALLB_POLARITY = 1;
            break;
        case HALLC_PORT_BIT:
            if (value)
                PAC5XXX_GPIO_HALLC_POLARITY = 0;
            else
                PAC5XXX_GPIO_HALLC_POLARITY = 1;
            break;
        default:
            break;
    }
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use to initialize hall port bit and enable hall port interrupt
 * @param uint8_t port_mask
 * @param uint8_t active_high_mask
 */
PAC5XXX_OPTIMIZE_SPEED_ON
static void pac5xxx_gpio_configure_interrupt_hallport_init(uint8_t port_mask, uint8_t active_high_mask)
{
    pac5xxx_gpio_int_mask_hallport_bit_set(port_mask);                         // Set interrupt mask at startup (interrupt startup workaround)
    if (active_high_mask)
        pac5xxx_gpio_int_polarity_hallport_bit_low_high(port_mask);            // Set interrupt polarity
    else
        pac5xxx_gpio_int_polarity_hallport_bit_high_low(port_mask);            // Set interrupt polarity
    pac5xxx_gpio_int_enable_hallport_bit(port_mask);                           // Enable interrupts in IO controller
    NVIC_EnableIRQ(Hall_Gpio_IRQn);                                            // Enable interrupts in NVIC
    pac5xxx_gpio_int_mask_hallport_bit_clr(port_mask);                         // Clear any active interrupt flags
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use to initialize and configure hall port and interrupt
 */
static void configure_hall_interrupt_init(void)
{
    // Configure Hall port interrupts and polarity
    pac5xxx_gpio_configure_interrupt_hallport_init(NIRQA_PIN_MASK, ~PAC5XXX_GPIO_HALLA->IN.HALLA_PORTNUM);
    pac5xxx_gpio_configure_interrupt_hallport_init(NIRQB_PIN_MASK, ~PAC5XXX_GPIO_HALLB->IN.HALLB_PORTNUM);
    pac5xxx_gpio_configure_interrupt_hallport_init(NIRQC_PIN_MASK, ~PAC5XXX_GPIO_HALLC->IN.HALLC_PORTNUM);
    NVIC_SetPriority(Hall_Gpio_IRQn, 0);                 // Set GPIO Port D interrupt priority to highest
}


/**
 * @brief Use for disabling hall port interrupt
 * @param uint8_t port_bits
 */
PAC5XXX_OPTIMIZE_SPEED_ON
static inline void pac5xxx_gpio_int_disable_hallport(uint8_t port_bits)
{
    //Disable hall port interrupts
    PAC5XXX_GPIO_HALL->INTE.b &= ~(NIRQA_PIN_MASK | NIRQB_PIN_MASK | NIRQC_PIN_MASK);
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use for enabling hall port interrupt
 * @param uint8_t port_bits
 */
PAC5XXX_OPTIMIZE_SPEED_ON
static inline void pac5xxx_gpio_int_enable_hallport(uint8_t port_bits)
{
    //Enable hall port interrupts
    PAC5XXX_GPIO_HALL->INTE.b |= (NIRQA_PIN_MASK | NIRQB_PIN_MASK | NIRQC_PIN_MASK);
}
PAC5XXX_OPTIMIZE_SPEED_OFF
#endif  // #if defined(FAMILY_52XX)


#if defined(FAMILY_55XX)
/**
 * @brief Use for clearing hall port interrupt
 * @param uint8_t port_bits
 */
PAC5XXX_OPTIMIZE_SPEED_ON
static inline void pac5xxx_gpio_int_flag_clear_hallport(uint8_t port_bits)
{
    PAC5XXX_GPIO_HALL->INTCLEAR.w |= port_bits;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use for initialization and enabling hall port interrupt
 * @param uint8_t port_bits
 */
PAC5XXX_OPTIMIZE_SPEED_ON
static void pac5xxx_gpio_configure_interrupt_hallport_init(uint8_t port_mask)
{
    PAC5XXX_GPIO_HALL->INTTYPE.w &= ~port_mask;          // Edge trigger
    #if defined(ENABLE_HALL_SIGNAL_FAULT_TOLERANT_OPERATION)
    PAC5XXX_GPIO_HALL->INTEDGEBOTH.w &= ~port_mask;      // Trigger on rising or falling edges
    #else
    PAC5XXX_GPIO_HALL->INTEDGEBOTH.w |= port_mask;       // Both edges
    #endif
    PAC5XXX_GPIO_HALL->INTEN.w |= port_mask;             // Enable Interrupt
    PAC5XXX_GPIO_HALL->INTCFG.w |= port_mask;            // Rising edge
    PAC5XXX_GPIO_HALL->CLKSYNC.w |= port_mask;           // Enable De-bounce
    NVIC_EnableIRQ(Hall_Gpio_IRQn);                      // Enable interrupts in NVIC
    PAC5XXX_GPIO_HALL->INTCLEAR.w |= port_mask;          // Clear interrupt flag
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use for initialization and configure hall port interrupts
 */
static void configure_hall_interrupt_init(void)
{
    // Configure Hall port interrupts and polarity
    PAC5XXX_GPIO_HALL->INTTYPE.w = 0;                    // Edge trigger
    PAC5XXX_GPIO_HALL->INTEN.w = 0;                      // Enable Interrupt
    PAC5XXX_GPIO_HALL->INTCFG.w = 0;                     // Rising edge
//    PAC5XXX_GPIO_HALL->CLKSYNC.w = 0;                    // Enable De-bounce
    PAC5XXX_GPIO_HALL->INTCLEAR.w = 0xFF;                // Clear interrupt flag
    pac5xxx_gpio_configure_interrupt_hallport_init(NIRQA_PIN_MASK);
    pac5xxx_gpio_configure_interrupt_hallport_init(NIRQB_PIN_MASK);
    pac5xxx_gpio_configure_interrupt_hallport_init(NIRQC_PIN_MASK);
    NVIC_SetPriority(Hall_Gpio_IRQn, 0);                 // Set GPIO Port D interrupt priority to highest
}


/**
 * @brief Use for disabling hall port interrupt
 * @param uint8_t port_bits
 */
PAC5XXX_OPTIMIZE_SPEED_ON
static inline void pac5xxx_gpio_int_disable_hallport(uint8_t port_bits)
{
    //Disable hall port interrupts
    PAC5XXX_GPIO_HALL->INTEN.w &= ~(NIRQA_PIN_MASK | NIRQB_PIN_MASK | NIRQC_PIN_MASK);
}
PAC5XXX_OPTIMIZE_SPEED_OFF


/**
 * @brief Use to set hall port interrupt
 * @param uint8_t port_bits
 */
PAC5XXX_OPTIMIZE_SPEED_ON
static inline void pac5xxx_gpio_int_enable_hallport(uint8_t port_bits)
{
    //Disable hall port interrupts
    PAC5XXX_GPIO_HALL->INTEN.w |= (NIRQA_PIN_MASK | NIRQB_PIN_MASK | NIRQC_PIN_MASK);
}
PAC5XXX_OPTIMIZE_SPEED_OFF

#endif  // #if defined(FAMILY_55XX)
#endif  // #if (HALL_METHOD_SELECT == HALL_METHOD_INTERRUPT)
#endif  //#if ((MODULE_MPOS_HALL == ENABLED))

#endif  //HALL_IO_CONFIG_H
