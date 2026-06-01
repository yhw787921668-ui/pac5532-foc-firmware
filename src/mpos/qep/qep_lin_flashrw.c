
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

#include "motor_struct.h"
#include "flashrw_funcs.h"


#if defined(ENABLE_QEP_LIN_FLASHRW)
/**
 * @brief  This function read data from the flash and store in src array
 * @return none
 */
void flashrw_read(void)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    /* Passing flash_test_data as array */
#if (QEP_LIN_FLASHRW_MODESEL == FLASHRW_MODELSEL_FXADDR)
    flash_read_wr_ptr->flash_status = flashrw_fxadr_read(QEP_LIN_FLASH_START_ADDR, motor.mpos.qep.qep_count_diff_between_ideal_and_raw_q06,
        QEP_LIN_FLASH_WORD_ARRAY_SIZE, QEP_LIN_FLASH_END_ADDR);
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
#if (QEP_LIN_FLASHRW_MODESEL == FLASHRW_MODELSEL_FXADDR)
    flash_read_wr_ptr->flash_status = flashrw_fxadr_write_data(flash_read_wr_ptr, QEP_LIN_FLASH_START_ADDR, motor.mpos.qep.qep_count_diff_between_ideal_and_raw_q06,
        QEP_LIN_FLASH_WORD_ARRAY_SIZE, QEP_LIN_FLASH_END_ADDR);
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
#if (QEP_LIN_FLASHRW_MODESEL == FLASHRW_MODELSEL_FXADDR)
    flash_read_wr_ptr->flash_status = flash_erase(QEP_LIN_FLASH_START_ADDR, QEP_LIN_FLASH_END_ADDR);
#endif

#if (QEP_LIN_FLASHRW_MODESEL == FLASHRW_MODELSEL_FXADDR)
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
void qep_lin_flashrw_param_process(ParameterCode param_code, int32_t rx_data)
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

#endif
