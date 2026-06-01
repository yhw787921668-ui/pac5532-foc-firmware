
/**
 * @file push_button.c
 * @brief Use to configure push button as GPIO input, button status and state initialization
 */

//=============================================================================
// Copyright (C) 2020 - 2023, Qorvo, Inc.
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
#include "push_button.h"
#include "pac5xxx_driver_gpio.h"

#include "pac5xxx.h"
#include "config_app.h"
#include "board.h"


/**
 * @brief  This function use configure push button, use GPIO as input
 */
#if (COMMSEL_PUSH_BUTTON == TRUE)

void push_button_config(void)
{
#if defined(FAMILY_55XX)
    PAC5XXX_GPIO_PUSH_BUTTON->MODE.PUSH_BUTTON_PORTNUM = 3;    // High-Impedance Digital Input
    PAC55XX_SCC->PUSH_BUTTON_PULLUP_PORT.PUSH_BUTTON_PORTNUM = 1;    // Enable the weak 60k pull-up to VCCIO
    PAC55XX_SCC->PUSH_BUTTON_SCHMITT_TRIG_PORT.PUSH_BUTTON_SCHMITT_ENABLE = 1;    // Enable Schmitt Trigger
    PAC55XX_SCC->PUSH_BUTTON_PORT_MUX.PUSH_BUTTON_PORTNUM = 0;  // MUX GPIO Mode
    PAC5XXX_GPIO_PUSH_BUTTON->INTEN.PUSH_BUTTON_PORTNUM = 0;   // Disable Interrupt
    PAC5XXX_GPIO_PUSH_BUTTON->CLKSYNC.PUSH_BUTTON_PORTNUM = 1; // Enable De-bounce
#endif

#if defined(FAMILY_52XX)
    PAC5XXX_GPIO_PUSH_BUTTON->OUTEN.PUSH_BUTTON_PORTNUM = 0;    // Disable the output
    PAC5XXX_GPIO_PUSH_BUTTON->DS.PUSH_BUTTON_PORTNUM = 0;       // Disable output Strength   
    PAC5XXX_GPIO_PUSH_BUTTON->PU.PUSH_BUTTON_PORTNUM = 1;       // Enable weak pull-up
    PAC5XXX_GPIO_PUSH_BUTTON->PD.PUSH_BUTTON_PORTNUM = 0;       // Diasable weak pull-down
    PAC5XXX_GPIO_PUSH_BUTTON->PSEL.PUSH_BUTTON_PORTNUM = 0;     // Port GPIO Mode
    PAC5XXX_GPIO_PUSH_BUTTON->INTE.PUSH_BUTTON_PORTNUM = 0;     // Port Pin Interrupt Disable
    PAC5XXX_GPIO_PUSH_BUTTON->INE.PUSH_BUTTON_PORTNUM = 1;      // Port Pin Input Enable
#endif
}


/**
 * @brief  This function use get status of push button
 * @return 1 or 0, low means button pressed
 */
uint32_t get_push_button_gpio_status(void)
{
#if defined(FAMILY_55XX)
    return (PAC5XXX_GPIO_PUSH_BUTTON->IN.PUSH_BUTTON_PORTNUM);    // Return port status, if low means button pressed
#elif defined(FAMILY_52XX)
    return (PAC5XXX_GPIO_PUSH_BUTTON->IN.PUSH_BUTTON_PORTNUM);    // Return port status, if low means button pressed
#else
    return 1;
#endif
}

typedef enum
{
    STATE_PUSH_BUTTON_INIT = 0,                          // 0: Initialize
    STATE_PUSH_BUTTON_CHECK_PRESSED,                     // 1: Check for button press, active low
    STATE_PUSH_BUTTON_CHECK_RELEASED,                    // 2: Check for button release, active high
    STATE_PUSH_BUTTON_END = STATE_PUSH_BUTTON_CHECK_RELEASED // Use to check the range of enumeration
} StateBottonControl;

typedef struct
{
    StateBottonControl button_state;
    uint32_t button_status;
    uint32_t de_bounce_counter;
} Struct_Button;

Struct_Button on_off_button;


/**
 * @brief  This function use get status of push button
 * @return status, 1 or 0
 */
uint32_t get_push_button_status(void)
{
    return (on_off_button.button_status);
}

/**
 * @brief  This function use initialize push button state machine state
 */
void push_button_init_state(void)
{
    on_off_button.button_state = STATE_PUSH_BUTTON_INIT;
}

#define DEBOUNCE_OFF_TO_ON_COUNT_THRD    20
#define DEBOUNCE_ON_TO_OFF_COUNT_THRD    20


/**
 * @brief  This function as push button detection state machine
 */
void statemachine_buttoncontrol (void)
{
    static Struct_Button* button_ptr = &on_off_button;
    static Struct_Motor* motor_ptr = &motor;

    switch (button_ptr->button_state)
    {
        case STATE_PUSH_BUTTON_INIT:
            button_ptr->de_bounce_counter = 0;
            button_ptr->button_status = 0;
            motor_ptr->app.motor_enable_cmd = 0;
            // It will stay in init state until switch released, active low
            if (get_push_button_gpio_status())
            {
                button_ptr->button_state = STATE_PUSH_BUTTON_CHECK_PRESSED;
            }
            break;

        case STATE_PUSH_BUTTON_CHECK_PRESSED:
            // Switch not pressed
            if(get_push_button_gpio_status())
            {
                if (button_ptr->de_bounce_counter > 0)
        	        button_ptr->de_bounce_counter--;
            }
            // Switch pressed
            else
            {
                button_ptr->de_bounce_counter++;
            }

            if (button_ptr->de_bounce_counter >= DEBOUNCE_OFF_TO_ON_COUNT_THRD)
            {
        	      button_ptr->de_bounce_counter = 0;
        	      button_ptr->button_status = 1;
        	      if (!motor_ptr->app.motor_enable_cmd)
        	          motor_ptr->app.motor_enable_cmd = 1;
        	      button_ptr->button_state = STATE_PUSH_BUTTON_CHECK_RELEASED;
            }
            break;

        case STATE_PUSH_BUTTON_CHECK_RELEASED:
            // Switch pressed, active low
            if(!get_push_button_gpio_status())
            {
        	    if (button_ptr->de_bounce_counter > 0)
        	        button_ptr->de_bounce_counter--;
            }
            // Switch released (active high)
            else
            {
        	    button_ptr->de_bounce_counter++;
            }

            if (button_ptr->de_bounce_counter >= DEBOUNCE_ON_TO_OFF_COUNT_THRD)
            {
        	    button_ptr->de_bounce_counter = 0;
        	    button_ptr->button_status = 0;
        	    if (motor_ptr->app.motor_enable_cmd)
        	        motor_ptr->app.motor_enable_cmd = 0;
        	    button_ptr->button_state = STATE_PUSH_BUTTON_CHECK_PRESSED;
            }
            break;

        default:
            button_ptr->button_state = STATE_PUSH_BUTTON_INIT;
            break;
    }
}

#endif  //#if (COMMSEL_PUSH_BUTTON == TRUE)
