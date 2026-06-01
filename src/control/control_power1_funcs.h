/**
 * @file control_power1_funcs.h
 * @brief Use for control power calculation
 */

#ifndef CONTROL_POWER1_FUNCS_H
#define CONTROL_POWER1_FUNCS_H


#include "motor_struct.h"
#include "control_power1_struct.h"



/**
 * @brief This function use to copy current descale app variable to running variable
 * @param Struct_Motor* motor_ptr pointer to motor structure
 */
static inline void power1_calc_param(Struct_Motor *motor_ptr)
{
    motor_ptr->power.current_descale_factor_q16 = motor_ptr->app.descale_current_factor;
}

#endif  //#ifndef CONTROL_POWER1_FUNCS_H
