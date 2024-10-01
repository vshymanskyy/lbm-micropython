/**
 * Author:    Volodymyr Shymanskyy
 * Created:   24.08.2024
 **/

#include "py/dynruntime.h"
#include "smtc_modem_api.h"
#include "smtc_modem_utilities.h"
#include "smtc_modem_relay_api.h"

/*
 * NOTE:
 * This file is a complete mess, it's not an actual implementation
 */

#include <string.h>

#define STACK_ID 0

#define MODEM_EXAMPLE_REGION SMTC_MODEM_REGION_EU_868

#define SMTC_HAL_TRACE_INFO(...)
#define SMTC_HAL_TRACE_ERROR(...)
#define SMTC_HAL_TRACE_WARNING(...)
#define SMTC_HAL_TRACE_MSG_COLOR(...)
#define SMTC_HAL_TRACE_PRINTF(...)

#define USER_LORAWAN_DEVICE_EUI                        \
    {                                                  \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
    }
#define USER_LORAWAN_JOIN_EUI                          \
    {                                                  \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
    }
#define USER_LORAWAN_GEN_APP_KEY                                                                       \
    {                                                                                                  \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
    }
#define USER_LORAWAN_APP_KEY                                                                           \
    {                                                                                                  \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
    }

const uint8_t user_dev_eui[8]      = USER_LORAWAN_DEVICE_EUI;
const uint8_t user_join_eui[8]     = USER_LORAWAN_JOIN_EUI;
const uint8_t user_gen_app_key[16] = USER_LORAWAN_GEN_APP_KEY;
const uint8_t user_app_key[16]     = USER_LORAWAN_APP_KEY;

mp_obj_t mp_mod_time;
mp_obj_t mp_ticks_ms;
mp_obj_t mp_sleep_ms;

static mp_obj_t has_joined(void) {
    smtc_modem_status_mask_t status_mask = 0;
    smtc_modem_get_status( STACK_ID, &status_mask );
    return mp_obj_new_bool(( status_mask & SMTC_MODEM_STATUS_JOINED ) == SMTC_MODEM_STATUS_JOINED);
}

static MP_DEFINE_CONST_FUN_OBJ_0(has_joined_obj, has_joined);

static mp_obj_t loop(void) {

    int port = 101;
    uint32_t uplink_counter = 10;

    uint8_t buff[4] = { 0 };
    buff[0]         = ( uplink_counter >> 24 ) & 0xFF;
    buff[1]         = ( uplink_counter >> 16 ) & 0xFF;
    buff[2]         = ( uplink_counter >> 8 ) & 0xFF;
    buff[3]         = ( uplink_counter & 0xFF );
    smtc_modem_request_uplink( STACK_ID, port, false, buff, 4 );

    uint32_t sleep_time_ms = smtc_modem_run_engine();

    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_0(loop_obj, loop);

// Module Attributes

uint8_t membuff[256];

static mp_obj_t getattr(mp_obj_t attr) {
    size_t attr_len;
    char* attr_str = mp_obj_str_get_data(attr, &attr_len);
    if (!attr_str) {
        mp_raise_msg(&mp_type_RuntimeError, "Invalid attr");
        return mp_const_none;
    }
    if (!strncmp("_memory", attr_str, attr_len)) {
        return mp_obj_new_bytearray_by_ref(sizeof(membuff), (void*)(membuff));
    } else {
        mp_raise_msg(&mp_type_RuntimeError, "Unknown attr");
        return mp_const_none;
    }
}

static MP_DEFINE_CONST_FUN_OBJ_1(getattr_obj, getattr);

// Module Initialization

static void modem_event_callback( void );

mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY

    mp_mod_time = mp_import_name(MP_QSTR_time, mp_const_none, 0);
    mp_ticks_ms = mp_load_attr(mp_mod_time, MP_QSTR_ticks_ms);
    mp_sleep_ms = mp_load_attr(mp_mod_time, MP_QSTR_sleep_ms);

    // Make the function available in the module's namespace
    mp_store_global(MP_QSTR___getattr__, MP_OBJ_FROM_PTR(&getattr_obj));

    mp_store_global(MP_QSTR_has_joined, MP_OBJ_FROM_PTR(&has_joined_obj));


    smtc_modem_init( &modem_event_callback );

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}

