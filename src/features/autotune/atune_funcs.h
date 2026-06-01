/**
 * @file atune_funcs.h
 * @brief Use for auto-tuning functions definition
 */

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

#ifndef AUTOTUNE_PI_FUNCS_H
#define AUTOTUNE_PI_FUNCS_H

#include "motor_struct.h"
#include "param_funcs.h"
#include "fault.h"
#include "mid1_funcs.h"
#include "mid2_funcs.h"

#if defined(ENABLE_AUTOTUNE_PI)

// ATPI Functions
/**
 * @brief Reset auto-tuning PI gains
 * @param Struct_Motor *motor_ptr
 */
void atpi_reset_vars(Struct_Motor *motor_ptr);

/**
 * @brief Use to check if parameters are changed
 * @param Struct_Motor *motor_ptr
 * @param ParameterCode param_code
 * @param int32_t rx_data
 */
void atpi_paramchange_check(Struct_Motor *motor_ptr, ParameterCode param_code, int32_t rx_data);

/**
 * @brief Use to regulate q-axis current reference
 * @param Struct_Motor *motor_ptr
 */
#ifdef ENABLE_AUTOTUNE_REGULATE_IQREF
extern void regulate_iqref(Struct_Motor* motor_ptr);
#endif


// AT General Functions
/**
 * @brief Use to calculate parameters
 * @param Struct_Motor *motor_ptr
 */
void at_calc_param(Struct_Motor *motor_ptr);

/**
 * @brief Use to reset states
 * @param Struct_Motor *motor_ptr
 */
void at_reset_state(Struct_Motor *motor_ptr);

/**
 * @brief Use to write parameters
 * @param Struct_Motor *motor_ptr
 * @param ParameterCode param_code
 * @param int32_t rx_data
 */
void at_param_write(Struct_Motor *motor_ptr, ParameterCode param_code, int32_t rx_data);

/**
 * @brief Use to process parameter calculation request
 * @param Struct_Motor *motor_ptr
 * @param ParameterCode param_code
 * @param int32_t rx_data
 */
void at_param_process(Struct_Motor *motor_ptr, ParameterCode param_code, int32_t rx_data);


// UI Functions
/**
 * @brief Use to calculate current loop gain from UI cmd
 * @param Struct_Motor *motor_ptr
 */
static inline void at_uicmd_calcgain_current(Struct_Motor *motor_ptr)
{
    if (motor_ptr->atune.uicmd_atune == UICMD_ATUNE_NEXTCMD)
    {
        motor_ptr->atune.uicmd_atune = UICMD_ATUNE_CALCGAIN_CURRENT;
    }
}

/**
 * @brief Use to calculate speed loop gain from UI cmd
 * @param Struct_Motor *motor_ptr
 */
static inline void at_uicmd_calcgain_speed(Struct_Motor *motor_ptr)
{
    if (motor_ptr->atune.uicmd_atune == UICMD_ATUNE_NEXTCMD)
    {
        motor_ptr->atune.uicmd_atune = UICMD_ATUNE_CALCGAIN_SPEED;
    }
}

/**
 * @brief Use to reset UI cmd
 * @param Struct_Motor *motor_ptr
 */
static inline void at_uicmd_reset(Struct_Motor *motor_ptr)
{
    if (motor_ptr->atune.uicmd_atune == UICMD_ATUNE_NEXTCMD)
    {
        motor_ptr->atune.uicmd_atune = UICMD_ATUNE_RESET;
    }
}

/**
 * @brief Use to stop UI cmd
 * @param Struct_Motor *motor_ptr
 */
static inline void at_uicmd_stop(Struct_Motor *motor_ptr)
{
    if (motor_ptr->atune.uicmd_atune == UICMD_ATUNE_NEXTCMD)
    {
        motor_ptr->atune.uicmd_atune = UICMD_ATUNE_MID2_STOP;
    }
}

/**
 * @brief Use to set the current step reference from UI cmd
 * @param Struct_Motor *motor_ptr
 */
static inline void at_uicmd_istep(Struct_Motor *motor_ptr)
{
    if (motor_ptr->atune.uicmd_atune == UICMD_ATUNE_NEXTCMD)
    {
        motor_ptr->atune.uicmd_atune = UICMD_ATUNE_ISTEP;
    }
}

/**
 * @brief Use to calculate motor identification from UI cmd
 * @param Struct_Motor *motor_ptr
 */
static inline void at_uicmd_calcmid(Struct_Motor *motor_ptr)
{
    if (motor_ptr->atune.uicmd_atune == UICMD_ATUNE_NEXTCMD)
    {
        motor_ptr->atune.uicmd_atune = UICMD_ATUNE_CALCMID;
    }
}

/**
 * @brief Use to update the status of UI
 * @param Struct_Motor *motor_ptr
 */
static inline void at_uistatus(Struct_Motor *motor_ptr)
{
    // Report ATPI status needs to be checked
    if (motor_ptr->atune.status_atune & STATUS_ATUNE_ALLWARNINGS_MAINSTAT)
        set_warning(WARNING_ATUNE_CHECK_STATUS);
    else
        clear_warning(WARNING_ATUNE_CHECK_STATUS);
}

/**
 * @brief Use to clear the status on GUI
 * @param Struct_Motor *motor_ptr
 */
static inline void at_status_clear_on_guiread(Struct_Motor *motor_ptr)
{
    motor_ptr->atune.status_atune &= ~(FLAG_ATUNE_ATPI_UPDATE_I | FLAG_ATUNE_ATPI_UPDATE_SPD | FLAG_ATUNE_MID2_UPDATE_CALC | FLAG_ATUNE_MID1_UPDATE_CALC);
}

/**
 * @brief Process the systick timelevel functions for auto pi gains
 * @param Struct_Motor *motor_ptr
 */
static inline void atpi_process_systick(Struct_Motor *motor_ptr)
{
    if (motor_ptr->state.main != STATE_MAIN_MID1)
    {
        // ATPI Controller
        atpi_controller(&motor_ptr->atune, &motor_ptr->control, &motor_ptr->app, &motor_ptr->mparam);
    }
}    
#endif  //#if defined(ENABLE_AUTOTUNE_PI)
#endif  //#ifndef AUTOTUNE_PI_FUNCS_H



