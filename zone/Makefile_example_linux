#To compile zone.cpp, you will need:
#  - Boost's compiled Program Options and Thread libraries.
#  - Supply a path to Boost's root directory.

# !-!-! Set your current version of Boost !-!-!
boostversion = 1_38_0

# !-!-! Set Boost's Path !-!-!
boostpath = /major/janderso/lib/boost/boost_$(boostversion)

# !-!-! Set Boost's Library Path !-!-!
boostlibpath = /major/janderso/lib/boost/boost_$(boostversion)

# !-!-! Set Boost's Libraries !-!-!
boostlibs = $(boostpath)/libs/libboost_program_options-gcc43-mt.a $(boostpath)/libs/libboost_thread-gcc43-mt.a
 
 
#----- No changes necessary beyond this point -----
paths = -I$(boostpath) -L$(boostlibpath) exec = zone_linux.exe
objs = zone.o

all: zone.o
	g++ zone.o -Wall -o $(exec) $(paths) $(boostlibs) -lpthread

zone.o: zone.cpp zone.h
	g++ zone.cpp -c $(paths)
	
clean: 
	rm -f ${exec} ${objs}
