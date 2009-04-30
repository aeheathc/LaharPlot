CXXFLAGS = -Wall -Wno-long-long -O2 $(PATHS)

bin_PROGRAMS = zone
zone_LDADD = -lboost_system-mt -lboost_thread-mt -lboost_program_options-mt
zone_LDFLAGS = $(PSFLAGS)
zone_SOURCES = zone.cpp zone.h
