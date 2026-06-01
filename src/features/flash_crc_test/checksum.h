
//=============================================================================
// Copyright (C) 2017-2019, Qorvo, Inc.
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

#ifndef CHECKSUM_H
#define CHECKSUM_H

unsigned short crc16_table_256(unsigned short sum, unsigned char *p, unsigned int len, unsigned short *p_crc16_table);
unsigned short crc16(unsigned short sum, unsigned char *p, unsigned int len);

extern unsigned short *crc16_table;

#endif  //CHECKSUM_H
