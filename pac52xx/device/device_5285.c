
/**
 * @file device_5285.c
 * @brief Use to initialize AFE low level blocks(SOC Bridge) and registers(CAFE)
 */

//=============================================================================
// * @note
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
#include "config_app.h"

#include "motor_struct.h"
#include "pac_init.h"
#include "param_funcs.h"
#include "fault.h"
#include "pac5xxx_driver_tile.h"
#include "pac5xxx_tile_SignalManager.h"

#if (DEVICE_SELECT == DEVICE_5285)
/**
 * @brief  This function initialize AFE low level blocks(SOC Bridge) and registers(CAFE)
 */
void cafe_init(void)
{
    volatile unsigned char reg_value;

    // Configure SOC Bridge for talking to CAFE
    pac5xxx_tile_socbridge_config(1, 0);                                    // enable, int_enable

    // Write all CAFE registers
    // If any power manager error bits set on startup, clear them
    if (pac5xxx_tile_register_read(ADDR_STATUS))
        pac5xxx_tile_register_write(ADDR_STATUS, 0xFF);

    pac5xxx_tile_register_write(ADDR_PWRCTL, 0x05);                         // Wake Up Timer == 1sec
    pac5xxx_tile_register_write(ADDR_MISC, MODULE_MISC_BITS_DEFAULT);       // Set MCU Alive, Enable Signal Manager

    // Reset AFE registers to default state
    pac5xxx_tile_register_write(ADDR_PROTINTM, 0x00);

    pac5xxx_tile_register_write(ADDR_EMUXEN, 0x00);                         // Disable ADC Scan (EMUX), Disable ADC Buffer

    init_diff_amp_gain();

    // Set OC protection threshold
    init_over_current_limit(MOTOR_CURRENT_LIMIT_DEFAULT_VALUE);

    // Clear any error bits
    pac5xxx_tile_register_write(ADDR_PROTSTAT, 0xFF);               // clear any set bits

    // Enable protection interrupt
    protint_enable();
#ifdef ENABLE_CBC_CURRENT_LIMIT
    pac5xxx_tile_register_write(ADDR_PROTINTM, (0x01 | 0x10));      // PROTINTM: (LPPROT10 | HPROT10)
#else
    pac5xxx_tile_register_write(ADDR_PROTINTM, 0x10);               // PROTINTM: HPPROT10
#ifdef ENABLE_5285_LPDAC_INT
    pac5xxx_tile_register_write(ADDR_PROTINTM, (0x01 | 0x10));      // PROTINTM: (LPPROT10 | HPROT10)
#endif
#endif

    // EMUX configuration
    pac5xxx_tile_register_write(ADDR_EMUXEN, (0x10 | 0x08 | 0x01));			// Enable EMUX (0x10), Enable ADC Buffer (0x08), Enable S/H for Diff Amp 10 (0x01)

    // Enable over temperature warning	
#ifdef ENABLE_OVER_TEMPERAURE_WARNING
    pac5xxx_tile_register_write(ADDR_FAULTEN, 0x40);
#endif

    // Check to see if power manager has any fault bits set before starting
    reg_value = pac5xxx_tile_register_read(ADDR_DRV_FLT);
    if (reg_value & (0x01 | 0x04))    // Drive Fault, Charge Pump Fault
    {
        set_fault_terminal(FAULT_TERMINAL_AFEINIT);
    }

    // Enable signal manager and verify active
    pac5xxx_tile_register_write(ADDR_MISC, pac5xxx_tile_register_read(ADDR_MISC) | 0x01 | 0x04);
    if ((pac5xxx_tile_register_read(ADDR_MISC) & 0x01) != 1)
    {
        set_fault_terminal(FAULT_TERMINAL_AFEINIT);
    }

    // If any power manager fault bits set on startup, clear them
    if (pac5xxx_tile_register_read(ADDR_FAULT))
        pac5xxx_tile_register_write(ADDR_FAULT, 0xFF);
}


#if 0
int32_t temp_conv_vptat2temp(int32_t vptat_q16)
{
    int32_t temp1, temp2;
    int32_t temp_c_q16;

    // Convert VPTAT to Kelvin
    temp1 = vptat_q16 + QCONST(0.075f, Q16);            // q16
    temp2 = vt300k_q16 + QCONST(0.075f, Q16);           // q16
    temp1 = fix16_div(temp1, temp2);                    // q16 = div16(q16,q16)
    temp1 = fix16_mul(temp1, QCONST(300.0f, Q16));      // q16 = mul16(q16,q16)

    // Convert from Kelvin to Celsius
    temp_c_q16 = temp1 - QCONST(273.15, Q16);           // q16

    // Return calculated temperature in Celsius (q16 format)
    return temp_c_q16;
}
#endif


/**
 * @brief  This function return status of PROTSTAT for over current
 */
