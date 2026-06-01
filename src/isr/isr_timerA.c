/**
 * @file isr_timerA.c
 * @brief Use for timerA timer interrupt handler to run critical FOC functions
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

#include "isr_config.h"
#include "isr_funcs.h"
#include "motor_struct.h"

//新增变量   by:owz
extern uint8_t  task_1ms_timer_f;
extern uint8_t  task_10ms_timer_f;
extern uint8_t  task_100ms_timer_f;
extern uint8_t  task_500ms_timer_f;
extern uint8_t tmp_ct,tmp_ct1,tmp_ct2,tmp_ct3;
extern uint16_t time_counter1,time_counter,tmp_ct4,tmp_ct5;
extern int Debug_Test_Number1;
extern int Debug_Test_Number2;
extern int Debug_Test_Number3;

/**
 * @brief Process interrupt for TimerA_IRQHandler to run critical FOC functions
 */
PAC5XXX_OPTIMIZE_SPEED_ON
PAC5XXX_RAMFUNC void TimerA_IRQHandler(void)
{
//    debugio1_toggle();

#if defined(ENABLE_INT_TA_BASE) || defined(ENABLE_INT_TA_CCR0) || defined(ENABLE_INT_TA_CCR2) 
    static Struct_Motor* motor_ptr = &motor;

    int32_t intflags = read_timer_a_interrupt_flags();
	
	//代码增加 by:owz
		tmp_ct++;
    if( tmp_ct>=200 )  // 20khz,此处计数到200表示10ms
    {
        tmp_ct=0;
			  task_10ms_timer_f = 1;
    }
    
    tmp_ct2++;
    if( tmp_ct2 >= 20 ) // 20khz pwm, 所以此处计数到20表示为1ms
    {
        tmp_ct2 = 0; 
        task_1ms_timer_f = 1;		
        tmp_ct3++;
        if( tmp_ct3 >= 100 )
        {
            tmp_ct3 = 0;
            task_100ms_timer_f = 1; //100ms外部变量
        }
				
				tmp_ct5++;
        if( tmp_ct5 >= 500 )
        {
            tmp_ct5 = 0;
            task_500ms_timer_f = 1; //500ms外部变量
        }
			}
	
			
    clear_timer_a_interrupt_flags();
#endif

    #ifdef ENABLE_INT_TA_BASE
    if ((intflags & BIT_08) && read_timer_a_inten_base())
    {
        // Call TA base interrupt function pointer
        motor_ptr->fptr.fptr_int_TA_base(motor_ptr);
        motor_ptr->fptr.fptr_int_TA_base_2(motor_ptr);
    }
    #endif

    #ifdef ENABLE_INT_TA_CCR0
    if ((intflags & BIT_00) && read_timer_a_inten_ccr0())
    {
        // Call TA CCR0 interrupt function pointer 
        motor_ptr->fptr.fptr_int_TA_ccr0(motor_ptr);
        motor_ptr->fptr.fptr_int_TA_ccr0_2(motor_ptr);
    }
    #endif

    #ifdef ENABLE_INT_TA_CCR2
    if ((intflags & BIT_02) && read_timer_a_inten_ccr2())
    {
        // Call TA CCR2 interrupt function pointer 
        motor_ptr->fptr.fptr_int_TA_ccr2(motor_ptr);
        motor_ptr->fptr.fptr_int_TA_ccr2_2(motor_ptr);
    }
    #endif
//    debugio1_toggle();
//    debugio1_toggle();
//    debugio1_toggle();
}
PAC5XXX_OPTIMIZE_SPEED_OFF
