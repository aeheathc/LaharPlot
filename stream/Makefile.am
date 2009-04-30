CXXFLAGS = -Wall -Wno-long-long -O2 $(PATHS)

bin_PROGRAMS = stream
stream_LDADD = -lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt -lboost_program_options-mt -lboost_date_time-mt -lgdal $(LINUXLIBS)
stream_LDFLAGS = $(PSFLAGS)
stream_SOURCES = cell.cpp cell.h fill.cpp fill.h main.cpp main.h util.cpp util.h