int32_t pac5xxx_get_over_current_status (void)
{
    int32_t rv;

#ifdef ENABLE_5285_LPDAC_INT
    rv = pac5xxx_tile_register_read((ADDR_PROTSTAT) & 0x11);
#else
    rv = (pac5xxx_tile_register_read(ADDR_PROTSTAT) & 0x10);
#endif
    return rv;
}


/**
 * @brief  This function return status of ADDR_FAULT for temperature warning and fault
 */
int32_t pac5xxx_get_over_temperature_status (void)
{
    int32_t rv = 0;
#ifdef ENABLE_OVER_TEMPERAURE_WARNING
    // temperature warning - warning latch - fault
    rv = pac5xxx_tile_register_read(ADDR_FAULT) & (0x80 | 0x40 | 0x20);
#endif
    return rv;
}


/**
 * @brief  This function return status of ADDR_DRV_FLT for driver fault
 */
int32_t pac5xxx_get_driver_fault (void)
{
    int32_t rv = 0;
#ifdef ENABLE_DRIVER_FAULT
    // Check to see if power manager has any fault bits set
    rv = pac5xxx_tile_register_read(ADDR_DRV_FLT) & (0x01 | 0x04);    // Drive Fault, Charge Pump Fault;
#endif
    return rv;
}


/**
 * @brief  This function enable predriver
 */
void enable_driver (void)
{
    pac5xxx_tile_register_write(ADDR_ENDRV, 1 | (1 << 1));
}


/**
 * @brief  This function pro stat flags
 */
void pac5xxx_clr_int_fault_flags(void)
{
    int32_t register_val;

    register_val = pac5xxx_tile_register_read(ADDR_PROTSTAT);
    while (register_val & 0x11)
    {
        pac5xxx_tile_register_write(ADDR_PROTSTAT, 0x11);    
        register_val = pac5xxx_tile_register_read(ADDR_PROTSTAT);
    }
    pac5xxx_tile_register_write(ADDR_PROTSTAT, 0x11); 
}


/**
 * @brief  This function use to reset cycle by cycle current control
 */
#ifdef ENABLE_CBC_CURRENT_LIMIT
void cbc_current_reset(void)
{
    protint_enable();

    // Disable driver
    do {
        pac5xxx_tile_register_write(ADDR_ENDRV, 0);
    } while ((pac5xxx_tile_register_read(ADDR_ENDRV) & 0x1) != 0x0);

    // Enable driver
    do {
        pac5xxx_tile_register_write(ADDR_ENDRV, 1);
    } while ((pac5xxx_tile_register_read(ADDR_ENDRV) & 0x1) != 0x1);
}
#endif


/**
 * @brief  This function use to update diff amp gain variable
 */
void diff_amp_gain_param_write (Struct_Motor* motor_ptr, fix16_t rx_data)
{
    motor_ptr->app.iphase_amp_gain = UVW_PHASE_RSENSE_GAIN_DEFAULT_VALUE_5285;
}


/**
 * @brief  This function use to update HPDAC
 */
void hpdac_param_write (uint32_t lpdac_value, char lpdac_factor, uint32_t current_limit)
{
    // Empty for 5285
}


/**
 * @brief  This function use to update LPDAC
 */
void lpdac_param_write(uint32_t lpdac_value)
{
    uint32_t lpdac_value_new;

    if ((current_limit >> 6) > 15)
        lpdac_value_new = 15;
    else if ((current_limit >> 6) < 1)
        lpdac_value_new = 1;
    else
        lpdac_value_new = current_limit >> 6; 
    pac5xxx_tile_register_write(ADDR_LPDACH, (uint8_t)(lpdac_value_new & 0x0F));   // 4bits scale from 10 bits
}


/**
 * @brief  This function use to get ARCH type
 */
fix16_t arch_type_param_read(uint32_t* value)
{
    fix16_t temp_value;

    temp_value  = *value;
    // 4 Bits reserved for Arch definition
    temp_value |= (0x03 << 24);
    return temp_value;   
}


/**
 * @brief This function use read LPDAC and HPDAC values
 */
#ifdef ENABLE_DEBUG_PARAMEXPORT
void hpdac_lpdaccombined_param_read (fix16_t* value1, fix16_t* value2, fix16_t* value3)
{
    *value1 = (pac5xxx_tile_register_read(ADDR_LPDACH) & 0x0F);    // DAC: 4 LSB
}
#endif


/**
 * @brief This function use read LPDAC and HPDAC values for debug export
 */
#ifdef ENABLE_DEBUG_PARAMEXPORT 
void lpdac_hpdac_param_read (fix16_t* value1, fix16_t* value2, fix16_t* value3)
{
    *value3 = (pac5xxx_tile_register_read(ADDR_LPDACH) & 0x0F);    // DAC: 4 LSB
}
#endif

#endif // #ifdef DEVICE_SELECT == DEVICE_5285

