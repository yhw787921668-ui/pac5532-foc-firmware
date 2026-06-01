
/**
 * @file comm_cmd.h
 * @brief Use for comm command processing
 */

//=============================================================================
// Copyright (C) 2017 - 2020, Qorvo, Inc.
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

#ifndef COMM_CMD_H
#define COMM_CMD_H

#include "motor_struct.h"
#include "param_funcs.h"
#include "project_version.h"
#include "fault.h"


/**
 * @brief This function use to calculate checksum for given buffer
 * @param uint8_t* pBuffer buffer pointer
 */
uint8_t uart_checksum(uint8_t* pBuffer, uint32_t bytesnum);


static inline void comm_cmd(Struct_Motor *motor_ptr)
{
    switch (motor_ptr->comm.cmd_code)              // Switch on command code
    {
    case cmd_version:
#if (PROJECT_VERSION_RELEASE_STATE != PAC5XXX_IN_DEVELOPMENT)
        motor_ptr->comm.tx_data =
                PROJECT_VERSION_RELEASE_STATE << 24 |
                PROJECT_VERSION_MAJOR << 16 |
                PROJECT_VERSION_MINOR << 8 |
                PROJECT_VERSION_BUGFIX;
#else //(PROJECT_VERSION_RELEASE_STATE == PAC5XXX_IN_DEVELOPMENT)
        motor_ptr->comm.tx_data =
                PROJECT_VERSION_RELEASE_STATE << 24 |
                ((PROJECT_BUILD_DATE_NUM / 10000) % 100) << 16 |
                ((PROJECT_BUILD_DATE_NUM / 100) % 100) << 8 |
                (PROJECT_BUILD_DATE_NUM % 100);
#endif
        break;

    case cmd_motor_control_read:
        motor_ptr->comm.tx_data = 0;

        if ((motor_ptr->status.motor & STATUS_MOTOR_ENABLED) != 0)
            motor_ptr->comm.tx_data = 1;

        if ((motor_ptr->status.motor & STATUS_MOTOR_OPEN_LOOP) != 0)
            motor_ptr->comm.tx_data |= 2;
        break;

    case cmd_motor_control_write:
        if (motor_ptr->comm.rx_data & 8)
        {
            oc_reset();
        }
        if (motor_ptr->comm.rx_data & 1) // enable motor
        {
            if (motor_ptr->app.motor_enable_cmd == 0)
            {
                motor_ptr->app.motor_enable_cmd = 1;
            }
        }
        else
        {
            motor_ptr->app.motor_enable_cmd = 0;
        }
        break;

    case cmd_cafe_reg_read:
        motor_ptr->comm.tx_data = (motor_ptr->comm.rx_data & 0xFF);
        break;

    case cmd_page_read:
        motor_ptr->comm.tx_data = 0;
        break;

    case cmd_page_write:
        break;

    case cmd_parameter_read:
        param_read(motor_ptr->comm.param_code, motor_ptr);
        break;

    case cmd_parameter_write:
#if defined(ENABLE_TM) && defined(TM_PARAMCHECK)
        if (motor_ptr->debug.testmode_enable)
            parameter_check((ParameterCode)motor_ptr->comm.param_code, motor_ptr->comm.rx_data, motor_ptr);
        else
            param_write((ParameterCode)motor_ptr->comm.param_code, motor_ptr->comm.rx_data, motor_ptr);
#else
        param_write((ParameterCode)motor_ptr->comm.param_code, motor_ptr->comm.rx_data, motor_ptr);
#endif
        break;

    default:
        motor_ptr->comm.status = host_write_error;
        break;
    }
}

#endif  //#ifndef COMM_CMD_H
