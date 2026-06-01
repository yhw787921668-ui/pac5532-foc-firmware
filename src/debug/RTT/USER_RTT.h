
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

#ifndef DEBUG_RTT_H
#define DEBUG_RTT_H

#include "motor_struct.h"
#include "pac5xxx.h"
#include "config_debug.h"
#include "config_app.h"
#include "board_options.h"
#include "debug_dac.h"
#include "debug_io.h"

#include "SEGGER_RTT_Conf.h"
#include "SEGGER_RTT.h"


PAC5XXX_RAMFUNC static inline void run_debug_rtt(void)
{
    Struct_Motor* motor_ptr = &motor;

#ifdef SEGGER_RTT_USED
    // Calculations for Debug DAC's
#if (MPOS_SELECT == MPOS_LBG1_START_HALL)
    motor_ptr->debug.angle_q14 = motor_ptr->mpos.hall.hall_interpolated_angle >> 5;
    motor_ptr->debug.estimated_angle_q14 = motor_ptr->mpos.est.estimated_angle >> 5;
#elif (MPOS_SELECT == MPOS_QEP_START_ALIGNGO)
    motor_ptr->debug.angle_q14 = motor_ptr->mpos.mpos_angle >> 5;
    motor_ptr->debug.estimated_angle_q14 = motor_ptr->mpos.qep.qep_angle >> 5;
#elif (MPOS_SELECT == MPOS_HALL_ONLY)
    motor_ptr->debug.angle_q14 = motor_ptr->mpos.hall.hall_interpolated_angle >> 5;
    motor_ptr->debug.estimated_angle_q14 = motor_ptr->debug.angle_q14;
#elif (MPOS_SELECT == MPOS_LBG1_START_ALIGNGO)
    motor_ptr->debug.angle_q14 = motor_ptr->mpos.mpos_angle >> 5;
    motor_ptr->debug.estimated_angle_q14 = motor_ptr->mpos.est.estimated_angle >> 5;
#endif  
    
    {
        unsigned wr_off;
        SEGGER_RTT_CB * p;  
        p = &_SEGGER_RTT;

        wr_off = (p->aUp[0].WrOff + RTT_RAMP_COUNTER);
        if(wr_off == RTT_UP_BUFFER)wr_off = 0;
        

        if (wr_off != p->aUp[0].RdOff)
        {
             int16_t * pData;
             pData = (int16_t *)(p->aUp[0].pBuffer + p->aUp[0].WrOff);
            
            
            *pData++ = (int16_t)(*dac_ptr1);    
            *pData++ = (int16_t)(*dac_ptr2);
            *pData++ = (int16_t)(*dac_ptr3);    
            *pData++ = (int16_t)(*dac_ptr4);
            p->aUp[0].WrOff = wr_off;
        }
    }
#endif
}

#endif  //#ifndef DEBUG_RTT_H

