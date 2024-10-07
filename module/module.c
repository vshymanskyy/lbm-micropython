/**
 * Author:    Volodymyr Shymanskyy
 * Created:   24.09.2024
 **/

#include "lbm_mpy.h"
#include "smtc_modem_api.h"
#include "smtc_modem_utilities.h"
#include "smtc_modem_relay_api.h"

// Define the type for LoRaWAN
mp_obj_full_type_t mp_type_lorawan;

// This is the internal state of a LoRaWAN instance.
typedef struct {
    mp_obj_base_t base;
    mp_int_t stack_id;
} mp_obj_lorawan_t;

// Essentially LoRaWAN.__new__ (and __init__).
// Takes up to two arguments (the values of attr1 and attr2).
static mp_obj_t lorawan_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 2, false);

    mp_obj_lorawan_t *o = mp_obj_malloc(mp_obj_lorawan_t, type);
    o->stack_id = 0;

    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t lorawan_poll(mp_obj_t self_obj) {
    uint32_t sleep_time_ms = smtc_modem_run_engine();
    return mp_obj_new_int(sleep_time_ms);
}

static MP_DEFINE_CONST_FUN_OBJ_1(lorawan_poll_obj, lorawan_poll);

static mp_obj_t lorawan_joined(mp_obj_t self_obj) {
    mp_obj_lorawan_t *self = MP_OBJ_TO_PTR(self_obj);
    smtc_modem_status_mask_t status_mask = 0;
    smtc_modem_get_status(self->stack_id, &status_mask);
    return mp_obj_new_bool((status_mask & SMTC_MODEM_STATUS_JOINED) == SMTC_MODEM_STATUS_JOINED);
}

static MP_DEFINE_CONST_FUN_OBJ_1(lorawan_joined_obj, lorawan_joined);

// Locals dict for the LoRaWAN type
mp_map_elem_t lorawan_locals_dict_table[2];
static MP_DEFINE_CONST_DICT(lorawan_locals_dict, lorawan_locals_dict_table);

// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY

    smtc_modem_init( &modem_event_callback );

    // Initialise the LoRaWAN type.
    mp_type_lorawan.base.type = (void*)&mp_type_type;
    mp_type_lorawan.flags = MP_TYPE_FLAG_NONE;
    mp_type_lorawan.name = MP_QSTR_LoRaWAN;
    MP_OBJ_TYPE_SET_SLOT(&mp_type_lorawan, make_new, lorawan_make_new, 0);
    lorawan_locals_dict_table[0] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_joined), MP_OBJ_FROM_PTR(&lorawan_joined_obj) };
    lorawan_locals_dict_table[1] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_poll), MP_OBJ_FROM_PTR(&lorawan_poll_obj) };
    MP_OBJ_TYPE_SET_SLOT(&mp_type_lorawan, locals_dict, (void*)&lorawan_locals_dict, 1);

    // Make the LoRaWAN type available on the module.
    mp_store_global(MP_QSTR_LoRaWAN, MP_OBJ_FROM_PTR(&mp_type_lorawan));

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}
