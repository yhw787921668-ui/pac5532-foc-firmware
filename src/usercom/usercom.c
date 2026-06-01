/******************************************************************************
 * FILE NAME    :  usercom.c
 ******************************************************************************
 * PROJECT NAME :  DK03
 *
 * DESCRIPTION  :  user_call()  user_param_set()
 * 
 * ORIGINATOR   :  TATC
 *
 * AUTHORS      :  Ou Wenzhong
 *
 * CREATED ON   :  2024
 *
 * HISTORY:
 * DATE         BY    DESCRIPTION
 * ----------   ---   -----------
 * 2024         owz        /
 *****************************************************************************/

#include "usercom.h"
#include "motor_struct.h"
#include "fault.h"
#include "hall_io_config.h"
#include "pac5xxx_driver_uart.h"
#include "general_headers.h"

extern uint8_t user_vsm_start_mode;
extern uint32_t limit_spd_rpm;

MotorUser motoruser; 
LIMIT_CURRENT limit_current;

/*******************************
//需要一直运行的用户参数设置
********************************/
void user_call() 
{
	 Struct_Motor* motor_ptr = &motor;

	 motor_ptr->app.speed_mode_flag  =run_command.run_mode;     // 整车信号->控制器端  电机运行模式：速度和扭矩模式。

	 motor_ptr->app.motor_enable_cmd = run_command.inv_enable;  // 电机使能控制
	
	 motor_ptr->app.iq_cmd_q0 = run_command.iq_cmd;

}


/*******************************
//只需要初始化的用户参数
********************************/
void user_param_set() //需要初始化的用户参数，覆盖了param_user_1.h的参数设置。
{
	Struct_Motor* motor_ptr = &motor;

	motor_ptr->app.speed_mode_flag=0;         //0为扭矩模式，1为转速模式

	motor_ptr->app.direction_reverse_cmd=0;   //控制正反转，0为正转，1为反转。

	motoruser.cmd_torque_max_limit=20;
	
	run_command.inv_enable = 0;               //电机使能控制
	
	run_command.id_cmd = 0;
	
	run_command.iq_cmd = 0;
	
	run_command.kp = 0;
	
	run_command.ki = 0;

  motoruser.hall_damage=0; //xuan添加
 //用户直接初始化以下无感参数
		
 //对齐时间
	motor_ptr->startup.aligngo.aligntime_cmd = 6553;
 //开环目标速度 HZ
	motor_ptr->startup.aligngo.speedrefol_target = ((20<<16)); //要小于切换速度
 //开环目标速度 rad/s
	motor_ptr->startup.aligngo.speedrefol_target_radps = (((20<<16) >> 9) * (motor_ptr->control.dt_control_1div_q6 >> 7));
 //开环升速斜率
 //	motor_ptr->startup.aligngo.speedrefol_rate=40; //xuan 屏蔽。此参数对无感启动来说很重要 (理论上按照V/F控制 升速斜率越小越好 但是会导致开环启动太慢 影响驾驶体验 因此需要权衡到一个合适的值)
}