#define ASSERT_SMTC_MODEM_RC( rc_func )                                                         \
    do                                                                                          \
    {                                                                                           \
        smtc_modem_return_code_t rc = rc_func;                                                  \
        if( rc == SMTC_MODEM_RC_NOT_INIT )                                                      \
        {                                                                                       \
            SMTC_HAL_TRACE_ERROR( "In %s - %s (line %d): %s\n", __FILE__, __func__, __LINE__,   \
                                  xstr( SMTC_MODEM_RC_NOT_INIT ) );                             \
        }                                                                                       \
        else if( rc == SMTC_MODEM_RC_INVALID )                                                  \
        {                                                                                       \
            SMTC_HAL_TRACE_ERROR( "In %s - %s (line %d): %s\n", __FILE__, __func__, __LINE__,   \
                                  xstr( SMTC_MODEM_RC_INVALID ) );                              \
        }                                                                                       \
        else if( rc == SMTC_MODEM_RC_BUSY )                                                     \
        {                                                                                       \
            SMTC_HAL_TRACE_ERROR( "In %s - %s (line %d): %s\n", __FILE__, __func__, __LINE__,   \
                                  xstr( SMTC_MODEM_RC_BUSY ) );                                 \
        }                                                                                       \
        else if( rc == SMTC_MODEM_RC_FAIL )                                                     \
        {                                                                                       \
            SMTC_HAL_TRACE_ERROR( "In %s - %s (line %d): %s\n", __FILE__, __func__, __LINE__,   \
                                  xstr( SMTC_MODEM_RC_FAIL ) );                                 \
        }                                                                                       \
        else if( rc == SMTC_MODEM_RC_NO_TIME )                                                  \
        {                                                                                       \
            SMTC_HAL_TRACE_WARNING( "In %s - %s (line %d): %s\n", __FILE__, __func__, __LINE__, \
                                    xstr( SMTC_MODEM_RC_NO_TIME ) );                            \
        }                                                                                       \
        else if( rc == SMTC_MODEM_RC_INVALID_STACK_ID )                                         \
        {                                                                                       \
            SMTC_HAL_TRACE_ERROR( "In %s - %s (line %d): %s\n", __FILE__, __func__, __LINE__,   \
                                  xstr( SMTC_MODEM_RC_INVALID_STACK_ID ) );                     \
        }                                                                                       \
        else if( rc == SMTC_MODEM_RC_NO_EVENT )                                                 \
        {                                                                                       \
            SMTC_HAL_TRACE_INFO( "In %s - %s (line %d): %s\n", __FILE__, __func__, __LINE__,    \
                                 xstr( SMTC_MODEM_RC_NO_EVENT ) );                              \
        }                                                                                       \
    } while( 0 )

uint8_t                  rx_payload[SMTC_MODEM_MAX_LORAWAN_PAYLOAD_LENGTH];  // Buffer for rx payload
uint8_t                  rx_payload_size;      // Size of the payload in the rx_payload buffer
smtc_modem_dl_metadata_t rx_metadata;           // Metadata of downlink
uint8_t                  rx_remaining;      // Remaining downlink payload in modem

