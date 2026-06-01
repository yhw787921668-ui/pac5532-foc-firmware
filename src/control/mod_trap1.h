
//=============================================================================
// Copyright (C) 2020 - 2023, Qorvo, Inc.
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

#ifndef MOD_TRAP1_H
#define MOD_TRAP1_H

#include "pac5xxx.h"

#include "mod_gen_funcs.h"
#include "foc_math.h"

#if (MODULATION_SELECT == MODULATION_HYBRID_TRAP_DMIN)
    #define MODULATION_INDEX        1.15f
#endif


PAC5XXX_OPTIMIZE_SPEED_ON
static inline void mod_process_trap1(int32_t valpha_q14, int32_t vbeta_q14, int32_t vmag_q14,
        int32_t *mod_sector, int32_t *mod_va_q14, int32_t *mod_vb_q14, int32_t *mod_vc_q14)
{
    fix16_t temp1_q14, temp2_q14, temp3_q14;
    int active_sector;

    temp1_q14 = vbeta_q14;
    temp2_q14 = ((vbeta_q14 >> 1) + ((valpha_q14 * ZERO_POINT_866_IN_Q14)>> 14));    // Q14 + Q14 = Q14, Q14 * Q14 = Q28 >> 14 = Q14
    temp3_q14 = temp2_q14 - temp1_q14;

    // Calculate sector for sector PWM
    active_sector = 3;
    if (temp2_q14 > 0)
        active_sector = active_sector - 1;
    if (temp3_q14 > 0)
        active_sector = active_sector - 1;
    if (temp1_q14 < 0)
        active_sector = 7 - active_sector;

    *mod_sector = active_sector;

    switch (*mod_sector)
    {
    case 1:
        // Float V, LSON W, PWM U
        PAC55XX_GPIOB->OUT.w = 0x00000000;
        PAC55XX_SCC->PBMUXSEL.w =  0x00010001;              // PB0=TAPWM0, PB1=TAPWM1, PB2=TAPWM2, PB4=TAPWM4, PB5=TAPWM5, PB6=TAPWM6
        PAC55XX_GPIOB->OUT.w = 0x04;
        break;
    case 2:
        // Float U, LSON W, PWM V
        PAC55XX_GPIOB->OUT.w = 0x00000000;
        PAC55XX_SCC->PBMUXSEL.w =  0x00100010;              // PB0=TAPWM0, PB1=TAPWM1, PB2=TAPWM2, PB4=TAPWM4, PB5=TAPWM5, PB6=TAPWM6
        PAC55XX_GPIOB->OUT.w = 0x04;
        break;
    case 3:
        // Float W, LSON U, PWM V
        PAC55XX_GPIOB->OUT.w = 0x00000000;
        PAC55XX_SCC->PBMUXSEL.w =  0x00100010;              // PB0=TAPWM0, PB1=TAPWM1, PB2=TAPWM2, PB4=TAPWM4, PB5=TAPWM5, PB6=TAPWM6
        PAC55XX_GPIOB->OUT.w = 0x01;
        break;
    case 4:
        // Float V, LSON U, PWM W
        PAC55XX_GPIOB->OUT.w = 0x00000000;
        PAC55XX_SCC->PBMUXSEL.w =  0x01000100;              // PB0=TAPWM0, PB1=TAPWM1, PB2=TAPWM2, PB4=TAPWM4, PB5=TAPWM5, PB6=TAPWM6
        PAC55XX_GPIOB->OUT.w = 0x01;
        break;
    case 5:
        // Float U, LSON V, PWM W
        PAC55XX_GPIOB->OUT.w = 0x00000000;
        PAC55XX_SCC->PBMUXSEL.w =  0x01000100;              // PB0=TAPWM0, PB1=TAPWM1, PB2=TAPWM2, PB4=TAPWM4, PB5=TAPWM5, PB6=TAPWM6
        PAC55XX_GPIOB->OUT.w = 0x02;
        break;
    case 6:
        // Float W, LSON V, PWM U
        PAC55XX_GPIOB->OUT.w = 0x00000000;
        PAC55XX_SCC->PBMUXSEL.w =  0x00010001;              // PB0=TAPWM0, PB1=TAPWM1, PB2=TAPWM2, PB4=TAPWM4, PB5=TAPWM5, PB6=TAPWM6
        PAC55XX_GPIOB->OUT.w = 0x02;
        break;
    default:
        break;
    }

    *mod_va_q14 = vmag_q14;
    *mod_vb_q14 = vmag_q14;
    *mod_vc_q14 = vmag_q14;
}
PAC5XXX_OPTIMIZE_SPEED_OFF


