
/**
 * @file device_5526.c
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

#if (DEVICE_SELECT == DEVICE_5526)
//====================================
// CONFIG: VP voltage setting
//====================================
// Configure VP as default as User Guide
#define CONFIG_VP_VOLTAGE               VP_VOLTAGE_12V

/**
 * @brief  This function initialize AFE low level blocks(SOC Bridge) and registers(CAFE)
 */
void cafe_init(void)
{
   // Configure SOC Bridge for talking to CAFE
    pac5xxx_tile_socbridge_config(1, 0);                                    // enable, int_enable

    // Write all CAFE registers
    // If any power manager error bits set on startup, clear them
    if (pac5xxx_tile_register_read(ADDR_STATUS))
        pac5xxx_tile_register_write(ADDR_STATUS, 0xFF);

    pac5xxx_tile_register_write(ADDR_PWRCTL, 0x04);                         // Wake Up Timer == 1sec
    pac5xxx_tile_register_write(ADDR_MISC, MODULE_MISC_BITS_DEFAULT);       // Set MCU Alive, Enable Signal Manager

    // Reset AFE registers to default state
    pac5xxx_tile_register_write(ADDR_PROTINTM, 0x00);

    // Diff Amp AIO54 Prop Delay 0 ns (0x00), Disable HS drivers on H-PROT (0x02), Disable LS drivers on L-PROT (0x01)
    // Enable Break Before Maker. Insert 100nS dead time between high and low side PWM signal of each pair of half bridge.

    // Diff Amp AIO54 Prop Delay 0 ns (0x00), Disable HS drivers on H-PROT (0x02), Disable LS drivers on L-PROT (0x01)
    pac5xxx_tile_register_write(ADDR_CFGDRV1, (DRV_LOWSIDE_PR | DRV_HIGHSIDE_PR));

    pac5xxx_tile_register_write(ADDR_CFGDRV2, 0x00);
    pac5xxx_tile_register_write(ADDR_CFGDRV3, 0x00);
    pac5xxx_tile_register_write(ADDR_SIGSET, 0x00);
    pac5xxx_tile_register_write(ADDR_SHCFG1, 0x00);                         // Disable ADC Scan (EMUX), Disable ADC Buffer

    // Set Application Specific Power Driver (ASPD) sink and source current for both low and high side drivers
    // Set Application Specific Power Driver (ASPD) sink and source current for low side drivers
    pac5xxx_tile_register_write(ADDR_DRVILIMLS, (ASPD_LSSINK << 4) | ASPD_LSSOURCE);
    // Set Application Specific Power Driver (ASPD) sink and source current for high side drivers
    pac5xxx_tile_register_write(ADDR_DRVILIMHS, (ASPD_HSSINK << 4) | ASPD_HSSOURCE);
    // Enable Break Before Maker. Insert 100nS dead time between high and low side PWM signal of each pair of half bridge.
    pac5xxx_tile_register_write(ADDR_CFGDRV4, 0x01);

    // SYSCONF: set vp as the configured voltage
    config_register_bits(ADDR_SYSCONF, VP_CONFIG_BITS_MASK, CONFIG_VP_VOLTAGE);

    init_diff_amp_gain();

    // Set OC protection threshold
    init_over_current_limit(MOTOR_CURRENT_LIMIT_DEFAULT_VALUE);

    // Clear any error bits
    pac5xxx_tile_register_write(ADDR_PROTSTAT, 0xFF);               // clear any set bits

    // Enable protection interrupt
    protint_enable();

    // Enable hysteresis on comparators
    // Enable hysteresis
    //Disable all HS and LS drivers upon protection threshold trip
#ifdef ENABLE_CBC_CURRENT_LIMIT
    pac5xxx_tile_register_write(ADDR_SIGSET, (0x04 | 0x08));        // Enable comparator hysteresis on (LPROT | HPROT) comparators
#elif (SEL_OC_METHOD == OC_METHOD_HPDAC)
    pac5xxx_tile_register_write(ADDR_SIGSET, 0x08);                 // Enable comparator hysteresis on HPROT comparators   
#else
    pac5xxx_tile_register_write(ADDR_SIGSET, 0x04);                 // Enable comparator hysteresis on LPROT comparators
#endif

    // EMUX configuration
    pac5xxx_tile_register_write(ADDR_SHCFG1, (0x10 | 0x08 | 0x07));         // Enable EMUX (0x10), Enable ADC Buffer (0x08), Enable S/H for Diff Amp 10/32/54 (0x07)

    
    // Check to see if power manager has any fault bits set before starting
    if (pac5xxx_tile_register_read(ADDR_FAULT) != 0)
    {
        set_fault_terminal(FAULT_TERMINAL_AFEINIT);
    }

    // Enable signal manager and verify active
    pac5xxx_tile_register_write(ADDR_MISC, pac5xxx_tile_register_read(ADDR_MISC) | 0x0);
    if ((pac5xxx_tile_register_read(ADDR_MISC) & 0x01) != 1)
    {
        set_fault_terminal(FAULT_TERMINAL_AFEINIT);
    }

    // If any power manager fault bits set on startup, clear them
    if (pac5xxx_tile_register_read(ADDR_FAULT))
        pac5xxx_tile_register_write(ADDR_FAULT, 0xFF);
}

