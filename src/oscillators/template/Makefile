# Point PLATFORMDIR at your logue-sdk platform folder, or set LOGUE_SDK.
ifndef PLATFORMDIR
  ifdef LOGUE_SDK
    PLATFORMDIR := $(LOGUE_SDK)/platform/nutekt-digital
  else
    PLATFORMDIR := ../../../../logue-sdk/platform/nutekt-digital
  endif
endif

include $(PLATFORMDIR)/Makefile.inc