PAC5XXX_OPTIMIZE_SPEED_ON
static inline void mod_process_hybrid_trap_dmin(int32_t flag_modesel_dmin, int32_t valpha_q14, int32_t vbeta_q14,
        int32_t *mod_sector, int32_t *mod_va_q14, int32_t *mod_vb_q14, int32_t *mod_vc_q14)
{
    fix16_t temp1_q14, temp2_q14, temp3_q14;
    int active_sector;
    int32_t valpha_int_q14, vbeta_int_q14;
    int32_t va_int_q14, vb_int_q14, vc_int_q14;
    int32_t vno_int_q14;
    static int32_t sector_prev;


    temp1_q14 = vbeta_q14;
    temp2_q14 = ((vbeta_q14 >> 1) + ((valpha_q14 * ZERO_POINT_866_IN_Q14)>> 14));    // Q14 + Q14 = Q14, Q14 * Q14 = Q28 >> 14 = Q14
    temp3_q14 = temp2_q14 - temp1_q14;

    // Calculate sector for sector PWM
    active_sector = 3;
    if (temp2_q14 > 0)
        active_sector = active_sector - 1;
    if (temp3_q14 > 0)
        active_sector = active_sector - 1;
    if (temp1_q14 < 0)
        active_sector = 7 - active_sector;

    *mod_sector = active_sector;

    if (active_sector != sector_prev)
    {
        if (flag_modesel_dmin)
        {
            // Consider running this case only once, instead of every sector change
            PAC55XX_GPIOB->OUT.w = 0x00000000;
            PAC55XX_SCC->PBMUXSEL.w =  0x01110111;              // PB0=TAPWM0, PB1=TAPWM1, PB2=TAPWM2, PB4=TAPWM4, PB5=TAPWM5, PB6=TAPWM6
        }
        else
        {
            switch (active_sector)
            {
            case 1:
                // Float V, LSON W, PWM U
                PAC55XX_GPIOB->OUT.w = 0x00000000;
                PAC55XX_SCC->PBMUXSEL.w =  0x00010001;              // PB0=TAPWM0, PB1=TAPWM1, PB2=TAPWM2, PB4=TAPWM4, PB5=TAPWM5, PB6=TAPWM6
                PAC55XX_GPIOB->OUT.w = 0x04;
                break;
            case 2:
                // Float U, LSON W, PWM V
                PAC55XX_GPIOB->OUT.w = 0x00000000;
                PAC55XX_SCC->PBMUXSEL.w =  0x00100010;              // PB0=TAPWM0, PB1=TAPWM1, PB2=TAPWM2, PB4=TAPWM4, PB5=TAPWM5, PB6=TAPWM6
                PAC55XX_GPIOB->OUT.w = 0x04;
                break;
            case 3:
                // Float W, LSON U, PWM V
                PAC55XX_GPIOB->OUT.w = 0x00000000;
                PAC55XX_SCC->PBMUXSEL.w =  0x00100010;              // PB0=TAPWM0, PB1=TAPWM1, PB2=TAPWM2, PB4=TAPWM4, PB5=TAPWM5, PB6=TAPWM6
                PAC55XX_GPIOB->OUT.w = 0x01;
                break;
            case 4:
                // Float V, LSON U, PWM W
                PAC55XX_GPIOB->OUT.w = 0x00000000;
                PAC55XX_SCC->PBMUXSEL.w =  0x01000100;              // PB0=TAPWM0, PB1=TAPWM1, PB2=TAPWM2, PB4=TAPWM4, PB5=TAPWM5, PB6=TAPWM6
                PAC55XX_GPIOB->OUT.w = 0x01;
                break;
            case 5:
                // Float U, LSON V, PWM W
                PAC55XX_GPIOB->OUT.w = 0x00000000;
                PAC55XX_SCC->PBMUXSEL.w =  0x01000100;              // PB0=TAPWM0, PB1=TAPWM1, PB2=TAPWM2, PB4=TAPWM4, PB5=TAPWM5, PB6=TAPWM6
                PAC55XX_GPIOB->OUT.w = 0x02;
                break;
            case 6:
                // Float W, LSON V, PWM U
                PAC55XX_GPIOB->OUT.w = 0x00000000;
                PAC55XX_SCC->PBMUXSEL.w =  0x00010001;              // PB0=TAPWM0, PB1=TAPWM1, PB2=TAPWM2, PB4=TAPWM4, PB5=TAPWM5, PB6=TAPWM6
                PAC55XX_GPIOB->OUT.w = 0x02;
                break;
            default:
                break;
            }
        }

        // Store the previous sector
        sector_prev = active_sector;
    }

    // Scale Valpha/Vbeta up to genmod index
    valpha_int_q14 = ((valpha_q14 * QCONST(MODULATION_INDEX, Q14)) >> 14);      // q14 * q14 = q28>>14 = q14
    vbeta_int_q14 = ((vbeta_q14 * QCONST(MODULATION_INDEX, Q14)) >> 14);        // q14 * q14 = q28>>14 = q14

    // Transform Valpha/Vbeta from 2-coordinate to 3-coordinate system
    inverse_clarke_transform(valpha_int_q14, vbeta_int_q14, &va_int_q14, &vb_int_q14, &vc_int_q14);

    // Zero Sequence Calculator
    mod_gen_zsc_dpwmmin(va_int_q14, vb_int_q14, vc_int_q14, &vno_int_q14);

    // Inject modulation to phase voltage
    *mod_va_q14 = va_int_q14 + vno_int_q14;
    *mod_vb_q14 = vb_int_q14 + vno_int_q14;
    *mod_vc_q14 = vc_int_q14 + vno_int_q14;
}
PAC5XXX_OPTIMIZE_SPEED_OFF

#endif  //#ifndef MOD_TRAP1_H
