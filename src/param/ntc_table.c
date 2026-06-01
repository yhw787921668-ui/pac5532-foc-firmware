/**
 * @file ntc_table.c
 * @brief Use for temperature calculation
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

#include "config_features.h"
#include "motor_struct.h"

#ifdef ENABLE_LCORR_TEMP
const unsigned short
ntc_value_table[110] =
  {
          12564,        // 5 degree C
          11926,
          11324,
          10756,
          10220,
          9714,
          9236,
          8784,
          8356,
          7952,

          7570,
          7208,
          6866,
          6541,
          6234,
          5943,
          5668,
          5406,
          5158,
          4923,

          4700,
          4488,
          4287,
          4096,
          3915,
          3742,
          3579,
          3423,
          3275,
          3134,

          3000,
          2872,
          2750,
          2635,
          2524,
          2419,
          2319,
          2224,
          2133,
          2046,

          1963,
          1884,
          1809,
          1737,
          1668,
          1602,
          1540,
          1480,
          1422,
          1368,

          1315,
          1265,
          1217,
          1171,
          1127,
          1085,
          1045,
          1006,
          969,
          934,

          900,
          867,
          836,
          806,
          778,
          750,
          724,
          698,
          674,
          651,

          628,
          607,
          586,
          566,
          547,
          528,
          511,
          494,
          477,
          462,

          446,
          432,
          418,
          404,
          391,
          379,
          367,
          355,
          344,
          333,

          323,
          313,
          303,
          294,
          285,
          276,
          268,
          260,
          252,
          244,

          237,
          230,
          223,
          217,
          210,
          204,
          198,
          193,
          187,
          182,      // 114 degree C
  };

//  NTC
uint16_t temperature_value_sum;
uint16_t temperature_value_buff[16];
uint8_t temperature_value_index;
uint16_t temperature_adc_value;
fix16_t ntc_resistor_value;
uint8_t ntc_resistor_error;
uint8_t temperature_degree_value;
uint8_t Timer_NTC=0;

void NTC_monitor(void)
{
    static Struct_Motor* motor_ptr = &motor;
    uint8_t ntc_table_count;
    if(Timer_NTC<1)
        return;
    Timer_NTC=0;
#if 1
    temperature_value_sum -= temperature_value_buff[temperature_value_index];
    temperature_value_buff[temperature_value_index] = PAC5XXX_ADC->AS1R3.VAL;
    temperature_value_sum += temperature_value_buff[temperature_value_index];
    temperature_value_index++;
    if(temperature_value_index >= 16)
    {
        temperature_value_index = 0;
    }
    temperature_adc_value = temperature_value_sum>>4;
    ntc_resistor_value = 1023 - temperature_adc_value;
    ntc_resistor_value = fix16_mul(ntc_resistor_value, SUM_RESISTOR_VALUE);
    ntc_resistor_value = fix16_div(ntc_resistor_value, temperature_adc_value<<16);

    if(ntc_resistor_value < 100)    //Short-circuit (Temperature higher than 125 degree C, NTC resistance low)
    {
        if(ntc_resistor_error++ >= 3)
        {
            ntc_resistor_error = 3;
            temperature_degree_value = 200;
        }
    }
    else if(ntc_resistor_value > 58718)    //Open-circuit (Temperature less than 21 degree C, NTC resistance high)
    {
        if(ntc_resistor_error++ >= 3)
        {
            ntc_resistor_error = 3;
            temperature_degree_value = 250;
        }
    }
    else
    {
        for(ntc_table_count = 0; ntc_table_count < 109; ntc_table_count++)
        {
            if(ntc_resistor_value >= ntc_value_table[ntc_table_count])
            {
                break;
            }
        }
        temperature_degree_value = ntc_table_count + 5;
        ntc_resistor_error = 0;
    }
    motor_ptr->temperature_degree_value = temperature_degree_value<<16;
#endif
}


#endif  //#ifdef ENABLE_LCORR_TEMP
