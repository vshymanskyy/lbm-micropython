
/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdint.h>   // C99 types
#include <stdbool.h>  // bool type

#include "sx126x_hal.h"

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS-----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE CONSTANTS -------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE TYPES -----------------------------------------------------------
 */

typedef enum
{
    RADIO_SLEEP,
    RADIO_AWAKE
} radio_sleep_mode_t;

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE VARIABLES -------------------------------------------------------
 */

// This variable will hold the current sleep status of the radio
static radio_sleep_mode_t radio_mode = RADIO_AWAKE;

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */

/**
 * @brief Wait until radio busy pin returns to 0
 */
static void sx126x_hal_wait_on_busy( void );

/**
 * @brief Check if device is ready to receive spi transaction.
 * @remark If the device is in sleep mode, it will awake it and wait until it is ready
 */
static void sx126x_hal_check_device_ready( void );

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITION ---------------------------------------------
 */

sx126x_hal_status_t sx126x_hal_write( const void* context, const uint8_t* command, const uint16_t command_length,
                                      const uint8_t* data, const uint16_t data_length )
{
    sx126x_hal_check_device_ready( );

    /*
    // Put NSS low to start spi transaction
    hal_gpio_set_value( RADIO_NSS, 0 );
    for( uint16_t i = 0; i < command_length; i++ )
    {
        hal_spi_in_out( RADIO_SPI_ID, command[i] );
    }
    for( uint16_t i = 0; i < data_length; i++ )
    {
        hal_spi_in_out( RADIO_SPI_ID, data[i] );
    }
    // Put NSS high as the spi transaction is finished
    hal_gpio_set_value( RADIO_NSS, 1 );

    // 0x84 - SX126x_SET_SLEEP opcode. In sleep mode the radio dio is struck to 1 => do not test it
    if( command[0] != 0x84 )
    {
        sx126x_hal_check_device_ready( );
    }
    else
    {
        radio_mode = RADIO_SLEEP;
    }
    */

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_read( const void* context, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length )
{
    sx126x_hal_check_device_ready( );
    /*
    // Put NSS low to start spi transaction
    hal_gpio_set_value( RADIO_NSS, 0 );
    for( uint16_t i = 0; i < command_length; i++ )
    {
        hal_spi_in_out( RADIO_SPI_ID, command[i] );
    }
    for( uint16_t i = 0; i < data_length; i++ )
    {
        data[i] = hal_spi_in_out( RADIO_SPI_ID, 0 );
    }
    // Put NSS high as the spi transaction is finished
    hal_gpio_set_value( RADIO_NSS, 1 );
    */

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_reset( const void* context )
{
    /*
    hal_gpio_set_value( RADIO_NRST, 0 );
    hal_mcu_wait_us( 5000 );
    hal_gpio_set_value( RADIO_NRST, 1 );
    hal_mcu_wait_us( 5000 );
    radio_mode = RADIO_AWAKE;
    */
    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_wakeup( const void* context )
{
    sx126x_hal_check_device_ready( );
    return SX126X_HAL_STATUS_OK;
}

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DEFINITION --------------------------------------------
 */

static void sx126x_hal_wait_on_busy( void )
{
    /*while( hal_gpio_get_value( RADIO_BUSY_PIN ) == 1 )
    {
    };*/
}

static void sx126x_hal_check_device_ready( void )
{
    /*if( radio_mode != RADIO_SLEEP )
    {
        sx126x_hal_wait_on_busy( );
    }
    else
    {
        // Busy is HIGH in sleep mode, wake-up the device
        hal_gpio_set_value( RADIO_NSS, 0 );
        sx126x_hal_wait_on_busy( );
        hal_gpio_set_value( RADIO_NSS, 1 );
        radio_mode = RADIO_AWAKE;
    }*/
}
