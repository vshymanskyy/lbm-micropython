# Author:    Volodymyr Shymanskyy
# Created:   30.09.2024

# Location of MicroPython top-level directory
MPY_DIR ?= /opt/micropython
LBM_VER ?= 4.5.0
LBM_URL ?= "https://github.com/Lora-net/SWL2001/archive/refs/tags"
LBM_REGION ?= "EU_868,US_915"

BUILD ?= build

# x86, x64, armv6m, armv7m, armv7emsp, armv7emdp, xtensa, xtensawin
ARCH ?= xtensawin

MOD ?= lbm

SRC += module/module.c          \
       module/modem_cbk.c       \
       module/libc.c            \
       module/smtc_modem_hal.c  \
       module/ral_sx126x_bsp.c  \
       module/sx126x_hal.c


ifeq ($(ARCH),xtensa)
  SRC += module/esp8266-rom.S
else ifeq ($(ARCH),x86)
  SRC += module/libc_x86.c
else ifeq ($(ARCH),x64)
  SRC += module/libc_x86.c
endif

MICROPY_LINK_RUNTIME = 1

include $(MPY_DIR)/py/dynruntime.mk

MPY_LD_FLAGS += -llbm_lib/$(BUILD)/basic_modem.a

CLEAN_EXTRA += lbm_lib/$(BUILD)

CFLAGS += -Os -Imodule -Wno-unused-value -Wno-unused-function \
          -Wno-unused-variable -Wno-unused-but-set-variable -Wwrite-strings \
          -Ilbm_lib/smtc_modem_api -Ilbm_lib/smtc_modem_hal \
          -Ilbm_lib/smtc_modem_core/smtc_ral/src \
          -Ilbm_lib/smtc_modem_core/radio_drivers/sx126x_driver/src

# Build LoRa Basic Modem before anything else
module/module.c: lbm_lib/$(BUILD)/basic_modem.a

lbm_lib/$(BUILD)/basic_modem.a: lbm_lib FORCE_LBM_BUILD
	$(Q)$(MAKE) --no-print-directory -C lbm_lib basic_modem_sx1262 BUILD_ROOT="$(BUILD)" \
		PREFIX="$(CROSS)" MCU_FLAGS="$(MICROPY_ARCH_CFLAGS)" EXTRAFLAGS="-Werror -DNDEBUG" \
		REGION="$(LBM_REGION)" MODEM_TRACE="no" VERBOSE="no"

lbm_lib:
	$(ECHO) "DL lbm $(LBM_VER)"
	$(Q)python3 ./extra/lbm-download.py $(LBM_URL) $(LBM_VER)
	$(Q)patch -d lbm_lib -fp1 < ./extra/lbm-$(LBM_VER).patch

FORCE_LBM_BUILD: ;