static void modem_event_callback( void )
{
    SMTC_HAL_TRACE_MSG_COLOR( "Modem event callback\n", HAL_DBG_TRACE_COLOR_BLUE );

    smtc_modem_event_t current_event;
    uint8_t            event_pending_count;
    uint8_t            stack_id = STACK_ID;

    // Continue to read modem event until all event has been processed
    do
    {
        // Read modem event
        ASSERT_SMTC_MODEM_RC( smtc_modem_get_event( &current_event, &event_pending_count ) );

        switch( current_event.event_type )
        {
        case SMTC_MODEM_EVENT_RESET:
            SMTC_HAL_TRACE_INFO( "Event received: RESET\n" );

#if !defined( USE_LR11XX_CREDENTIALS )
            // Set user credentials
            ASSERT_SMTC_MODEM_RC( smtc_modem_set_deveui( stack_id, user_dev_eui ) );
            ASSERT_SMTC_MODEM_RC( smtc_modem_set_joineui( stack_id, user_join_eui ) );
            ASSERT_SMTC_MODEM_RC( smtc_modem_set_appkey( stack_id, user_gen_app_key ) );
            ASSERT_SMTC_MODEM_RC( smtc_modem_set_nwkkey( stack_id, user_app_key ) );
#else
            // Get internal credentials
            ASSERT_SMTC_MODEM_RC( smtc_modem_get_chip_eui( stack_id, chip_eui ) );
            SMTC_HAL_TRACE_ARRAY( "CHIP_EUI", chip_eui, SMTC_MODEM_EUI_LENGTH );
            ASSERT_SMTC_MODEM_RC( smtc_modem_get_pin( stack_id, chip_pin ) );
            SMTC_HAL_TRACE_ARRAY( "CHIP_PIN", chip_pin, SMTC_MODEM_PIN_LENGTH );
#endif
            // Set user region
            ASSERT_SMTC_MODEM_RC( smtc_modem_set_region( stack_id, MODEM_EXAMPLE_REGION ) );
// Schedule a Join LoRaWAN network
#if defined( USE_RELAY_TX )
            // by default when relay mode is activated , CSMA is also activated by default to at least protect the WOR
            // transmission
            // if you want to disable the csma please uncomment the next line
            // ASSERT_SMTC_MODEM_RC(smtc_modem_csma_set_state (stack_id,false));

            relay_config.second_ch_enable = false;

            // The RelayModeActivation field indicates how the end-device SHOULD manage the relay mode.
            relay_config.activation = SMTC_MODEM_RELAY_TX_ACTIVATION_MODE_DYNAMIC;

            // number_of_miss_wor_ack_to_switch_in_nosync_mode  field indicates that the
            // relay mode SHALL be restart in no sync mode when it does not receive a WOR ACK frame after
            // number_of_miss_wor_ack_to_switch_in_nosync_mode consecutive uplinks.
            relay_config.number_of_miss_wor_ack_to_switch_in_nosync_mode = 3;

            // smart_level field indicates that the
            // relay mode SHALL be enabled if the end-device does not receive a valid downlink after smart_level
            // consecutive uplinks.
            relay_config.smart_level = 8;

            // The BackOff field indicates how the end-device SHALL behave when it does not receive
            // a WOR ACK frame.
            // BackOff Description
            // 0 Always send a LoRaWAN uplink
            // 1..63 Send a LoRaWAN uplink after X WOR frames without a WOR ACK
            relay_config.backoff = 4;
            ASSERT_SMTC_MODEM_RC( smtc_modem_relay_tx_enable( stack_id, &relay_config ) );
#endif
            ASSERT_SMTC_MODEM_RC( smtc_modem_join_network( stack_id ) );
            break;

        case SMTC_MODEM_EVENT_ALARM:
            SMTC_HAL_TRACE_INFO( "Event received: ALARM\n" );
            // Send periodical uplink on port 101
            //send_uplink_counter_on_port( 101 );
            // Restart periodical uplink alarm
            //ASSERT_SMTC_MODEM_RC( smtc_modem_alarm_start_timer( PERIODICAL_UPLINK_DELAY_S ) );
            break;

        case SMTC_MODEM_EVENT_JOINED:
            SMTC_HAL_TRACE_INFO( "Event received: JOINED\n" );
            SMTC_HAL_TRACE_INFO( "Modem is now joined \n" );

            // Send first periodical uplink on port 101
            //send_uplink_counter_on_port( 101 );
            // start periodical uplink alarm
            //ASSERT_SMTC_MODEM_RC( smtc_modem_alarm_start_timer( DELAY_FIRST_MSG_AFTER_JOIN ) );
            break;

        case SMTC_MODEM_EVENT_TXDONE:
            SMTC_HAL_TRACE_INFO( "Event received: TXDONE\n" );
            SMTC_HAL_TRACE_INFO( "Transmission done \n" );
            break;

        case SMTC_MODEM_EVENT_DOWNDATA:
            SMTC_HAL_TRACE_INFO( "Event received: DOWNDATA\n" );
            // Get downlink data
            ASSERT_SMTC_MODEM_RC(
                smtc_modem_get_downlink_data( rx_payload, &rx_payload_size, &rx_metadata, &rx_remaining ) );
            SMTC_HAL_TRACE_PRINTF( "Data received on port %u\n", rx_metadata.fport );
            //SMTC_HAL_TRACE_ARRAY( "Received payload", rx_payload, rx_payload_size );
            break;

        case SMTC_MODEM_EVENT_JOINFAIL:
            SMTC_HAL_TRACE_INFO( "Event received: JOINFAIL\n" );
            break;

        case SMTC_MODEM_EVENT_ALCSYNC_TIME:
            SMTC_HAL_TRACE_INFO( "Event received: ALCSync service TIME\n" );
            break;

        case SMTC_MODEM_EVENT_LINK_CHECK:
            SMTC_HAL_TRACE_INFO( "Event received: LINK_CHECK\n" );
            break;

        case SMTC_MODEM_EVENT_CLASS_B_PING_SLOT_INFO:
            SMTC_HAL_TRACE_INFO( "Event received: CLASS_B_PING_SLOT_INFO\n" );
            break;

        case SMTC_MODEM_EVENT_CLASS_B_STATUS:
            SMTC_HAL_TRACE_INFO( "Event received: CLASS_B_STATUS\n" );
            break;

        case SMTC_MODEM_EVENT_LORAWAN_MAC_TIME:
            SMTC_HAL_TRACE_WARNING( "Event received: LORAWAN MAC TIME\n" );
            break;

        case SMTC_MODEM_EVENT_LORAWAN_FUOTA_DONE:
        {
            bool status = current_event.event_data.fuota_status.successful;
            if( status == true )
            {
                SMTC_HAL_TRACE_INFO( "Event received: FUOTA SUCCESSFUL\n" );
            }
            else
            {
                SMTC_HAL_TRACE_WARNING( "Event received: FUOTA FAIL\n" );
            }
            break;
        }

        case SMTC_MODEM_EVENT_NO_MORE_MULTICAST_SESSION_CLASS_C:
            SMTC_HAL_TRACE_INFO( "Event received: MULTICAST CLASS_C STOP\n" );
            break;

        case SMTC_MODEM_EVENT_NO_MORE_MULTICAST_SESSION_CLASS_B:
            SMTC_HAL_TRACE_INFO( "Event received: MULTICAST CLASS_B STOP\n" );
            break;

        case SMTC_MODEM_EVENT_NEW_MULTICAST_SESSION_CLASS_C:
            SMTC_HAL_TRACE_INFO( "Event received: New MULTICAST CLASS_C \n" );
            break;

        case SMTC_MODEM_EVENT_NEW_MULTICAST_SESSION_CLASS_B:
            SMTC_HAL_TRACE_INFO( "Event received: New MULTICAST CLASS_B\n" );
            break;

        case SMTC_MODEM_EVENT_FIRMWARE_MANAGEMENT:
            SMTC_HAL_TRACE_INFO( "Event received: FIRMWARE_MANAGEMENT\n" );
            if( current_event.event_data.fmp.status == SMTC_MODEM_EVENT_FMP_REBOOT_IMMEDIATELY )
            {
                //smtc_modem_hal_reset_mcu( );
            }
            break;

        case SMTC_MODEM_EVENT_STREAM_DONE:
            SMTC_HAL_TRACE_INFO( "Event received: STREAM_DONE\n" );
            break;

        case SMTC_MODEM_EVENT_UPLOAD_DONE:
            SMTC_HAL_TRACE_INFO( "Event received: UPLOAD_DONE\n" );
            break;

        case SMTC_MODEM_EVENT_DM_SET_CONF:
            SMTC_HAL_TRACE_INFO( "Event received: DM_SET_CONF\n" );
            break;

        case SMTC_MODEM_EVENT_MUTE:
            SMTC_HAL_TRACE_INFO( "Event received: MUTE\n" );
            break;
        case SMTC_MODEM_EVENT_RELAY_TX_DYNAMIC:  //!< Relay TX dynamic mode has enable or disable the WOR protocol
            SMTC_HAL_TRACE_INFO( "Event received: RELAY_TX_DYNAMIC\n" );
            break;
        case SMTC_MODEM_EVENT_RELAY_TX_MODE:  //!< Relay TX activation has been updated
            SMTC_HAL_TRACE_INFO( "Event received: RELAY_TX_MODE\n" );
            break;
        case SMTC_MODEM_EVENT_RELAY_TX_SYNC:  //!< Relay TX synchronisation has changed
            SMTC_HAL_TRACE_INFO( "Event received: RELAY_TX_SYNC\n" );
            break;
        default:
            SMTC_HAL_TRACE_ERROR( "Unknown event %u\n", current_event.event_type );
            break;
        }
    } while( event_pending_count > 0 );
}
