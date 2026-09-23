##############################################################################
# Configuration for Makefile — jp8000_supersaw
#

PROJECT := jp8000_supersaw
PROJECT_TYPE := osc

UCSRC = header.c
UCXXSRC = unit.cc

UINCDIR += ../common
ULIBDIR =
ULIBS = -lm
UDEFS =
