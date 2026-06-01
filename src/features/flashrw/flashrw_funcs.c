
/**
 * @file flash_read_wr.c
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
#include "flashrw_funcs.h"

#ifdef ENABLE_FLASH_WR_RD_FIXED_ADDR
#include "flash_ctr_fixed.h"
#endif

#if defined(ENABLE_FLASH_RW)

/**
 * @brief  This function clr flash write event status flags
 * @return None
 */
// Rename flash_status_clear_all()
void init_flash_data_write_status(void)
{
    flash_status_put_startwrite(FALSE);
    flash_status_put_writedone(FALSE);
    flash_status_put_readdone(FALSE);
    flash_status_put_starterase(FALSE);
    flash_status_put_erasedone(FALSE);
}


/**
 * @brief  This function use get status of flash data is ready to write
 * @return 1 or 0, low means we flash data is not ready to wrire else ready
 */
uint32_t flash_status_get_startwrite(void)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    return (flash_read_wr_ptr->start_write);
}


/**
 * @brief  This function clr flash write event status
 * @return None
 */
void flash_status_clear_startwrite(void)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    flash_read_wr_ptr->start_write = FALSE;
}


/**
 * @brief  This function clr or set flash write event status
 * @return None
 */
void flash_status_put_startwrite(int32_t value)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    flash_read_wr_ptr->start_write = value;
}


/**
 * @brief  This function use get status of flash write done status
 * @return 1 or 0, low means flash write event not done else done
 */
uint32_t flash_status_get_writedone(void)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    return (flash_read_wr_ptr->done_write);
}


/**
 * @brief  This function clr or set flash write event complete status
 * @return None
 */
void flash_status_put_writedone(int32_t value)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    flash_read_wr_ptr->done_write = value;
    
    if (value)
        motor.status.motor |= STATUS_FLASH_OPERATION_COMP;
}


/**
 * @brief  This function clr or set flash date read event status
 * @return None
 */
void flash_status_put_startread(int32_t value)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    flash_read_wr_ptr->start_read = value;
}

/**
 * @brief  This function return data read flag, if it set we have a flash read request
 * @return 1 or 0, 1 means we have flash read request,0 means no flash read request
 */
uint32_t flash_status_get_startread(void)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    return (flash_read_wr_ptr->start_read);
}


/**
 * @brief  This function clr or set data read flag, if it set we have a flash read request
 * @return None
 */
void flash_status_put_readdone(int32_t value)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    flash_read_wr_ptr->done_read = value;
    
    if (value)
        motor.status.motor |= STATUS_FLASH_OPERATION_COMP;
}


/**
 * @brief  This function return data read flag, if it set we have a flash read request
 * @return 1 or 0, 1 means we have flash read request,0 means no flash read request
 */
uint32_t flash_status_get_readdone(void)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    return (flash_read_wr_ptr->done_read);
}


/**
 * @brief  This function clr or set erase start flag
 * @return None
 */
void flash_status_put_starterase(int32_t value)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    flash_read_wr_ptr->start_erase = value;
}

/**
 * @brief  This function return data erase start flag, if it set we have a flash erase started
 * @return 1 or 0, 1 means we have flash erase started,0 means no flash erase not started
 */
uint32_t flash_status_get_starterase(void)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    return (flash_read_wr_ptr->start_erase);
}


/**
 * @brief  This function clr or set data erase flag, if it set we have a flash erase request
 * @return None
 */
void flash_status_put_erasedone(int32_t value)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    flash_read_wr_ptr->done_erase = value;
    
    if (value)
        motor.status.motor |= STATUS_FLASH_OPERATION_COMP;
}


/**
 * @brief  This function return data erase flag, if it set we have a flash erase request
 * @return 1 or 0, 1 means we have flash erase request,0 means no flash erase request
 */
uint32_t flash_status_get_erasedone(void)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    return (flash_read_wr_ptr->done_erase);
}


/**
 * @brief  This function clr or set flash read write busy flag, if it set we have a flash function in process
 * @return None
 */
void flash_status_put_busy(int32_t value)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    flash_read_wr_ptr->busy = value;
    
//    if (value)
//        motor.status.motor |= STATUS_FLASH_OPERATION_COMP;
}


/**
 * @brief  This function return flash read write busy flag, if it set we have a flash function in process
 * @return 1 or 0, 1 means we have flash erase request,0 means no flash erase request
 */
uint32_t flash_status_get_busy(void)
{
    Struct_Flash_Read_Wr* flash_read_wr_ptr = &motor.flash_read_wr;

    return (flash_read_wr_ptr->busy);
}


/**
 * @brief  This function initialize flash clear power down event status
 * @return None
 */
void init_flash_at_pwr_up(void)
{
    /* Buffer is in words and size of buffer is also in words */
    __disable_irq();
    init_flash_data_write_status();
    __enable_irq();
}


void flashrw_state_enter(Struct_Motor* motor_ptr)
{
    if(flash_status_get_startread() || flash_status_get_starterase() || flash_status_get_startwrite())
    {
        motor_ptr->state.main = STATE_MAIN_FLASHRW;
    }
}


void flashrw_state_exit(Struct_Motor* motor_ptr)
{
    if(motor_ptr->flash_read_wr.state_flashrw == STATE_FLASHRW_IDLE)
    {
        motor_ptr->state.main = STATE_MAIN_STOPPED;
    }
}


void flashrw_controller(Struct_Motor* motor_ptr)
{
    // FlashRW CMD Handler
    if(flash_status_get_startread())
    {
        motor_ptr->flash_read_wr.state_flashrw = STATE_FLASHRW_READ;
    }
    else if (flash_status_get_starterase())
    {
        motor_ptr->flash_read_wr.state_flashrw = STATE_FLASHRW_ERASE;
    }
    else if (flash_status_get_startwrite())
    {
        motor_ptr->flash_read_wr.state_flashrw = STATE_FLASHRW_WRITE;
    }

    // FlashRW States
    switch (motor_ptr->flash_read_wr.state_flashrw)
    {
    case STATE_FLASHRW_READ:
        flashrw_read();
        motor_ptr->flash_read_wr.state_flashrw = STATE_FLASHRW_IDLE;
        break;

    case STATE_FLASHRW_WRITE:
        flashrw_write();
        motor_ptr->flash_read_wr.state_flashrw = STATE_FLASHRW_IDLE;
         break;

    case STATE_FLASHRW_ERASE:
        flashrw_erase();
        motor_ptr->flash_read_wr.state_flashrw = STATE_FLASHRW_IDLE;
        break;

    case STATE_FLASHRW_IDLE:
    default:
        break;
    }
}

#endif  //#if defined(ENABLE_FLASH_RW)
