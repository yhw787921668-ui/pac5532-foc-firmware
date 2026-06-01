/******************************************************************************
 * FILE NAME   :  VSM_control.h
 ******************************************************************************
 *
 * PROJECT NAME:  
 *
 * DESCRIPTION :  Header file containing data types and prototypes for the 
 *				      functions in this module. 
 *
 * ORIGINATOR   :  TATC
 *
 * AUTHORS      :  Owen
 *
 * CREATED ON   :  2024.01.12
 *
 * HISTORY:
 * DATE         BY    DESCRIPTION
 * ----------   ---   -----------
 * 2024.01.12   Owen  Baseline Version
 *****************************************************************************/

#ifndef __VSM_CONTROL_H__
#define __VSM_CONTROL_H__

/***   Include Files   ***/
#include "motor_struct.h"
#include "app_funcs.h"
#include "pac_init.h"
#include "fault.h"
#include "pac5xxx_driver_adc.h"

/***   Type Definitions ***/
typedef  struct  {
    uint8_t  inv_enable;   // 0=stop, 1=run
    uint8_t  run_mode;     // 0=torque, 1=speed 
    uint8_t  lock_motor;   // 0=not lock, 1=lock motor
    int16_t  torq_cmd;     // 0~100%, Q10 format 准备改为转矩限制
    int16_t  speed_cmd;    // rpm
    uint8_t  gear_shadow;  // used for speed_mode
    int16_t  speed_shadow; // used for speed_mode
	  uint8_t  Assist_Rate;  // 助力比
	int16_t  speed_max_shadow;  //
	int16_t id_cmd;
	int16_t iq_cmd;
	int16_t angle_offset_cal;
	int16_t kp;
	int16_t ki;
	
} SYS_RUN_CMD;


typedef  struct  {
    uint8_t  inv_running;  // 0=stop, 1=running
    uint8_t  inv_fault;    // 0=no fault, 1=fault
    int16_t  speed_fdb;    // rpm
    int16_t  timer_ms;     // timer_ms, 运行或停止了多长时间
} SYS_RUN_FDB;

/***   Macro Definitions ***/
#define  SYS_RUN__00_POWER_ON     0
#define  SYS_RUN__01_INITIALIZE   1
#define  SYS_RUN__02_GEAR_P       2
#define  SYS_RUN__03_WAIT         3
#define  SYS_RUN__04_TORQUE       4
#define  SYS_RUN__05_SPEED        5
#define  SYS_RUN__06_STOPPING     6

#define  SYS_RUN__09_LOCK_MTR     9

#define  SYS_RUN__10_ERROR       10
#define  SYS_RUN__11_SLOW_RUN    11
#define  SYS_RUN__12_SLOW_STOP   12

#define  SYS_RUN__20_DISABLE     20


#define  TORQUE_MODE    0
#define  SPEED_MODE     1

// 控制器运行模式
#define  RUN_MODE_TORQUE    0
#define  RUN_MODE_SPEED     1

#define  SYS_COUNTER_MAX    60000




/***   Definitions Provided to other modules  ***/
/* NONE */

/***   Initialization Functions (provided to other modules)  ***/
extern void system_running_init(void);

/***   Non-init Functions (provided to other modules)  ***/
extern void system_running_state(void);
extern void run_command_word_clear(void);
extern void run_command_lock_motor(void);
extern void vsm_command_to_inverter(void);
extern void inv_status_feedback_to_vsm(void);

/***   Non-init Functions to be copied to RAM   ***/
/* NONE */

/***   Variables Provided to other modules  ***/
extern uint8_t  sys_running_state;//声明
extern SYS_RUN_CMD  run_command;
extern SYS_RUN_FDB  run_feedback;

#endif // __VSM_CONTROL_H__

/******************************************************************************
 * End of file
 *****************************************************************************/

