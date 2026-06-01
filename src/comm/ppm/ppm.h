
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

#ifndef PPM_H
#define PPM_H

#include "pac5xxx.h"
#include "motor_struct.h"
#include "pac5xxx_driver_timer.h"
#include "pac5xxx_timers.h"
#include "param_funcs.h"
#include "fault.h"
#include "board.h"
#include "config_app.h"
#include "config_features.h"
#include "config_debug.h"

#if (COMMSEL_PPM == TRUE)

////////////////////////////////////////////////////
// PPM User Configuration
////////////////////////////////////////////////////

// PPM pulse frequency (Hz). Frequencies tested: Min=50Hz / Max=500Hz
// Note that pulse widths below need to be set according to selected PPM frequency
#define PPM_FREQUENCY_HZ                     50 //250

// PPM min/max valid pulse width (seconds).  Pulse widths outside of this range will be ignored.
#define MIN_PPM_PULSE_WIDTH_IN_SEC           QCONST(0.000850, Q26)
#define MAX_PPM_PULSE_WIDTH_IN_SEC           QCONST(0.002200, Q26)

// PPM pulse width for motor start/stop speed (seconds)
#define MIN_PPM_START_IN_SEC                 QCONST(0.0016200, Q26) //QCONST(0.001200, Q26)

// PPM pulse width hysteresis for motor start/stop speed (seconds)
#define MIN_PPM_START_HYST_IN_SEC            QCONST(0.000012, Q26)//QCONST(0.000004, Q26)

// PPM pulse width for max speed (seconds)
#define MAX_PPM_WIDTH_MAX_SPD_SEC            QCONST(0.002000, Q26)

// Timeout for ignored pulses and no pulses (seconds).  Motor will disable and fault reported if timeout expires.
#define PPM_MOTOR_OFF_TIMEOUT_IN_SEC_FIX16   QCONST(1.00, Q16)


////////////////////////////////////////////////////
// Internal PPM Configuration
////////////////////////////////////////////////////
#define PPM_GOOD_PULSE_COUNT                 5    // Check 5 good pulses before turning motor on
//#define PPM_CCR_TIMER_CLOCK_DIVIDER          TxCTL_PS_DIV64
#define PPM_CCR_TIMER_CLOCK_DIVIDER          TXCTL_PS_DIV128 //TXCTL_PS_DIV64
#define PPM_TIMER_SINGLE_COUNT_IN_SECONDS    (QCONST(1, Q26)/(TIMERCLK_FREQ_HZ/(1 << PPM_CCR_TIMER_CLOCK_DIVIDER)))
#define PPM_TIMER_TIME_PERIOD                ((QCONST(1, Q26)/PPM_FREQUENCY_HZ)/PPM_TIMER_SINGLE_COUNT_IN_SECONDS) * (QCONST(1.75, Q16))/(QCONST(1, Q16))   // PPM pulse in frequency in Hz

#define PPM_PULSE_VALID_CHECK_IN_SEC_FIX16   ((PPM_TIMER_TIME_PERIOD * PPM_TIMER_SINGLE_COUNT_IN_SECONDS)/QCONST(1, Q10))    // check is we receive new pulse or not

#define MIN_PPM_PULSE_WIDTH_TCOUNTS          (MIN_PPM_PULSE_WIDTH_IN_SEC/PPM_TIMER_SINGLE_COUNT_IN_SECONDS)
#define MAX_PPM_PULSE_WIDTH_TCOUNTS          (MAX_PPM_PULSE_WIDTH_IN_SEC/PPM_TIMER_SINGLE_COUNT_IN_SECONDS)
#define MIN_PPM_START_TCOUNTS                (MIN_PPM_START_IN_SEC/PPM_TIMER_SINGLE_COUNT_IN_SECONDS)
#define MIN_PPM_START_HYST_TCOUNTS           (MIN_PPM_START_HYST_IN_SEC/PPM_TIMER_SINGLE_COUNT_IN_SECONDS)
// Motor close loop speed will be maximum at this duty
#define MAX_PPM_PULSE_WIDTH_MAX_SPD_TCOUNTS  (MAX_PPM_WIDTH_MAX_SPD_SEC/PPM_TIMER_SINGLE_COUNT_IN_SECONDS)

// Set PPM min-max close loop speed
// Using min-max close loop speed fault limit, you can pick any as long as it in with in CLOSE_LOOP_MIN_SPEED_LIMIT_DEFAULT_VALUE, CLOSE_LOOP_MAX_SPEED_LIMIT_DEFAULT_VALUE range
#define MOTOR_MINIMUM_CLOSE_LOOP_SPEED       CLOSE_LOOP_MIN_SPEED_LIMIT_DEFAULT_VALUE  // CL Minimum speed
#define MOTOR_MAXIMUM_CLOSE_LOOP_SPEED       NOMINAL_SPEED_DEFAULT_VALUE               //CLOSE_LOOP_MAX_SPEED_LIMIT_DEFAULT_VALUE // CL Nominal speed

