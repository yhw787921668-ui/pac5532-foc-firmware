
//=============================================================================
// Copyright (C) 2019 - 2023, Qorvo, Inc.
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

#ifndef QEP_IO_CONFIG_H
#define QEP_IO_CONFIG_H

#include "pac5xxx.h"
#include "config_board.h"
#include "config_app.h"
#include "mpos_struct.h"

#if (MODULE_MPOS_QEP == ENABLED)

#if defined(FAMILY_55XX)
//=====================================
// HALL Input Port Definitions
//=====================================
// get the hall value
#define GET_HALL_VALUE_QEP  ((PAC5XXX_GPIO_QEPPHB->IN.QEPPHB_PORTNUM << 2) + (PAC5XXX_GPIO_QEPPHA->IN.QEPPHA_PORTNUM << 1) + (PAC5XXX_GPIO_QEPIDX->IN.QEPIDX_PORTNUM << 0))

//==========================================
// Hall Control Macro Functions
//==========================================
// HALL Port Controls
static inline void qep_io_config(void)
{
    // Configure ports as inputs
     PAC5XXX_GPIO_QEPPHB->MODE.QEPPHB_PORTNUM = 3;
     PAC5XXX_GPIO_QEPPHA->MODE.QEPPHA_PORTNUM = 3;
     PAC5XXX_GPIO_QEPIDX->MODE.QEPIDX_PORTNUM = 3;

#if (QEP_PORT == QEP_PORT_C)
    // Select GPIO peripheral MUX
    PAC55XX_SCC->PCMUXSEL.QEPPHB_PORTNUM = 3;
    PAC55XX_SCC->PCMUXSEL.QEPPHA_PORTNUM = 3;
    PAC55XX_SCC->PCMUXSEL.QEPIDX_PORTNUM = 3;

     // Disable pullups
    PAC55XX_SCC->PCPUEN.QEPPHB_PORTNUM = 0;
    PAC55XX_SCC->PCPUEN.QEPPHA_PORTNUM = 0;
    PAC55XX_SCC->PCPUEN.QEPIDX_PORTNUM = 0;

    // Disable pulldowns
    PAC55XX_SCC->PCPDEN.QEPPHB_PORTNUM = 0;
    PAC55XX_SCC->PCPDEN.QEPPHA_PORTNUM = 0;
    PAC55XX_SCC->PCPDEN.QEPIDX_PORTNUM = 0;
#elif (QEP_PORT == QEP_PORT_D)
    // Select GPIO peripheral MUX
    PAC55XX_SCC->PDMUXSEL.QEPPHB_PORTNUM = 3;
    PAC55XX_SCC->PDMUXSEL.QEPPHA_PORTNUM = 3;
    PAC55XX_SCC->PDMUXSEL.QEPIDX_PORTNUM = 3;

     // Disable pullups
    PAC55XX_SCC->PDPUEN.QEPPHB_PORTNUM = 0;
    PAC55XX_SCC->PDPUEN.QEPPHA_PORTNUM = 0;
    PAC55XX_SCC->PDPUEN.QEPIDX_PORTNUM = 0;

    // Disable pulldowns
    PAC55XX_SCC->PDPDEN.QEPPHB_PORTNUM = 0;
    PAC55XX_SCC->PDPDEN.QEPPHA_PORTNUM = 0;
    PAC55XX_SCC->PDPDEN.QEPIDX_PORTNUM = 0;
#endif
}
#endif  //#if defined(FAMILY_55XX)

#endif  //#if (MODULE_MPOS_QEP == ENABLED)

#endif  //QEP_IO_CONFIG_H
