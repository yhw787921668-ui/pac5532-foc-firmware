
/**
 * @file control_power1.h
 * @brief Use for control power functions
 */

#ifndef CONTROL_POWER1_H
#define CONTROL_POWER1_H

#include "pac5xxx.h"
#include "maths.h"
#include "control_struct.h"
#include "control_power1_struct.h"


// Do not change!!! If changed control library needs to be compiled.
// Power Calculation Options:
// ---------------------------------------------
//#define POWER_DTCOMP                                    // Enable deadtime compensation used in the phase voltage calculation
//#define POWER_TRIANGULATE_CURRENT                       // Use the average of the new and previous current phase current samples in order to better represent the phase current value over the control cycle.
//#define POWER_TRIANGULATE_VOLTAGE                       // Use the average of the new and previous current phase voltage samples in order to better represent the phase voltage value over the control cycle.
#define SELECT_CENTERTAP_REF


#if defined(POWER_DTCOMP)
PAC5XXX_RAMFUNC void calc_powermeas_dtcomp(Struct_Power *power_ptr, Struct_Control *control_ptr);
#endif

PAC5XXX_RAMFUNC void power1_calc_instpwr(Struct_Power *power_ptr, Struct_Control *control_ptr);

#endif  //#ifndef CONTROL_POWER1_H
