
/**
 * @file push_button.h
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

#ifndef PUSH_BUTTON_H
#define PUSH_BUTTON_H

extern void push_button_config(void);
extern void push_button_init_state(void);
extern void statemachine_buttoncontrol(void);
extern uint32_t get_push_button_gpio_status(void);
extern uint32_t get_push_button_status(void);

#endif  //#ifndef PUSH_BUTTON_H
