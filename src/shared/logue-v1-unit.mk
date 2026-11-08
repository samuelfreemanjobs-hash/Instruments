# Shared Makefile tail for logue SDK v1 user units (NTS-1 / minilogue xd / prologue).
# Include from src/oscillators/<unit>/Makefile or src/effects/<unit>/Makefile.
# tools/build.sh exports PLATFORMDIR to the selected platform tree.

ifndef LOGUE_SDK
$(error LOGUE_SDK is not set — clone korginc/logue-sdk and export LOGUE_SDK)
endif

PROJECTDIR := $(abspath .)
PLATFORMDIR ?= $(LOGUE_SDK)/platform/nutekt-digital
LOGUE_UNIT_KIND ?= osc
LDDIR := $(PLATFORMDIR)/dummy-$(LOGUE_UNIT_KIND)/ld
EXTDIR := $(LOGUE_SDK)/platform/ext
TOOLSDIR := $(LOGUE_SDK)/tools

# Prefer Korg-bundled gcc 5.4 when present; otherwise system arm-none-eabi (apt).
KORG_GCC_BIN := $(TOOLSDIR)/gcc/gcc-arm-none-eabi-5_4-2016q3/bin
ifneq ($(wildcard $(KORG_GCC_BIN)/arm-none-eabi-gcc),)
  GCC_BIN_PATH ?= $(KORG_GCC_BIN)/
else
  GCC_BIN_PATH ?= $(dir $(shell command -v arm-none-eabi-gcc 2>/dev/null))
endif

include $(PLATFORMDIR)/dummy-$(LOGUE_UNIT_KIND)/Makefile
