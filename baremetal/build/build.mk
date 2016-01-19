MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

EBBRT_TARGET := serialize
EBBRT_APP_OBJECTS := serialize.o Printer.o
EBBRT_APP_VPATH := $(abspath $(MYDIR)../src)
EBBRT_CONFIG := $(abspath $(MYDIR)../src/ebbrtcfg.h)

EBBRT_APP_INCLUDES += -I $(MYDIR)../ext
EBBRT_APP_LINK += -L $(MYDIR)libs -lboost_system-gcc-1_54 -lboost_serialization-gcc-1_54

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrtbaremetal.mk)
