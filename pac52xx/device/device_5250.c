
/**
 * @file device_5250.c
 * @brief Use to initialize AFE low level blocks(SOC Bridge) and registers(CAFE)
 */

//=============================================================================
// Copyright (C) 2018 - 2023, Qorvo, Inc.
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

#if (DEVICE_SELECT == DEVICE_5250)
//====================================
// CONFIG: VP voltage setting
//====================================
// Configure VP as default as User Guide
#define CONFIG_VP_VOLTAGE               VP_VOLTAGE_12V

/**
 * @brief This function initialize AFE low level blocks(SOC Bridge) and registers(CAFE)
 */
void cafe_init(void)
{
    // Write all CAFE registers
  
    // If any power manager error bits set on startup, clear them
    if (pac5xxx_tile_register_read(ADDR_PWRSTAT))
        pac5xxx_tile_register_write(ADDR_PWRSTAT, 0xFF);

    // Reset AFE registers to default state
    pac5xxx_tile_register_write(ADDR_PROTINTM, 0);
    pac5xxx_tile_register_write(ADDR_SYSSTAT, 0);
    pac5xxx_tile_register_write(ADDR_SIGSET, 0);
    pac5xxx_tile_register_write(ADDR_ADCSCAN, 0);
    pac5xxx_tile_register_write(ADDR_CFGDRV1, 0);
    pac5xxx_tile_register_write(ADDR_ENSIG, 0);

    pac5xxx_tile_register_write(ADDR_PWRCTL, 0x40);                 // Set MCUALIVE
    pac5xxx_tile_register_write(ADDR_PSTATSET, 0x80);               // Set UNLOCK bit to allow firmware to modify SCFG
    pac5xxx_tile_register_write(ADDR_SCFG, 0x10);                   // Set VCLAMPSEL (62V) and FMODE (181kHz to 500kHz buck)
    pac5xxx_tile_register_write(ADDR_IMOD, 0xFF);                   // Set current modulation to 100%

    // Set vp as the configured voltage
    config_register_bits(0x15, VP_CONFIG_BITS_MASK, CONFIG_VP_VOLTAGE);

    init_diff_amp_gain();

    // Set OC protection threshold
    init_over_current_limit(MOTOR_CURRENT_LIMIT_DEFAULT_VALUE);
    
    // Clear any error bits
    pac5xxx_tile_register_write(ADDR_PROTSTAT, 0xFF);               // clear any set bits

    // Enable protection interrupt
    protint_enable();

    // Enable system nINTM
    pac5xxx_tile_register_write(ADDR_SYSSTAT, 1);                   // Enable nINTM interrupts from CAFE to MCU

    // Enable hysteresis
#ifdef ENABLE_CBC_CURRENT_LIMIT
    pac5xxx_tile_register_write(ADDR_SIGSET, (0x04 | 0x08));        // Enable comparator hysteresis on (LPROT | HPROT) comparators
#elif (SEL_OC_METHOD == OC_METHOD_HPDAC)
    pac5xxx_tile_register_write(ADDR_SIGSET, 0x08);                 // Enable comparator hysteresis on HPROT comparators
#else
    pac5xxx_tile_register_write(ADDR_SIGSET, 0x04);                 // Enable comparator hysteresis on LPROT comparators
#endif

    // Enable mux scan mode
    pac5xxx_tile_register_write(ADDR_ADCSCAN, 0x1F);                // ENADCBUF=1, ENSCAN=1, for auto-scan, sample and hold AO10SH=1, AO32SH=1, AO54SH=1

    //Disable all HS and LS drivers upon protection threshold trip
#if defined(ENABLE_CBC_CURRENT_LIMIT)
    pac5xxx_tile_register_write(ADDR_CFGDRV1, (DRV_LOWSIDE_PR1 | DRV_HIGHSIDE_PR1 | DRV_LOWSIDE_PR2 | DRV_HIGHSIDE_PR2));  // CFGDRV1: PROT1 or PROT2 will disable HS and LS drivers
#else
    pac5xxx_tile_register_write(ADDR_CFGDRV1, (DRV_LOWSIDE_PR1 | DRV_HIGHSIDE_PR1));                        // CFGDRV1: PROT1 will disable HS and LS drivers
#endif

    // Enable Brake-Before-Make to prevent low side and high side FET's from being enabled at the same time
    enable_bbm();

    // Check to see if power manager has any fault bits set before starting
    if (pac5xxx_tile_register_read(ADDR_PWRSTAT) != 0)
    {
        set_fault_terminal(FAULT_TERMINAL_AFEINIT);
    }

    // Enable signal manager and verify active
    pac5xxx_tile_register_write(ADDR_ENSIG, 1);
    if ((pac5xxx_tile_register_read(ADDR_ENSIG) & 0x01) != 1)
    {
        set_fault_terminal(FAULT_TERMINAL_AFEINIT);
    }
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
    // This function is empty for 5250 
}

