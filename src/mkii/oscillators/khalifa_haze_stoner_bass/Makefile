##############################################################################
# NTS-1 mkII unit — builds against LOGUE_SDK platform tree.

LOGUE_SDK ?= $(error LOGUE_SDK is not set)
NTS1_MKII := $(LOGUE_SDK)/platform/nts-1_mkii

PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

COMMON_INC_PATH := $(NTS1_MKII)/common
COMMON_SRC_PATH := $(NTS1_MKII)/common
LDDIR := $(NTS1_MKII)/ld
EXTDIR := $(LOGUE_SDK)/platform/ext
TOOLSDIR := $(LOGUE_SDK)/tools

include $(NTS1_MKII)/dummy-osc/Makefile