/**
 * @brief  This function return status of PROTSTAT for over current
 */
int32_t pac5xxx_get_over_current_status (void)
{
    int32_t rv;
 
    rv = pac5xxx_tile_register_read(ADDR_PROTSTAT) & 0x07;
    return rv;
}

/**
 * @brief  This function return status of PROTSTAT for over current from HPDAC
 */
int32_t pac5xxx_get_over_current_status_from_hpdac (void)
{
    int32_t rv;
 
    rv = pac5xxx_tile_register_read(ADDR_PROTSTAT) & 0x70;
    return rv;
}

/**
 * @brief  This function return status of ADDR_FAULT for temperature warning and fault
 */
int32_t pac5xxx_get_over_temperature_status (void)
{
    return 0;
}


/**
 * @brief  This function return status of ADDR_DRV_FLT for driver fault
 */
int32_t pac5xxx_get_driver_fault (void)
{
    return 0;
}


/**
 * @brief  This function enable predriver
 */
void enable_driver (void)
{
    pac5xxx_tile_register_write(ADDR_ENDRV, 1); 
}


/**
 * @brief  This function clr interrupt, fault flags
 */
void pac5xxx_clr_int_fault_flags (void)
{
    uint32_t register_val;

    register_val = pac5xxx_tile_register_read(ADDR_SIGINTF);
    while (register_val&0xA0)
    {
        register_val = pac5xxx_tile_register_read(ADDR_SIGINTF);
    }
    register_val = pac5xxx_tile_register_read(ADDR_PROTSTAT);
    while (register_val&0x80)
    {
        register_val = pac5xxx_tile_register_read(ADDR_PROTSTAT);
    }

    register_val = pac5xxx_tile_register_read(ADDR_SIGINTF);
    while (register_val&0x50)
    {
        register_val = pac5xxx_tile_register_read(ADDR_SIGINTF);
    }
    register_val = pac5xxx_tile_register_read(ADDR_PROTSTAT);
    while (register_val&0x08)
    {
        register_val = pac5xxx_tile_register_read(ADDR_PROTSTAT);
    }
    pac5xxx_tile_register_write(ADDR_PROTSTAT, 0xFF);
}


/**
 * @brief  This function read and clr prostat flags
 */
void pac5xxx_clr_prostat_flags (void)
{
    // This function is empty for 5526 
}


/**
 * @brief  This function use to reset cycle by cycle current control
 */
