
/**
 * @file startup_hall_struct.h
 * @brief Use for hallstartup enum and struct
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

#ifndef STARTUP_HALL_STRUCT_H
#define STARTUP_HALL_STRUCT_H


/**
 * @brief eStateHallStartMain enum
 */
typedef enum
{
    STATE_HALLSTART_MAIN_RESET = 0,                 // 0: Hall Startup Controller State - Reset
    STATE_HALLSTART_MAIN_START,                     // 1: Hall Startup Controller State - Start motor in hall startup mode
    STATE_HALLSTART_MAIN_COMPLETE                   // 2: Hall Startup Controller State - Hall startup complete
} eStateHallStartMain;


/**
 * @brief Struct_StartupHall struct
 */
typedef struct
{
    // State & Status
    eStateHallStartMain state_main;                 // Hall Startup Controller State

    // Running Variables
    int32_t i_total_hall_target_f16;                // Total startup current during hall startup
    int32_t idrefstart_target_q16;                  // Idref during hall startup
    int32_t iqrefstart_target_q16;                  // Iqref during hall startup

    // Configuration
    int32_t i_total_hall_gui;                       // Store total hall startup current GUI command
}Struct_StartupHall;

#endif //#ifndef STARTUP_HALL_STRUCT_H
