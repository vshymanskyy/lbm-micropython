
/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdint.h>   // C99 types
#include <stdbool.h>  // bool type

#include "ral_sx126x_bsp.h"
//#include "radio_utilities.h"

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

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE VARIABLES -------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITION ---------------------------------------------
 */

void ral_sx126x_bsp_get_reg_mode( const void* context, sx126x_reg_mod_t* reg_mode )
{
    *reg_mode = SX126X_REG_MODE_DCDC;
}

void ral_sx126x_bsp_get_rf_switch_cfg( const void* context, bool* dio2_is_set_as_rf_switch )
{
    *dio2_is_set_as_rf_switch = true;
}

void ral_sx126x_bsp_get_tx_cfg( const void* context, const ral_sx126x_bsp_tx_cfg_input_params_t* input_params,
                                ral_sx126x_bsp_tx_cfg_output_params_t* output_params )

{
    // get board tx power offset
    int8_t board_tx_pwr_offset_db = 0; // TODO: radio_utilities_get_tx_power_offset( );

    int16_t power = input_params->system_output_pwr_in_dbm + board_tx_pwr_offset_db;

    output_params->pa_ramp_time  = SX126X_RAMP_40_US;
    output_params->pa_cfg.pa_lut = 0x01;  // reserved value, same for sx1261 sx1262 and sx1268

#if defined( SX1262 ) || defined( SX1268 )

    // Clamp power if needed
    if( power > 22 )
    {
        power = 22;
    }
    if( power < -9 )
    {
        power = -9;
    }
    output_params->pa_cfg.device_sel                 = 0x00;  // select SX1262/SX1268 device
    output_params->pa_cfg.hp_max                     = 0x07;  // to achieve 22dBm
    output_params->pa_cfg.pa_duty_cycle              = 0x04;
    output_params->chip_output_pwr_in_dbm_configured = ( int8_t ) power;
    output_params->chip_output_pwr_in_dbm_expected   = ( int8_t ) power;
#else
    // Clamp power if needed
    if( power > 15 )
    {
        power = 15;
    }
    if( power < -17 )
    {
        power = -17;
    }

    // config pa according to power
    if( power == 15 )
    {
        output_params->pa_cfg.device_sel                 = 0x01;  // select SX1261 device
        output_params->pa_cfg.hp_max                     = 0x00;  // not used on sx1261
        output_params->pa_cfg.pa_duty_cycle              = 0x06;
        output_params->chip_output_pwr_in_dbm_configured = 14;
        output_params->chip_output_pwr_in_dbm_expected   = 15;
    }
    else if( power == 14 )
    {
        output_params->pa_cfg.device_sel                 = 0x01;  // select SX1261 device
        output_params->pa_cfg.hp_max                     = 0x00;  // not used on sx1261
        output_params->pa_cfg.pa_duty_cycle              = 0x04;
        output_params->chip_output_pwr_in_dbm_configured = 14;
        output_params->chip_output_pwr_in_dbm_expected   = 14;
    }
    else
    {
        output_params->pa_cfg.device_sel                 = 0x01;  // select SX1261 device
        output_params->pa_cfg.hp_max                     = 0x00;  // not used on sx1261
        output_params->pa_cfg.pa_duty_cycle              = 0x04;
        output_params->chip_output_pwr_in_dbm_configured = ( int8_t ) power;
        output_params->chip_output_pwr_in_dbm_expected   = ( int8_t ) power;
    }

#endif
}

void ral_sx126x_bsp_get_xosc_cfg( const void* context, ral_xosc_cfg_t* xosc_cfg,
                                  sx126x_tcxo_ctrl_voltages_t* supply_voltage, uint32_t* startup_time_in_tick )
{
    // No tcxo on Basic Modem sx1261,sx1262 or sx1268 reference boards
    *xosc_cfg = RAL_XOSC_CFG_XTAL;
}

void ral_sx126x_bsp_get_trim_cap( const void* context, uint8_t* trimming_cap_xta, uint8_t* trimming_cap_xtb )
{
    // Do nothing, let the driver choose the default values
}

void ral_sx126x_bsp_get_rx_boost_cfg( const void* context, bool* rx_boost_is_activated )
{
    *rx_boost_is_activated = false;
}

void ral_sx126x_bsp_get_ocp_value( const void* context, uint8_t* ocp_in_step_of_2_5_ma )
{
    // Do nothing, let the driver choose the default values
}

void ral_sx126x_bsp_get_lora_cad_det_peak( ral_lora_sf_t sf, ral_lora_bw_t bw, ral_lora_cad_symbs_t nb_symbol,
                                           uint8_t* in_out_cad_det_peak )
{
    // Function used to fine tune the cad detection peak, update if needed
}

/* --- EOF ------------------------------------------------------------------ */