// Motor close loop speed will be minimum at this duty
#define MIN_PPM_START_LIMIT_TCOUNTS         (MIN_PPM_START_TCOUNTS + MIN_PPM_START_HYST_TCOUNTS)    // 949 * 1.28 usec = 1183 usec
#define MOTOR_SPEED_TO_PULSE_WIDTH_SLOPE    (fix16_t)((1.0) * (MOTOR_MAXIMUM_CLOSE_LOOP_SPEED - MOTOR_MINIMUM_CLOSE_LOOP_SPEED))/(MAX_PPM_PULSE_WIDTH_MAX_SPD_TCOUNTS - MIN_PPM_START_LIMIT_TCOUNTS) //0x0006D972 //0x00039D41 // Motor speed in radians to pulse width ratio

// PPM Port/Timer Configuration
#if (PPM_PORT == PORT_PD2)
// PPM Timer Definitions
    #define PPM_TIMER_B                     1
    #define PPM_TIMER_SELECT                PPM_TIMER_B
    #define PPMTIMER                        PAC55XX_TIMERB
    #define PPM_TIMER_CTR                   CTR0
    #define PPM_TIMER_CCTRL                 CCTRL0
    #define PPMGPIO                         PAC55XX_GPIOD
    #define PPM_GPIO_PORT                   P2
    #define PPM_GPIO_PSEL                   3            // in binary format
// PPM Port/Timer Configuration
#elif (PPM_PORT == PORT_PD3)
// PPM Timer Definitions
    #define PPM_TIMER_B                     1
    #define PPM_TIMER_SELECT                PPM_TIMER_B
    #define PPMTIMER                        PAC5XXX_TIMERB
    #define PPM_TIMER_CTR                   CTR1
    #define PPM_TIMER_CCTRL                 CCTRL1
    #define PPMGPIO                         PAC5XXX_GPIOD
    #define PPM_GPIO_PORT                   P3
    #define PPM_GPIO_PSEL                   3            // in binary format
// PPM Port/Timer Configuration
#elif (PPM_PORT == PORT_PD5)
// PPM Timer Definitions
    #define PPM_TIMER_C                     2
    #define PPM_TIMER_SELECT                PPM_TIMER_C
    #define PPMTIMER                        PAC5XXX_TIMERC
    #define PPM_TIMER_CTR                   CTR1
    #define PPM_TIMER_CCTRL                 CCTRL1
    #define PPMGPIO                         PAC5XXX_GPIOD
    #define PPM_GPIO_PORT                   P5
    #define PPM_GPIO_PSEL                   2            // in binary format
  // PPM Port/Timer Configuration
#elif (PPM_PORT == PORT_PD7)
// PPM Timer Definitions
    #define PPM_TIMER_D                     3
    #define PPM_TIMER_SELECT                PPM_TIMER_D
    #define PPMTIMER                        PAC5XXX_TIMERD
    #define PPM_TIMER_CTR                   CTR0
    #define PPM_TIMER_CCTRL                 CCTRL0
    #define PPMGPIO                         PAC5XXX_GPIOD
    #define PPM_GPIO_PORT                   P7
    #define PPM_GPIO_PSEL                   2             // in binary format

#elif (PPM_PORT == PORT_PF2)

#else
//    #error: PPM port not selected.
#endif


extern void config_ppm (TimerInstance timer);
extern void ppm_init(Struct_Motor* motor_ptr);

