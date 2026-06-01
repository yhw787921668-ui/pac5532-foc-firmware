
/**
 * @file main.c
 * @brief Use for motor control hardware and software initialization
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
#include "motor_struct.h"
#include "app_funcs.h"
#include "pac_init.h"
#include "fault.h"
#include "fault_struct.h"
#include "pac5xxx_driver_adc.h"
#include "pac5xxx_driver_uart.h"
#include "VSM_control.h"  //add  by:owz
#include "Driver_Strategy.h"      //add  by:owz
#include "usercom.h"              //add  by:owz

//新增变量   by:owz
uint8_t tx_dataa[8] =  {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
fix16_t Vehicle_Speed = 0;
fix16_t Bus_Current = 0;
uint8_t MCU_CRC = 0;
uint8_t  task_1ms_timer_f;
uint8_t  task_10ms_timer_f;
uint8_t  task_100ms_timer_f;
uint8_t  task_500ms_timer_f;
int Debug_Test_Number1;
int Debug_Test_Number2;
int Debug_Test_Number3;
int Debug_Test_Number4;
extern uint8_t calcCRC8(uint8_t pBuf[], uint8_t len);
uint8_t tmp_ct=0,tmp_ct2=0,tmp_ct3=0;
uint16_t time_counter=0,time_counter1=0,tmp_ct4=0,tmp_ct5=0;

// UART 轮询接收缓冲区
uint8_t uart_rx_buf[9];  // CMD(1B) + data[8B] = 9字节
uint8_t uart_rx_idx = 0;
uint8_t uart_rx_sync = 0;  // 0=等待帧头, 1=接收中
MotorUser motoruser;  // usercom.c 未加入工程, 在此定义

// ── 心跳检测 ──
#define HEARTBEAT_TIMEOUT_TICKS  30   // 30 * 100ms = 3秒无心跳判定上位机断开
uint8_t  heartbeat_tick = 0;         // 心跳超时计数 (每100ms+1)
uint8_t  pc_connected = 0;           // 上位机连接状态: 0=断开, 1=已连接


#ifdef ENABLE_DEBUG_RTT
#include "USER_RTT.h"
#endif
/**
 * @brief This function use for initializing hardware and motor control FOC firmware
 */
