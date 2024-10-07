
/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include "py/dynruntime.h"
#include "smtc_modem_hal.h"

#include <stdint.h>   // C99 types
#include <stdbool.h>  // bool type

// for variadic args
#include <stdio.h>
#include <stdarg.h>

// for memcpy
#include <string.h>

#if defined( SX1272 ) || defined( SX1276 )
#include "smtc_modem_utilities.h"
#include "sx127x.h"
#endif

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS-----------------------------------------------------------
 */

#ifndef MIN
#define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE CONSTANTS -------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE TYPES -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE VARIABLES -------------------------------------------------------
 */

#if !defined( SX127X )
//static hal_gpio_irq_t radio_dio_irq;
#endif

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITION ---------------------------------------------
 */

/* ------------ Reset management ------------*/
void smtc_modem_hal_reset_mcu( void )
{
    //hal_mcu_reset( );
}

/* ------------ Watchdog management ------------*/

void smtc_modem_hal_reload_wdog( void )
{
    //hal_watchdog_reload( );
}

/* ------------ Time management ------------*/

uint32_t smtc_modem_hal_get_time_in_s( void )
{
    //return hal_rtc_get_time_s( );
    return 0;
}

uint32_t smtc_modem_hal_get_time_in_ms( void )
{
    //return hal_rtc_get_time_ms( );
    return 0;
}

void smtc_modem_hal_set_offset_to_test_wrapping( const uint32_t offset_to_test_wrapping )
{
    //hal_rtc_set_offset_to_test_wrapping( offset_to_test_wrapping );
}
/* ------------ Timer management ------------*/

void smtc_modem_hal_start_timer( const uint32_t milliseconds, void ( *callback )( void* context ), void* context )
{
    //hal_lp_timer_start( HAL_LP_TIMER_ID_1, milliseconds,
    //                    &( hal_lp_timer_irq_t ){ .context = context, .callback = callback } );
}

void smtc_modem_hal_stop_timer( void )
{
    //hal_lp_timer_stop( HAL_LP_TIMER_ID_1 );
}

/* ------------ IRQ management ------------*/

void smtc_modem_hal_disable_modem_irq( void )
{

}

void smtc_modem_hal_enable_modem_irq( void )
{

}

/* ------------ Context saving management ------------*/

void smtc_modem_hal_context_restore( const modem_context_type_t ctx_type, uint32_t offset, uint8_t* buffer,
                                     const uint32_t size )
{

}

void smtc_modem_hal_context_store( const modem_context_type_t ctx_type, uint32_t offset, const uint8_t* buffer,
                                   const uint32_t size )
{

}

void smtc_modem_hal_context_flash_pages_erase( const modem_context_type_t ctx_type, uint32_t offset, uint8_t nb_page )
{

}

/* ------------ crashlog management ------------*/

void smtc_modem_hal_crashlog_store( const uint8_t* crash_string, uint8_t crash_string_length )
{

}

void smtc_modem_hal_crashlog_restore( uint8_t* crash_string, uint8_t* crash_string_length )
{

}

void smtc_modem_hal_crashlog_set_status( bool available )
{

}

bool smtc_modem_hal_crashlog_get_status( void )
{
    return false;
}

/* ------------ assert management ------------*/

void smtc_modem_hal_on_panic( uint8_t* func, uint32_t line, const char* fmt, ... )
{
    mp_printf(&mp_plat_print, "%s:%lu ", func, line);

    va_list args;
    va_start(args, fmt);
    mp_vprintf(&mp_plat_print, fmt, args);
    va_end(args);

    mp_raise_msg(&mp_type_RuntimeError, "Panic");
}

/* ------------ Random management ------------*/

uint32_t smtc_modem_hal_get_random_nb_in_range( const uint32_t val_1, const uint32_t val_2 )
{
    return 0; //hal_rng_get_random_in_range( val_1, val_2 );
}

/* ------------ Radio env management ------------*/

void smtc_modem_hal_irq_config_radio_irq( void ( *callback )( void* context ), void* context )
{

}

