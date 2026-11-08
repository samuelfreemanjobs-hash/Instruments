# Prefer Korg-bundled gcc 10.3; fall back to system arm-none-eabi (apt / CI).
KORG_MKII_GCC_BIN := $(TOOLSDIR)/gcc/gcc-arm-none-eabi-10.3-2021.10/bin
ifneq ($(wildcard $(KORG_MKII_GCC_BIN)/arm-none-eabi-gcc),)
  GCC_BIN_PATH ?= $(KORG_MKII_GCC_BIN)
else
  SYS_ARM_GCC := $(shell command -v arm-none-eabi-gcc 2>/dev/null)
  ifneq ($(SYS_ARM_GCC),)
    GCC_BIN_PATH ?= $(dir $(SYS_ARM_GCC))
  endif
endif
