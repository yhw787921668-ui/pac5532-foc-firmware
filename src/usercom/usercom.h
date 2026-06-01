#ifndef _USERCOM_H_
#define _USERCOM_H_

#include "fix16.h"

#define  _q16(A)   (long) ((A) * 65536.0L)
#define  _q7(A)   (int16_t) ((A) * 128.0L) 
#define  q16                       65536

typedef int16_t fix7_t; 


typedef enum{
	
	 run_hall,
	 run_hall2est,
	 run_est

}RUN_MODE;


typedef struct{
	
	fix16_t cmd_torque;      //用户命令转矩(N·M)     
	fix16_t cmd_torque_max_limit;
	
	fix16_t cmd_speed;       //用户命令转速(RPM)
	fix16_t lookup_id;       //查表得到的d轴电流
	fix16_t lookup_iq;       //查表得到的q轴电流
	
	fix16_t user_debug;      //调试用参数
	fix16_t fback_speed_rpm; //反馈速度 rpm
	uint8_t coasting_flag;   //惯性滑行标志      0：非滑行  1：滑行中
	
	uint8_t motor_sm_run;    //电机运行与否标志  0：非运行  1：运行中
	uint8_t safe_signal;     //电机防盗信号      0：未触发  1: 触发防盗
	uint8_t hall_damage;     //霍尔损坏标志      0：正常    1: 霍尔故障
	
	uint8_t Assist_Rate;     //助力比
	
	uint32_t temp_hall;
	
	RUN_MODE run_mode;       //电机启动方式     霍尔启动切无感   无感启动
		
	
}MotorUser;


extern MotorUser motoruser;


void user_call(void);       //用户循环调用
void user_param_set(void);  //用户参数初始化

typedef struct
{
 uint16_t bit0:1;
 uint16_t bit1:1;
 uint16_t bit2:1;
 uint16_t bit3:1;
 uint16_t bit4:1;
 uint16_t bit5:1;
 uint16_t bit6:1;
 uint16_t bit7:1;
 uint16_t bit8:1;
 uint16_t bit9:1;
 uint16_t bit10:1;
 uint16_t bit11:1;
 uint16_t bit12:1;
 uint16_t bit13:1;
 uint16_t bit14:1;
 uint16_t bit15:1;
} uint16_bits;

typedef struct
{
 uint16_t byte0:8;
 uint16_t byte1:8;
} uint16_byte;

typedef union
{
 uint16_bits bit;
 uint16_byte byte;
 uint16_t    all;
} uint16_union;



#endif    
