boostversion = 1_38
boostpath = /major/janderso/lib/boost/boost_$(boostversion)_0
boostlibs = $(boostpath)/libs/libboost_program_options-gcc43-mt.a $(boostpath)/libs/libboost_thread-gcc43-mt.a
paths = -I$(boostpath) -L$(boostpath)/lib
exec = zone_linux.exe
objs = zone.o

all: zone.o
	g++ zone.o -Wall -o $(exec) $(paths) $(boostlibs) 

zone.o: zone.cpp zone.h
	g++ zone.cpp -c $(paths)
	
clean: 
	rm -f ${exec} ${objs}
