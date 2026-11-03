# Shared logue SDK v1.1 makefile for NTS-1 (nutekt-digital) units.
# Included from each oscillator directory. Paths are relative to that directory.

LOGUE_MK_DIR := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
SDKDIR ?= $(LOGUE_MK_DIR)/../../.deps/logue-sdk
PLATFORMDIR ?= $(SDKDIR)/platform/nutekt-digital
CMSISDIR ?= $(SDKDIR)/platform/ext/CMSIS/CMSIS
LDDIR ?= $(PLATFORMDIR)/dummy-osc/ld
UNIT_TPL ?= $(PLATFORMDIR)/dummy-osc/tpl/_unit.c
GCC_BIN_PATH ?= $(SDKDIR)/tools/gcc/gcc-arm-none-eabi-5_4-2016q3/bin

SHELL := C:/Users/user/AppData/Local/hermes/git/bin/bash.exe
.SHELLFLAGS := -c

include project.mk

MCU := cortex-m4
MCU_MODEL := STM32F446xE
GCC_TARGET := arm-none-eabi-

CC   := $(GCC_BIN_PATH)/$(GCC_TARGET)gcc
CXXC := $(GCC_BIN_PATH)/$(GCC_TARGET)g++
LD   := $(GCC_BIN_PATH)/$(GCC_TARGET)gcc
CP   := $(GCC_BIN_PATH)/$(GCC_TARGET)objcopy
OD   := $(GCC_BIN_PATH)/$(GCC_TARGET)objdump
SZ   := $(GCC_BIN_PATH)/$(GCC_TARGET)size
HEX  := $(CP) -O ihex
BIN  := $(CP) -O binary

RULESPATH := $(LDDIR)
LDSCRIPT := $(LDDIR)/userosc.ld
DLIBS := -lm
DADEFS := -D$(MCU_MODEL) -DCORTEX_USE_FPU=TRUE -DARM_MATH_CM4
DDEFS := -D$(MCU_MODEL) -DCORTEX_USE_FPU=TRUE -DARM_MATH_CM4 -D__FPU_PRESENT
COPT := -std=c11 -mstructure-size-boundary=8
CXXOPT := -std=c++11 -fno-rtti -fno-exceptions -fno-non-call-exceptions
LDOPT := -Xlinker --just-symbols=$(LDDIR)/osc_api.syms
CWARN := -W -Wall -Wextra
CXXWARN :=
FPU_OPTS := -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant -fcheck-new
OPT := -g -Os -mlittle-endian $(FPU_OPTS)
TOPT := -mthumb -mno-thumb-interwork -DTHUMB_NO_INTERWORKING -DTHUMB_PRESENT

PKGARCH := $(PROJECT).ntkdigunit
BUILDDIR := build
OBJDIR := $(BUILDDIR)/obj
LSTDIR := $(BUILDDIR)/lst

CSRC := $(UNIT_TPL)
CXXSRC := $(UCXXSRC)

COBJS := $(OBJDIR)/_unit.o
CXXOBJS := $(addprefix $(OBJDIR)/, $(notdir $(CXXSRC:.cc=.o)))
OBJS := $(COBJS) $(CXXOBJS)

DINCDIR := $(PLATFORMDIR)/inc $(PLATFORMDIR)/inc/dsp $(PLATFORMDIR)/inc/utils $(CMSISDIR)/Include
INCDIR := $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
DEFS := $(DDEFS) $(UDEFS)
MCFLAGS := -mcpu=$(MCU)
CFLAGS = $(MCFLAGS) $(TOPT) $(OPT) $(COPT) $(CWARN) -Wa,-alms=$(LSTDIR)/$(notdir $(<:.c=.lst)) $(DEFS)
CXXFLAGS = $(MCFLAGS) $(TOPT) $(OPT) $(CXXOPT) $(CXXWARN) -Wa,-alms=$(LSTDIR)/$(notdir $(<:.cc=.lst)) $(DEFS)
LDFLAGS := $(MCFLAGS) $(TOPT) $(OPT) -nostartfiles $(patsubst %,-I%,$(ULIBDIR)) -Wl,-Map=$(BUILDDIR)/$(PROJECT).map,--cref,--no-warn-mismatch,--library-path=$(RULESPATH),--script=$(LDSCRIPT) $(LDOPT)
LIBS := $(DLIBS) $(ULIB)

OUTFILES := $(BUILDDIR)/$(PROJECT).elf $(BUILDDIR)/$(PROJECT).bin $(BUILDDIR)/$(PROJECT).list

all: $(OUTFILES)
	@echo Done

$(OBJS): | $(BUILDDIR) $(OBJDIR) $(LSTDIR)

$(BUILDDIR) $(OBJDIR) $(LSTDIR):
	@mkdir -p $@

$(OBJDIR)/_unit.o: $(UNIT_TPL) | $(OBJDIR) $(LSTDIR)
	@echo Compiling _unit.c
	@$(CC) -c $(CFLAGS) -I. $(INCDIR) $< -o $@

$(OBJDIR)/%.o: %.cc | $(OBJDIR) $(LSTDIR)
	@echo Compiling $(<F)
	@$(CXXC) -c $(CXXFLAGS) -I. $(INCDIR) $< -o $@

$(BUILDDIR)/$(PROJECT).elf: $(OBJS) $(LDSCRIPT)
	@echo Linking $@
	@$(LD) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

$(BUILDDIR)/$(PROJECT).bin: $(BUILDDIR)/$(PROJECT).elf
	@echo Creating $@
	@$(BIN) $< $@
	@$(SZ) $<

$(BUILDDIR)/$(PROJECT).list: $(BUILDDIR)/$(PROJECT).elf
	@$(OD) -S $< > $@

install: $(OUTFILES)
	@powershell.exe -NoProfile -ExecutionPolicy Bypass -File "$(LOGUE_MK_DIR)/pack-unit.ps1" -Project "$(PROJECT)" -BuildDir "$(CURDIR)/$(BUILDDIR)" -Manifest "$(CURDIR)/manifest.json" -OutDir "$(CURDIR)"

clean:
	@rm -rf $(BUILDDIR) $(PKGARCH)