void smtc_modem_hal_radio_irq_clear_pending( void )
{

}

void smtc_modem_hal_start_radio_tcxo( void )
{
    // put here the code that will start the tcxo if needed
}

void smtc_modem_hal_stop_radio_tcxo( void )
{
    // put here the code that will stop the tcxo if needed
}

uint32_t smtc_modem_hal_get_radio_tcxo_startup_delay_ms( void )
{
    // Tcxo is present on LR1110 and LR1120 evk boards, LR1121 ref board does not have tcxo but only 32MHz xtal
#if defined( LR11XX ) && !defined( LR1121 )
    return 5;
#else
    return 0;
#endif
}

void smtc_modem_hal_set_ant_switch( bool is_tx_on )
{

}

/* ------------ Environment management ------------*/

uint8_t smtc_modem_hal_get_battery_level( void )
{
    // Please implement according to used board
    // According to LoRaWan 1.0.4 spec:
    // 0: The end-device is connected to an external power source.
    // 1..254: Battery level, where 1 is the minimum and 254 is the maximum.
    // 255: The end-device was not able to measure the battery level.
    return 255;
}

int8_t smtc_modem_hal_get_board_delay_ms( void )
{
#if defined( LR1121 )
    return 2;
#else
    return 1;
#endif  // LR1121
}

/* ------------ Trace management ------------*/

void smtc_modem_hal_print_trace( const char* fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    // TODO: hal_trace_print( fmt, args );
    va_end( args );
}

/* ------------ Fuota management ------------*/

#if defined( USE_FUOTA )
uint32_t smtc_modem_hal_get_hw_version_for_fuota( void )
{
    // Example value, please fill with application value
    return 0x12345678;
}

/**
 * @brief Only use if fmp package is activated
 *
 * @return uint32_t fw version as defined in fmp Alliance package TS006-1.0.0
 */
uint32_t smtc_modem_hal_get_fw_version_for_fuota( void )
{
    // Example value, please fill with application value
    return 0x11223344;
}

/**
 * @brief Only use if fmp package is activated
 *
 * @return uint8_t fw status field as defined in fmp Alliance package TS006-1.0.0
 */
uint8_t smtc_modem_hal_get_fw_status_available_for_fuota( void )
{
    // Example value, please fill with application value
    return 3;
}

uint32_t smtc_modem_hal_get_next_fw_version_for_fuota( void )
{
    // Example value, please fill with application value
    return 0x17011973;
}
/**
 * @brief Only use if fmp package is activated
 * @param [in] fw_to_delete_version    fw_to_delete_version as described in TS006-1.0.0
 * @return uint8_t fw status field as defined in fmp Alliance package TS006-1.0.0
 */
uint8_t smtc_modem_hal_get_fw_delete_status_for_fuota( uint32_t fw_to_delete_version )
{
    if( fw_to_delete_version != smtc_modem_hal_get_next_fw_version_for_fuota( ) )
    {
        return 2;
    }
    else
    {
        return 0;
    }
}
#endif  // USE_FUOTA

/* ------------ Needed for Cloud  ------------*/

int8_t smtc_modem_hal_get_temperature( void )
{
    // Please implement according to used board
    return 25;
}

uint16_t smtc_modem_hal_get_voltage_mv( void )
{
    return 3300;
}

/* ------------ Needed for Store and Forward service  ------------*/
#if defined( USE_STORE_AND_FORWARD )
uint16_t smtc_modem_hal_store_and_forward_get_number_of_pages( void )
{
    return 10;
}

uint16_t smtc_modem_hal_flash_get_page_size( void )
{
    return 0; //hal_flash_get_page_size( );
}
#endif

/* ------------ For Real Time OS compatibility  ------------*/

void smtc_modem_hal_user_lbm_irq( void )
{
    // Do nothing in case implementation is bare metal
}
/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DEFINITION --------------------------------------------
 */

/* --- EOF ------------------------------------------------------------------ */