#ifdef ENABLE_CBC_CURRENT_LIMIT
void cbc_current_reset (void)
{
    uint16_t register_val;

    register_val = pac5xxx_tile_register_read(ADDR_SIGINTF);
    while (register_val&0xA0)
    {
        register_val = pac5xxx_tile_register_read(ADDR_SIGINTF);
    }
    register_val = pac5xxx_tile_register_read(ADDR_PROTSTAT);
    while (register_val&0x80)
    {
        register_val = pac5xxx_tile_register_read(ADDR_PROTSTAT);
    }

    register_val = pac5xxx_tile_register_read(ADDR_SIGINTF);
    while (register_val&0x50)
    {
        register_val = pac5xxx_tile_register_read(ADDR_SIGINTF);
    }
    register_val = pac5xxx_tile_register_read(ADDR_PROTSTAT);
    while (register_val&0x08)
    {
        register_val = pac5xxx_tile_register_read(ADDR_PROTSTAT);
    }
    pac5xxx_tile_register_write(ADDR_PROTSTAT, 0xFF);

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
    motor_ptr->app.iphase_amp_gain = rx_data;
}


/**
 * @brief  This function use to write HPDAC
 */
void hpdac_param_write (uint32_t lpdac_value, char lpdac_factor, uint32_t current_limit)
{
#if defined(ENABLE_CBC_CURRENT_LIMIT) || (SEL_OC_METHOD == OC_METHOD_HPDAC)
    uint32_t hpdac_value = 0;  

    // Pre diff amp signal use for hpdac
    hpdac_value = (fix16_mul(fix16_div(((lpdac_value - 512) << 16), (fix16_t) (lpdac_factor << 16)), (fix16_t) (QCONST(HPDAC_OC_LIMIT_MULTIPLIER, 16))) >> 16);

    if (hpdac_value > 1023)
    {
        hpdac_value = 1023;
        set_warning(WARNING_MAIN_OC_VALUE);
    }
    else
    {
        clear_warning(WARNING_MAIN_OC_VALUE);
    }

    pac5xxx_tile_register_write(ADDR_HPDACH, (uint8_t)((hpdac_value >> 2) & 0xFF));   // HB: 8b MSB
    pac5xxx_tile_register_write(ADDR_HPDACL, (uint8_t)((hpdac_value & 0x3)));         // LB: 2b LSB
#endif
}


/**
 * @brief  This function use to write LPDAC
 */
void lpdac_param_write (uint32_t lpdac_value)
{
    pac5xxx_tile_register_write(ADDR_LPDACH, (uint8_t)((lpdac_value >> 2) & 0xFF));   // HB: 8b MSB
    pac5xxx_tile_register_write(ADDR_LPDACL, (uint8_t)((lpdac_value & 0x3)));         // LB: 2b LSB
}


/**
 * @brief  This function use to read ARCH type
 */
fix16_t arch_type_param_read (uint32_t* value)
{
    fix16_t temp_value;

    temp_value  = *value;
    // 4 Bits reserved for Arch definition
    temp_value |= (0x02 << 24);
    return temp_value;   
}


/**
 * @brief This function use read LPDAC and HPDAC values
 */
#ifdef ENABLE_DEBUG_PARAMEXPORT
void hpdac_lpdaccombined_param_read (fix16_t* value1, fix16_t* value2, fix16_t* value3)
{
    *value1 = pac5xxx_tile_register_read(ADDR_HPDACH);     // HB: 8b LSB
    *value2 = ((pac5xxx_tile_register_read(ADDR_LPDACH)<<2) & 0x03FC);    // HB: 8b MSB
    *value2  =  *value2 + (pac5xxx_tile_register_read(ADDR_LPDACL)& 0x03);         // LB: 2b LSB
}
#endif


/**
 * @brief This function use read LPDAC and HPDAC values for debug export
 */
#ifdef ENABLE_DEBUG_PARAMEXPORT 
void lpdac_hpdac_param_read (fix16_t* value1, fix16_t* value2, fix16_t* value3)
{
    *value1 = pac5xxx_tile_register_read(ADDR_LPDACH) & (0xFF);
    *value2 = pac5xxx_tile_register_read(ADDR_LPDACL) & (0x03);
    *value3 = (pac5xxx_tile_register_read(ADDR_HPDACH) << 2) & (0x3FC);
}
#endif

#endif // #ifdef DEVICE_SELECT == DEVICE_5526 
