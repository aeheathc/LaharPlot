boostversion = 1_38
boostpath = /major/aeheathc/lib/boost/boost_$(boostversion)_0
boostToolset = gcc43
boostlibs = $(boostpath)/lib/libboost_filesystem-$(boostToolset)-mt.a \
			$(boostpath)/lib/libboost_thread-$(boostToolset)-mt.a \
			$(boostpath)/lib/libboost_program_options-$(boostToolset)-mt.a \
			$(boostpath)/lib/libboost_date_time-$(boostToolset)-mt.a \
			$(boostpath)/lib/libboost_system-$(boostToolset)-mt.a
			
linuxlibs = -pthread -lcurl -lpng -ljpeg -lgif

#we don't use all the recommended warns just because the 3rd party libraries
#generate a ton of warnings due to poor coding standards
warns = -Wall -ansi -Wcast-align -Wcast-qual -Wchar-subscripts -Winline \
		-Wpointer-arith -Wwrite-strings -Wno-long-long

paths = -I/major/aeheathc/lib/gdal/include -L/major/aeheathc/lib/gdal/lib \
		-I$(boostpath) -L$(boostpath)/lib

all: main.cpp main.h cell.o fill.o util.h
	g++ main.cpp cell.o fill.o $(warns) -o stream  $(paths) -lgdal $(boostlibs) $(linuxlibs)

fill.o: fill.cpp fill.h util.h
	g++ fill.cpp -c

cell.o: cell.cpp cell.h
	g++ cell.cpp -c $(paths)

clean:
	rm *.o
	rm stream