int main(void)
{
    // Disable global interrupts until initialization is complete
    __disable_irq();
     
    // Set pointers to data structure for current motor
    Struct_Motor* motor_ptr = &motor;
    
#ifdef ENABLE_DEBUG_RTT
    SEGGER_RTT_Init();
#endif
    // Initialize main - FOC state machines and system variables
    systemvar_init(motor_ptr);

    // Initialize all MCU peripherals
    peripheral_init();

    // Initialize CAFE
    cafe_init();

    // Enable ADC. Automated sequences will begin
    pac5xxx_start_adc_sequencer();
	
	  system_running_init();     //add by:owz
             
    // Enable global interrupts
    __enable_irq();

    // All the processing is interrupt driven
    while(1)
    {
        /*** UART轮询接收：非阻塞读取上位机下发帧 ***/
        // 帧格式: [0xAA][CMD][8字节] = 10字节
        // 握手帧: CMD=0xA0, 收到后回复 [0xAA][0xA0]["OK\0\0\0\0\0\0"]
        // 下发帧: CMD=0xA1, data[0]低4位=辅助档位
        // 用 LSR.RDR (bit0) 检测RX数据，不依赖中断系统 (参考V1已验证方案)
        {
            uint8_t max_poll = 32;  // 单次最多轮询字节数
            while (max_poll-- && PAC55XX_UARTC->LSR.RDR)
            {
                uint8_t ch = pac5xxx_uart_read2(PAC55XX_UARTC);
                if (uart_rx_sync == 0)
                {
                    if (ch == 0xAA)
                    {
                        uart_rx_sync = 1;
                        uart_rx_idx = 0;
                    }
                }
                else
                {
                    uart_rx_buf[uart_rx_idx++] = ch;
                    if (uart_rx_idx >= 9)  // CMD + 8字节数据 = 9字节(不含帧头)
                    {
                        uart_rx_sync = 0;
                        if (uart_rx_buf[0] == 0xA0)  // 握手帧
                        {
                            // 回复 [0xAA][0xA0]["OK\0\0\0\0\0\0"]
                            uint8_t handshake_rsp[] = {0xAA, 0xA0, 'O', 'K', 0, 0, 0, 0, 0, 0};
                            for (int i = 0; i < 10; i++)
                            {
                                pac5xxx_uart_write2(PAC55XX_UARTC, handshake_rsp[i]);
                                while (!(PAC55XX_UARTC->LSR.THRE));
                            }
                        }
                        else if (uart_rx_buf[0] == 0xA1)  // 档位下发帧
                        {
                            motoruser.Assist_Rate = uart_rx_buf[1] & 0x0F;
                        }
                        else if (uart_rx_buf[0] == 0xA2)  // 心跳帧 (PC→MCU)
                        {
                            // 收到心跳，重置超时计数
                            heartbeat_tick = 0;
                            pc_connected = 1;
                            // 回复心跳: [0xAA][0xA2][0,0,0,0,0,0,0,0]
                            uint8_t hb_rsp[] = {0xAA, 0xA2, 0, 0, 0, 0, 0, 0, 0, 0};
                            for (int i = 0; i < 10; i++)
                            {
                                pac5xxx_uart_write2(PAC55XX_UARTC, hb_rsp[i]);
                                while (!(PAC55XX_UARTC->LSR.THRE));
                            }
                        }
                    }
                }
            }
        }

        /*** 100ms任务 add by:owz ***/
        if (task_100ms_timer_f == 1)  // 100ms发一次
        {
            task_100ms_timer_f = 0;

            // ── 心跳超时检测 ──
            heartbeat_tick++;
            if (heartbeat_tick >= HEARTBEAT_TIMEOUT_TICKS)
            {
                heartbeat_tick = HEARTBEAT_TIMEOUT_TICKS;  // 防止溢出
                pc_connected = 0;  // 上位机断开
            }

            // 握手帧: [0xAA][0xA0]["HI"] <-> [0xAA][0xA0]["OK"]
            // 数据帧: [0xAA][0xC1][8字节]

            // ID=0xC1
            if (task_500ms_timer_f == 1)
            {
                task_500ms_timer_f = 0;
                Vehicle_Speed = (fix16_mul(motor_ptr->mpos.hall.hall_spd_radps,685)>>13)*1.14231; // M车型：1.04712  Y车型：1.14231
                if (Vehicle_Speed <= 10 || motor_ptr->app.app_motor_direction == -1)
                {
                    Vehicle_Speed = 0;
                }
            }

            Bus_Current = ((fix16_sqrt((motor_ptr->foc.id_prefilter_q14 * motor_ptr->foc.id_prefilter_q14 >>12) + (motor_ptr->foc.iq_prefilter_q14 * motor_ptr->foc.iq_prefilter_q14 >>12)))>>13)*19.95;
            if (Bus_Current <= 0)
            {
                Bus_Current = 0;
            }

            // data[0:1] 车速 (u16)
            tx_dataa[0] = Vehicle_Speed & 0xFF;
            tx_dataa[1] = (Vehicle_Speed >> 8) & 0xFF;

            // data[2:3] 母线电流 (i16)
            tx_dataa[2] = Bus_Current & 0xFF;
            tx_dataa[3] = (Bus_Current >> 8) & 0xFF;

            // data[4] 母线电压 (0.2V/LSB)
            tx_dataa[4] = (uint8_t)(((uint32_t)motor_ptr->app.dcbus_voltage * 5) >> 16);

            // data[5] fault_byte0: 8项主要故障
            {
                uint32_t fault_motor = motor.fault.motor;
                uint8_t fb0 = 0;
                if (fault_motor & FAULT_TERMINAL)               fb0 |= (1 << 0);  // 终端故障
                if (fault_motor & FAULT_CAFE_OC)                fb0 |= (1 << 1);  // 过流故障
                if (fault_motor & FAULT_OU_VOLTAGE)             fb0 |= (1 << 2);  // 电压故障
                if (fault_motor & FAULT_MOTOR_STALLED)          fb0 |= (1 << 3);  // 堵转故障
                if (fault_motor & FAULT_OPD)                    fb0 |= (1 << 4);  // 缺相故障
                if (fault_motor & FAULT_MOTOR_HALL_INVSTATE)    fb0 |= (1 << 5);  // 霍尔异常
                if (fault_motor & FAULT_HALL_TIMEOUT)           fb0 |= (1 << 6);  // 霍尔超时
                if (mod_init_false == 1)                        fb0 |= (1 << 7);  // 初始化失败
                tx_dataa[5] = fb0;
            }

            // data[6] fault_byte1: 8项次要故障+警告
            {
                uint32_t fault_motor = motor.fault.motor;
                uint8_t fb1 = 0;
                if (fault_motor & FAULT_SPDANGLDIF_TIMEOUT)     fb1 |= (1 << 0);  // 速度角度超时
                if (fault_motor & FAULT_COAST_TIMEOUT)          fb1 |= (1 << 1);  // 滑行超时
                if (fault_motor & FAULT_MIN_MAX_SPEED)          fb1 |= (1 << 2);  // 转速越限
                if (fault_motor & FAULT_PARAM_WHILE_RUN)        fb1 |= (1 << 3);  // 参数变更
                if (fault_motor & FAULT_PPM_PULSE_TIMEOUT)      fb1 |= (1 << 4);  // PPM超时
                if (fault_motor & FAULT_EST_INVALID_INPUT)      fb1 |= (1 << 5);  // 估算器异常
                if (fault_motor & WARNING_OU_VOLTAGE)           fb1 |= (1 << 6);  // 电压预警
                if (fault_motor & WARNING_FOCCTRL_OVERFLOW)     fb1 |= (1 << 7);  // FOC超时
                tx_dataa[6] = fb1;
            }

            // data[7] bit6:0=CRC8(覆盖data[0..6]), bit7=PC连接状态(1=已连接)
            MCU_CRC = calcCRC8(tx_dataa, 7);
            tx_dataa[7] = (MCU_CRC & 0x7F) | (pc_connected ? 0x80 : 0x00);

            /*** UART轮询发送 ***/
            // 发送帧头
            pac5xxx_uart_write2(PAC55XX_UARTC, 0xAA);
            while (!(PAC55XX_UARTC->LSR.THRE));
            // 发送CMD
            pac5xxx_uart_write2(PAC55XX_UARTC, 0xC1);
            while (!(PAC55XX_UARTC->LSR.THRE));
            // 发送8字节数据
            for (int i = 0; i < 8; i++)
            {
                pac5xxx_uart_write2(PAC55XX_UARTC, tx_dataa[i]);
                while (!(PAC55XX_UARTC->LSR.THRE));
            }

            PAC55XX_GPIOF->DOCLEAR.P4 = 1;
        }
    }
}