PAC5XXX_RAMFUNC static void ppm_convert_pulse_width_into_speed (uint32_t* motor_enable_cmd_ptr, fix16_t* speedrefcl_target_ptr, eStateAppMain state_main, Struct_Motor* motor_ptr)
{
    fix16_t motor_new_calculated_speed;
    fix16_t temp_pulse_time = 0;

    // Is we detect valid pulse from PPM pulse in ISR
    if (motor_ptr->ppm.pulse_detected)
    {
        __disable_irq();

        motor_ptr->ppm.ppm_pulse_delta = motor_ptr->ppm.ppm_pulse_low - motor_ptr->ppm.ppm_pulse_high ;
        motor_ptr->ppm.pulse_detected = FALSE;
        __enable_irq();

        if (motor_ptr->ppm.ppm_pulse_delta < 0)
        {
            motor_ptr->ppm.ppm_pulse_delta += PPM_TIMER_TIME_PERIOD;
        }
        
				motor_ptr->ppm.pulse_time = motor_ptr->ppm.ppm_pulse_delta;

        if (motor_ptr->ppm.invalid_pulse_detect_count > 0)
            motor_ptr->ppm.invalid_pulse_detect_count --;

        // if detect the valid pulse check is pulse on time in with-in the range
        // check pulse on time is with-in range
        if ((motor_ptr->ppm.pulse_time >= MIN_PPM_PULSE_WIDTH_TCOUNTS) || (motor_ptr->ppm.pulse_time <= MAX_PPM_PULSE_WIDTH_TCOUNTS))
        {
            // Make sure when powerup pluse need to be within range of stick center position else motor will off
					  if (motor_ptr->ppm.motor_powerup_flag)
            {
                // make sure for 1st motor startup, PPM pulse need to be minimum

//  							if (motor_ptr->ppm.pulse_time < (MIN_PPM_START_TCOUNTS + MIN_PPM_START_HYST_TCOUNTS))
//                {
//                    motor_ptr->ppm.motor_powerup_flag = 0;
//                }

//                *motor_enable_cmd_ptr = 0;

                // make sure for 1st motor startup, PPM pulse need to be minimum
  							if ((motor_ptr->ppm.pulse_time > (MIN_PPM_START_TCOUNTS - MIN_PPM_START_HYST_TCOUNTS))
								    && (motor_ptr->ppm.pulse_time < (MIN_PPM_START_TCOUNTS + MIN_PPM_START_HYST_TCOUNTS)))	
                {
                    motor_ptr->ppm.motor_powerup_flag = 0;
                }

                *motor_enable_cmd_ptr = 0;
						}
            else
            {
                if (motor_ptr->ppm.pulse_time >= (MIN_PPM_START_TCOUNTS + MIN_PPM_START_HYST_TCOUNTS))
                {
                    if (*motor_enable_cmd_ptr == 0)
                    {
                        // if motor is not enable but get valid pulse with on time, check for it for 5 time then enable the motor
                        if (motor_ptr->ppm.valid_motor_on_pulse_detect++ >= PPM_GOOD_PULSE_COUNT)
                        {
                            motor_ptr->app.app_motor_direction_cmd = 1;
							              *motor_enable_cmd_ptr  = 1;
                            motor_ptr->ppm.valid_motor_on_pulse_detect = 0;
                        }
                    }

                    if(motor_ptr->ppm.pulse_time > MAX_PPM_PULSE_WIDTH_MAX_SPD_TCOUNTS)
                        temp_pulse_time = MAX_PPM_PULSE_WIDTH_MAX_SPD_TCOUNTS - MIN_PPM_START_LIMIT_TCOUNTS;
                    else
                        temp_pulse_time = motor_ptr->ppm.pulse_time - MIN_PPM_START_LIMIT_TCOUNTS;

                    temp_pulse_time = temp_pulse_time << 16;

                    // calculate speed only when motor state is open loop, close loop or coasting else close loop target speed set to 0
                    if ((state_main == STATE_MAIN_STARTUP) || (state_main == STATE_MAIN_RUN) || (state_main == STATE_MAIN_COASTING))
                    {
                        // calculate close loop speed with pulse on time and slope
											  temp_pulse_time = fix16_mul_inline(temp_pulse_time, MOTOR_SPEED_TO_PULSE_WIDTH_SLOPE);
//											  debugio1_toggle();
                    }
                    else
                        temp_pulse_time = 0;
										
										motor_new_calculated_speed = motor_ptr->startup.aligngo.speedrefol_target_radps + temp_pulse_time;
										*speedrefcl_target_ptr = motor_new_calculated_speed;
                }
								else if (motor_ptr->ppm.pulse_time <= (MIN_PPM_START_TCOUNTS - MIN_PPM_START_HYST_TCOUNTS))
                {
                    if (*motor_enable_cmd_ptr == 0)
                    {
                        // if motor is not enable but get valid pulse with on time, check for it for 5 time then enable the motor
                        if (motor_ptr->ppm.valid_motor_on_pulse_detect++ >= PPM_GOOD_PULSE_COUNT)
                        {
                            motor_ptr->app.app_motor_direction_cmd = -1;
													  *motor_enable_cmd_ptr  = 1;
                            motor_ptr->ppm.valid_motor_on_pulse_detect = 0;
                        }
                    }

                    temp_pulse_time = MIN_PPM_START_LIMIT_TCOUNTS - motor_ptr->ppm.pulse_time;

                    temp_pulse_time = temp_pulse_time << 16;

                    // calculate speed only when motor state is open loop, close loop or coasting else close loop target speed set to 0
                    if ((state_main == STATE_MAIN_STARTUP) || (state_main == STATE_MAIN_RUN) || (state_main == STATE_MAIN_COASTING))
                    {
                        // calculate close loop speed with pulse on time and slope
											  temp_pulse_time = fix16_mul_inline(temp_pulse_time, MOTOR_SPEED_TO_PULSE_WIDTH_SLOPE);
//											  debugio1_toggle();
                    }
                    else
                        temp_pulse_time = 0;
										
										motor_new_calculated_speed = motor_ptr->startup.aligngo.speedrefol_target_radps + temp_pulse_time;
										*speedrefcl_target_ptr = motor_new_calculated_speed;
                }
                else
                {
                    // check is pulse less or equal to turn the motor off, count for 5 pulses before turning motor off
                    //if (motor_ptr->ppm.pulse_time <= (MIN_PPM_START_TCOUNTS - MIN_PPM_START_HYST_TCOUNTS))
                    // make sure for 1st motor startup, PPM pulse need to be minimum
  							    if ((motor_ptr->ppm.pulse_time > (MIN_PPM_START_TCOUNTS - MIN_PPM_START_HYST_TCOUNTS))
								        && (motor_ptr->ppm.pulse_time < (MIN_PPM_START_TCOUNTS + MIN_PPM_START_HYST_TCOUNTS)))	
									  {
                        if (*motor_enable_cmd_ptr == 1)
                        {
                            if (motor_ptr->ppm.valid_motor_on_pulse_detect++ >= PPM_GOOD_PULSE_COUNT)
                            {
                                *motor_enable_cmd_ptr = 0;
                                motor_ptr->ppm.valid_motor_on_pulse_detect = 0;
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        // Check is this time to check for valid pulse
        if (motor_ptr->ppm.valid_pulse_counter_temp++ > motor_ptr->ppm.pulse_timeout_in_sys_tick_isr_counts)
        {
            motor_ptr->ppm.valid_pulse_counter_temp = 0;
            motor_ptr->ppm.invalid_pulse_detect_count++;

            // If pulse is not valid then we need to disable the motor
            if (*motor_enable_cmd_ptr == 1)
            {
                // if motor is enabled and did not received any pulse for motor stop timeout, disable the motor
                if (motor_ptr->ppm.invalid_pulse_detect_count > motor_ptr->ppm.motor_off_timeout_in_pulse_timeout_counts)
                {
                    motor_ptr->ppm.invalid_pulse_detect_count = 0;
                    motor_ptr->ppm.valid_motor_on_pulse_detect = 0;
                    set_fault(FAULT_PPM_PULSE_TIMEOUT);
                }
            }
        }
    }
}

#if (COMMSEL_PPM == TRUE)
PAC5XXX_RAMFUNC static inline void ppm_process_capture_timer_events (void)
{
    static Struct_Motor* motor_ptr = &motor;
		
	  if (PPM_Timer_Ptr->PPM_Timer_IntFlag.BASEIF)
		{
        // Clear interrupt flag
		    PPM_Timer_Ptr->PPM_Timer_IntFlag.BASEIF = 1;
	      //debugio1_toggle();
		}
	  	
		if (PPM_Timer_Ptr->PPM_Timer_IntFlag.PPM_CCR_INT_FLAG == TRUE)
    {
        motor_ptr->ppm.timer_counts = PPM_Timer_Ptr->PPM_CC_COUNTER.CTR;   //->PPM_CC_COUNTER;

        // Check is GPIO high
			  if(PAC5XXX_GPIO_PPM_IN->IN.PPM_INPUT_PORTNUM == TRUE)
        {
            motor_ptr->ppm.ppm_rise_ccr_value = motor_ptr->ppm.timer_counts;
            motor_ptr->ppm.flag_cap_rising_first = 1 ;
					  //debugio1_on();
        }
        else
        {    // Check is GPIO low and we got the rising edge
            if(motor_ptr->ppm.flag_cap_rising_first == TRUE)
            {
                motor_ptr->ppm.ppm_pulse_high = motor_ptr->ppm.ppm_rise_ccr_value;
                motor_ptr->ppm.ppm_pulse_low = motor_ptr->ppm.timer_counts;
                motor_ptr->ppm.flag_cap_rising_first = 0 ;
                motor_ptr->ppm.pulse_detected = TRUE;
							  //debugio1_off();
            }
        }
        // Clear capture flag
				PPM_Timer_Ptr->PPM_Timer_IntFlag.PPM_CCR_INT_FLAG = 1;
     }
#endif
}
#endif  //#if (COMMSEL_PPM == TRUE)

#endif  //#ifndef PPM_H
