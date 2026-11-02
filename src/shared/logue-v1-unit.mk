# Shared Makefile tail for NTS-1 mkI / nutekt-digital user units (logue SDK v1 layout).
# Include from src/oscillators/<unit>/Makefile or src/effects/<unit>/Makefile.

ifndef LOGUE_SDK
$(error LOGUE_SDK is not set — clone korginc/logue-sdk and export LOGUE_SDK)
endif

NUTEKT_DIGITAL := $(LOGUE_SDK)/platform/nutekt-digital
PROJECTDIR := $(abspath .)
PLATFORMDIR := $(NUTEKT_DIGITAL)
LDDIR := $(NUTEKT_DIGITAL)/dummy-osc/ld
EXTDIR := $(LOGUE_SDK)/platform/ext
TOOLSDIR := $(LOGUE_SDK)/tools

# Prefer Korg-bundled gcc 5.4 when present; otherwise system arm-none-eabi (apt).
KORG_GCC_BIN := $(TOOLSDIR)/gcc/gcc-arm-none-eabi-5_4-2016q3/bin
ifneq ($(wildcard $(KORG_GCC_BIN)/arm-none-eabi-gcc),)
  GCC_BIN_PATH ?= $(KORG_GCC_BIN)/
else
  GCC_BIN_PATH ?= $(dir $(shell command -v arm-none-eabi-gcc 2>/dev/null))
endif

include $(NUTEKT_DIGITAL)/dummy-osc/Makefile
