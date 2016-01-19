MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

app_sources := serialize.cc SerializeEbb.cc 

target := SerializeEbb

EBBRT_APP_LINK += -lboost_system -lboost_serialization

include $(abspath $(EBBRT_SRCDIR)/apps/ebbrthosted.mk)
