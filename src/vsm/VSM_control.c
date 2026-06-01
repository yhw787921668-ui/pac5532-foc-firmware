/******************************************************************************
 * FILE NAME    :  VSM_control.c
 ******************************************************************************
 * PROJECT NAME :  
 *
 * DESCRIPTION  :  vcu control
 * 
 * ORIGINATOR   :  TATC
 *
 * AUTHORS      :  Owen
 *
 * CREATED ON   :  2024.01.12
 *
 * HISTORY:
 * DATE         BY    DESCRIPTION
 * ----------   ----  ------------
 * 2024.01.12   Owen  Baseline Version
 *****************************************************************************/

/***   Include Files   ***/
#include "VSM_control.h"
#include "motor_struct.h"
#include "app_funcs.h"
#include "pac_init.h"
#include "fault.h"
#include "pac5xxx_driver_adc.h"
#include "Driver_Strategy.h"  // by:owz

uint8_t  sys_running_state=0;//定义
SYS_RUN_CMD  run_command;
SYS_RUN_FDB  run_feedback;

//新增变量   by:owz
extern int Debug_Test_Number1;
extern int Debug_Test_Number2;
extern int Debug_Test_Number3;
extern int Debug_Test_Number4;

/***   External Variables   ***/

/******************************************************************************
 * FUNCTION NAME :  void system_running_init(void)
 ******************************************************************************
 * DESCRIPTION   :  主系统运行状态机初始化
 *      Inputs   :  
 *      Outputs  :  sys_running_state
 *                            
 * HISTORY :
 * ----------  ----  ------------
 * 2024.01.12  Owen  Baseline Version
 *****************************************************************************/
void system_running_init(void)
{
	  static Struct_Motor* motor_ptr = &motor;
	
    motor_ptr->app.motor_enable_cmd = 0;            // Set to non-zero if motor is enabled
    motor_ptr->app.speed_mode_flag = 0;             // Torque mode
    motor_ptr->app.app_motor_direction_cmd = 1;     // Direction indicator: +1=forward, -1=reverse
    motor_ptr->app.auto_close_loop_cmd = 0;         // Bit set when we need to go from OL to close loop in auto mode
    motor_ptr->app.coast_timeout_target = 1;
    motor_ptr->app.idrefcl_cmd_f16 = 0;             // Storage for idref_target_f16
    motor_ptr->app.iqrefcl_cmd_f16 = 0;             // Storage for iqref_target_f16
    motor_ptr->app.flag_first_start = 0;            // Flag indicates when the controller powers up for autostart
    motor_ptr->app.outerloop_target_cmd_f16 = 0;
}


/******************************************************************************
 * FUNCTION NAME :  void system_running_state(void)
 ******************************************************************************
 * DESCRIPTION   :  主系统运行状态机
 *      Inputs   :  
 *      Outputs  :  sys_running_state
 *                            
 * HISTORY :
 * ----------  ----  ------------
 * 2024.01.12  Owen  Baseline Version
 *****************************************************************************/
void system_running_state(void)         //在10ms周期任务中循环调度
{
	  static Struct_Motor* motor_ptr = &motor;
		
		/*** 模型代码嵌入部分 ***/
		mod_acc_data_read = readseq_test();
	  mod_bus_volt_read = motor_ptr->app.dcbus_voltage >> 16;
		mod_espd = motor_ptr->mpos.mpos_spd_radps >> 16;
		mod_gear = run_command.Assist_Rate;
	  mod_fault = motor.fault.motor;
	  mod_direction = motor_ptr->app.app_motor_direction;
		Driver_Strategy_step();
//	  Debug_Test_Number1 = mod_acc_data_read;
//	  Debug_Test_Number2 = mod_gear;
//	  Debug_Test_Number3 = mod_fault;
//	  debug_test_number4 = run_command.assist_rate;
		if(mod_motor_enable==1)
		{
			run_command.inv_enable = 1;
		}
		else
		{
			run_command.inv_enable = 0;
		}
		
		if(run_command.inv_enable == 0)
		{
			pid_reset(&motor_ptr->foc.Id_pid);
      pid_reset(&motor_ptr->foc.Iq_pid); 					
		}
		run_command.iq_cmd = mod_iq_cmd;
	  run_command.id_cmd = mod_id_cmd;

		/*** Update Parameters ***/
		motor_ptr->app.motor_enable_cmd = run_command.inv_enable;
		motor_ptr->control.idref_target_f16 = run_command.id_cmd * 65535;
		motor_ptr->control.iqref_target_f16 = run_command.iq_cmd * 65535;
//		motor_ptr->control.idiqref_rate_f16 = (mod_current_rate / CURRENT_SCALE_FACTOR_FLT * FOC_DT_FLT) * 65535;
		
}


/******************************************************************************
 * End of file
 *****************************************************************************/