/**
 * @brief  This function read and clr prostat flags
 */
void pac5xxx_clr_prostat_flags (void)
{
    // This function is empty for 5250 
}


/**
 * @brief  This function use to reset cycle by cycle current control
 */
#ifdef ENABLE_CBC_CURRENT_LIMIT
void cbc_current_reset(void)
{
    // Clear int/fault flags
    pac5xxx_tile_register_write(ADDR_PROTINTM, 0x00);
    while (pac5xxx_tile_register_read(ADDR_PROTSTAT));
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
 * @brief  This function use to update HPDAC
 */
void hpdac_param_write (uint32_t lpdac_value, char lpdac_factor, uint32_t current_limit)
{
#if defined(ENABLE_CBC_CURRENT_LIMIT) || (SEL_OC_METHOD == OC_METHOD_HPDAC)
    uint32_t hpdac_value = 0;     
    // divide subtract 512 for current limit then des-scale it to fix16, divide it the amp gain in fix16,
    // scale it back divide it by 4 to fit for hpdac
    hpdac_value = (fix16_mul(fix16_div(((lpdac_value - 512) << 16), (fix16_t) (lpdac_factor << 16)), (fix16_t) (QCONST(HPDAC_OC_LIMIT_MULTIPLIER, 16))) >> 16) >> 2;

    if (hpdac_value > 255)
    {
        hpdac_value = 255;
        set_warning(WARNING_MAIN_OC_VALUE);
    }
    else
    {
        clear_warning(WARNING_MAIN_OC_VALUE);
    }

        pac5xxx_tile_register_write(ADDR_HPDAC, hpdac_value);
#endif
}


/**
 * @brief  This function use to update LPDAC
 */
void lpdac_param_write(uint32_t lpdac_value)
{
    pac5xxx_tile_register_write(ADDR_LPDAC0, (uint8_t)((lpdac_value >> 2) & 0xFF));   // HB: 8b MSB
    pac5xxx_tile_register_write(ADDR_LPDAC1, (uint8_t)((lpdac_value & 0x3)));         // LB: 2b LSB
}


/**
 * @brief  This function use to get ARCH type
 */
fix16_t arch_type_param_read(uint32_t* value)
{
    fix16_t temp_value;

    temp_value  = *value;
    // 4 Bits reserved for Arch definition
    temp_value |= (0x01 << 24);
    return temp_value;    
}


/**
 * @brief This function use read LPDAC and HPDAC values
 */
#ifdef ENABLE_DEBUG_PARAMEXPORT
void hpdac_lpdaccombined_param_read (fix16_t* value1, fix16_t* value2, fix16_t* value3)
{
    *value1 = pac5xxx_tile_register_read(ADDR_HPDAC);     // HB: 8b MSB
    *value2 = ((pac5xxx_tile_register_read(ADDR_LPDAC0)<<2) & 0x03FC);    // HB: 8b MSB
    *value2 = *value2 + (pac5xxx_tile_register_read(ADDR_LPDAC1)& 0x03);          // LB: 2b LSB
}
#endif


/**
 * @brief This function use read LPDAC and HPDAC values for debug export
 */
#ifdef ENABLE_DEBUG_PARAMEXPORT 
void lpdac_hpdac_param_read (fix16_t* value1, fix16_t* value2, fix16_t* value3)
{
    *value1 = pac5xxx_tile_register_read(ADDR_LPDAC0) & (0xFF);
    *value2 = pac5xxx_tile_register_read(ADDR_LPDAC1) & (0x03);
    *value3 = pac5xxx_tile_register_read(ADDR_HPDAC);
}
#endif

#endif // #ifdef DEVICE_SELECT == DEVICE_5250

