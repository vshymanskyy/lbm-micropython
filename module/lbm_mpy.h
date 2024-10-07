#include "py/dynruntime.h"

#define STACK_ID 0

#define MODEM_EXAMPLE_REGION SMTC_MODEM_REGION_EU_868

#define SMTC_HAL_TRACE_INFO(...)
#define SMTC_HAL_TRACE_ERROR(...)
#define SMTC_HAL_TRACE_WARNING(...)
#define SMTC_HAL_TRACE_MSG_COLOR(...)
#define SMTC_HAL_TRACE_PRINTF(...)

void modem_event_callback( void );
