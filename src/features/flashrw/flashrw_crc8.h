//=============================================================================
// Copyright (C) 2021 - 2023, Qorvo, Inc.
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
#ifndef CRC_8_H
#define CRC_8_H

#include "pac5xxx.h"
/**
*@brief calculate crc code
*@param CRC input crc
*@param DataArray input data for calculation
*@param Length length of input data
*@return crc code
**/
extern PAC5XXX_RAMFUNC uint8_t crc_8_calculation(uint8_t CRC, const uint8_t *DataArray, const uint16_t Length);
/**
*@brief calculate crc code for 1 byte
*@param crc input crc
*@param data input data
*@return crc code
**/
extern uint8_t fast_crc_calculation(uint8_t crc, uint8_t data);

#endif // CRC_8_H

