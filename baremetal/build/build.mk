MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

EBBRT_TARGET := SerializeEbb
EBBRT_APP_OBJECTS := SerializeEbb.o
EBBRT_APP_VPATH := $(abspath $(MYDIR)../src)
EBBRT_CONFIG := $(abspath $(MYDIR)../src/ebbrtcfg.h)

EBBRT_APP_INCLUDES += -I $(MYDIR)../ext
EBBRT_APP_LINK += -L $(MYDIR)libs -lboost_system-gcc-1_54 -lboost_serialization-gcc-1_54

#needed to disable warning as error flags
EBBRT_APP_CPPFLAGS = -Wno-unused-local-typedefs

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrtbaremetal.mk)
