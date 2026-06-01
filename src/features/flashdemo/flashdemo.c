
/**
 * @file flashdemo.c
 * @brief Use to provide interface to read write data from and to flash
 */

//=============================================================================
// Copyright (C) 2022 - 2023, Qorvo, Inc.
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
#include "pac5xxx.h"
#include "pac5xxx_driver_gpio.h"
#include "pac5xxx_driver_memory.h"
#include "motor_struct.h"
#include "config_app.h"
#include "board.h"
#include "param_funcs.h"
#include "config_features.h"
#include "flashdemo.h"
#include "flashrw_funcs.h"
#if (FLASHDEMO_FLASHRW_MODESEL == FLASHRW_MODELSEL_DYADDR)
#include "flash_dyadr_ctr.h"
#endif

#if defined(ENABLE_FLASHDEMO)

int32_t flash_test_data[FLASHDEMO_FLASH_WORD_ARRAY_SIZE];

Struct_Flash_Rd_Wr_DyCtr flash_rw_dyctr_demo;


/**
 * @brief  This function use to write local test string to flash memory
 * @return none
 */
void flashrw_init(void)
{
#if (FLASHDEMO_FLASHRW_MODESEL == FLASHRW_MODELSEL_DYADDR)
    flashrw_dyadr_init(&flash_rw_dyctr_demo, FLASHDEMO_FLASH_START_ADDR, FLASHDEMO_FLASH_END_ADDR);
#endif
}
/**
 * @brief  This function read data from the flash and store in src array
 * @return none
 */
void flashrw_read(void)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    /* Passing flash_test_data as array */
#if (FLASHDEMO_FLASHRW_MODESEL == FLASHRW_MODELSEL_FXADDR)
    flash_read_wr_ptr->flash_status = flashrw_fxadr_read(FLASHDEMO_FLASH_START_ADDR, flash_test_data,
        FLASHDEMO_FLASH_WORD_ARRAY_SIZE, FLASHDEMO_FLASH_END_ADDR);
#endif

#if (FLASHDEMO_FLASHRW_MODESEL == FLASHRW_MODELSEL_DYADDR)
    flash_read_wr_ptr->flash_status = flashrw_dyadr_read(&flash_rw_dyctr_demo, (uint32_t *) &flash_test_data, FLASHDEMO_FLASH_WORD_ARRAY_SIZE);
#endif

    flash_status_put_startread(FALSE);
    flash_status_put_readdone(TRUE);
}

/**
 * @brief  This function use to write local test string to flash memory
 * @return none
 */
PAC5XXX_RAMFUNC void flashrw_write(void)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    /* Passing flash_test_data as array */
#if (FLASHDEMO_FLASHRW_MODESEL == FLASHRW_MODELSEL_FXADDR)
    flash_read_wr_ptr->flash_status = flashrw_fxadr_write_data(flash_read_wr_ptr, FLASHDEMO_FLASH_START_ADDR, flash_test_data,
        FLASHDEMO_FLASH_WORD_ARRAY_SIZE, FLASHDEMO_FLASH_END_ADDR);
#endif

#if (FLASHDEMO_FLASHRW_MODESEL == FLASHRW_MODELSEL_DYADDR)
    flash_read_wr_ptr->flash_status = flashrw_dyadr_write(&flash_rw_dyctr_demo, (uint32_t *) &flash_test_data, FLASHDEMO_FLASH_WORD_ARRAY_SIZE);
#endif

    // CLR flash data write ready flag so we will not reflash again
    flash_status_clear_startwrite();
    flash_status_put_writedone(TRUE);
}


/**
 * @brief  This function use to write src string to flash memory
 * @return none
 */
PAC5XXX_RAMFUNC void flashrw_erase(void)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    flash_read_wr_ptr->flash_status = FLASH_OK;
    // Disable global interrupts
    __disable_irq();
    flash_status_put_starterase(FALSE);
#if (FLASHDEMO_FLASHRW_MODESEL == FLASHRW_MODELSEL_FXADDR)
    flash_read_wr_ptr->flash_status = flash_erase(FLASHDEMO_FLASH_START_ADDR, FLASHDEMO_FLASH_END_ADDR);
#endif

#if (FLASHDEMO_FLASHRW_MODESEL == FLASHRW_MODELSEL_DYADDR)
    flashrw_dyadr_erase_page(31);
    flash_status_put_erasedone(TRUE);
#endif

#if (FLASHDEMO_FLASHRW_MODESEL == FLASHRW_MODELSEL_FXADDR)
    if (!flash_read_wr_ptr->flash_status)
        flash_status_put_erasedone(TRUE);
#endif

    // Enable global interrupts
    __enable_irq();
}


/**
 * @brief  This function is use to read, write and erase flash memory
 * @return none
 */
void flashdemo_param_process(ParameterCode param_code, int32_t rx_data)
{
    // Parameter handlers
    switch(param_code)
    {
        case FLASH_read:
            flash_status_put_busy(TRUE);
            flash_status_put_startread(TRUE);
            break;
        case FLASH_write:
            flash_status_put_busy(TRUE);
            flash_status_put_startwrite(TRUE);
            break;
        case FLASH_erase:
            flash_status_put_busy(TRUE);
            flash_status_put_starterase(TRUE);
            break;
        default:
            break;
    }
}


//#if defined(ENABLE_FLASHDEMO_DEBUGREAD)
/**
 * @brief  This function read data from array depending on index
 * @return none
 */
int32_t flashdemo_param_export(int32_t index)
{
    /* Passing flash_test_data as array */
    if ((index >=0) && (index < FLASHDEMO_FLASH_WORD_ARRAY_SIZE))
        return (flash_test_data[index]);
    else
        return flash_test_data[0];
}
//#endif


// JAGTAG: Add interface command
void flashdemo_data_set(void)
{
    int32_t test_count;

    for(test_count = 0; test_count < FLASHDEMO_FLASH_WORD_ARRAY_SIZE; test_count++)
        flash_test_data[test_count] = test_count + TEST_VALUE;
}


// JAGTAG: Add interface command
void flashdemo_data_clear(void)
{
    int32_t test_count;

    for(test_count = 0; test_count < FLASHDEMO_FLASH_WORD_ARRAY_SIZE; test_count++)
        flash_test_data[test_count] = 0;
}


Struct_Flash_Rd_Wr_DyCtr* flashdemo_get_address (void)
{
    return &flash_rw_dyctr_demo;
}

#endif  //#if defined(ENABLE_FLASHDEMO)
