CXXFLAGS = -Wall -Wno-long-long -O2 $(PATHS)

bin_PROGRAMS = stream
stream_LDADD = -lboost_filesystem -lboost_system -lboost_thread -lboost_program_options -lboost_date_time -lgdal $(LINUXLIBS)
stream_LDFLAGS = $(PSFLAGS)
stream_SOURCES = cell.cpp cell.h fill.cpp fill.h main.cpp main.h util.cpp util.h
