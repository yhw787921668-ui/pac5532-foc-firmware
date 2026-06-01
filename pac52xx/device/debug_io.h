
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

#ifndef DEBUG_IO_H
#define DEBUG_IO_H

#include "board.h"
#include "config_debug.h"

#ifdef ENABLE_DEBUG_IO

#if (DEBUGIO1_PORT == PORT_PE0)
#define DEBUGIO1_PORTNAME               PAC5XXX_GPIOE                   // Debug IO #1 port name
#define DEBUGIO1_PORTNUM                P0                              // Debug IO #1 port number
#elif (DEBUGIO1_PORT == PORT_PE1)
#define DEBUGIO1_PORTNAME               PAC5XXX_GPIOE                   // Debug IO #1 port name
#define DEBUGIO1_PORTNUM                P1                              // Debug IO #1 port number
#elif (DEBUGIO1_PORT == PORT_PE2)
#define DEBUGIO1_PORTNAME               PAC5XXX_GPIOE                   // Debug IO #1 port name
#define DEBUGIO1_PORTNUM                P2                              // Debug IO #1 port number
#elif (DEBUGIO1_PORT == PORT_PE3)
#define DEBUGIO1_PORTNAME               PAC5XXX_GPIOE                   // Debug IO #1 port name
#define DEBUGIO1_PORTNUM                P3                              // Debug IO #1 port number

#ifndef ENABLE_DEBUG_DAS
#elif (DEBUGIO1_PORT == PORT_PE4)
#define DEBUGIO1_PORTNAME               PAC5XXX_GPIOE                   // Debug IO #1 port name
#define DEBUGIO1_PORTNUM                P4                              // Debug IO #1 port number
#endif

#elif (DEBUGIO1_PORT == PORT_PE5)
#define DEBUGIO1_PORTNAME               PAC5XXX_GPIOE                   // Debug IO #1 port name
#define DEBUGIO1_PORTNUM                P5                              // Debug IO #1 port number
#endif

#if (DEBUGIO2_PORT == PORT_PE0)
#define DEBUGIO2_PORTNAME               PAC5XXX_GPIOE                   // Debug IO #1 port name
#define DEBUGIO2_PORTNUM                P0                              // Debug IO #1 port number
#elif (DEBUGIO2_PORT == PORT_PE1)
#define DEBUGIO2_PORTNAME               PAC5XXX_GPIOE                   // Debug IO #1 port name
#define DEBUGIO2_PORTNUM                P1                              // Debug IO #1 port number
#elif (DEBUGIO2_PORT == PORT_PE2)
#define DEBUGIO2_PORTNAME               PAC5XXX_GPIOE                   // Debug IO #1 port name
#define DEBUGIO2_PORTNUM                P2                              // Debug IO #1 port number
#elif (DEBUGIO2_PORT == PORT_PE3)
#define DEBUGIO2_PORTNAME               PAC5XXX_GPIOE                   // Debug IO #1 port name
#define DEBUGIO2_PORTNUM                P3                              // Debug IO #1 port number

#ifndef ENABLE_DEBUG_DAS
#elif (DEBUGIO2_PORT == PORT_PE4)
#define DEBUGIO2_PORTNAME               PAC5XXX_GPIOE                   // Debug IO #1 port name
#define DEBUGIO2_PORTNUM                P4                              // Debug IO #1 port number
#endif

#elif (DEBUGIO2_PORT == PORT_PE5)
#define DEBUGIO2_PORTNAME               PAC5XXX_GPIOE                   // Debug IO #1 port name
#define DEBUGIO2_PORTNUM                P5                              // Debug IO #1 port number
#endif

static inline void config_debugio(void)     { DEBUGIO1_PORTNAME->OUTEN.DEBUGIO1_PORTNUM |= 1; DEBUGIO2_PORTNAME->OUTEN.DEBUGIO2_PORTNUM |= 1;}
PAC5XXX_RAMFUNC static inline void debugio1_on(void)        { DEBUGIO1_PORTNAME->OUT.DEBUGIO1_PORTNUM |= 1; }
PAC5XXX_RAMFUNC static inline void debugio1_off(void)       { DEBUGIO1_PORTNAME->OUT.DEBUGIO1_PORTNUM &= ~1; }
PAC5XXX_RAMFUNC static inline void debugio1_toggle(void)    { DEBUGIO1_PORTNAME->OUT.DEBUGIO1_PORTNUM ^= 1; }
PAC5XXX_RAMFUNC static inline void debugio2_on(void)        { DEBUGIO2_PORTNAME->OUT.DEBUGIO2_PORTNUM |= 1; }
PAC5XXX_RAMFUNC static inline void debugio2_off(void)       { DEBUGIO2_PORTNAME->OUT.DEBUGIO2_PORTNUM &= ~1; }
PAC5XXX_RAMFUNC static inline void debugio2_toggle(void)    { DEBUGIO2_PORTNAME->OUT.DEBUGIO2_PORTNUM ^= 1; }
#else
    static inline void config_debugio(void)     { }
PAC5XXX_RAMFUNC static inline void debugio1_on(void)        { }
PAC5XXX_RAMFUNC static inline void debugio1_off(void)       { }
PAC5XXX_RAMFUNC static inline void debugio1_toggle(void)    { }
PAC5XXX_RAMFUNC static inline void debugio2_on(void)        { }
PAC5XXX_RAMFUNC static inline void debugio2_off(void)       { }
PAC5XXX_RAMFUNC static inline void debugio2_toggle(void)    { }
#endif  //#ifdef ENABLE_DEBUG_IO

#endif  //#ifndef DEBUG_IO_H
