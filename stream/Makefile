boostversion = 1_38
boostpath = /major/aeheathc/lib/boost/boost_$(boostversion)_0
boostToolset = gcc43
boostlibs = -lboost_filesystem-$(boostToolset)-mt \
			-lboost_thread-$(boostToolset)-mt \
			-lboost_program_options-$(boostToolset)-mt \
			-lboost_date_time-$(boostToolset)-mt \
			-lboost_system-$(boostToolset)-mt
			
linuxlibs = -pthread -lcurl -lpng -ljpeg -lgif

#we don't use all the recommended warns just because the 3rd party libraries
#generate a ton of warnings due to poor coding standards
warns = -Wall -ansi -Wcast-align -Wcast-qual -Wchar-subscripts -Winline \
		-Wpointer-arith -Wwrite-strings -Wno-long-long

paths = -I/major/aeheathc/lib/gdal/include -L/major/aeheathc/lib/gdal/lib \
		-I$(boostpath) -L$(boostpath)/lib

		
all: main.cpp cell.o
	g++ main.cpp cell.o $(warns) -o stream  $(paths) -lgdal $(boostlibs) $(linuxlibs)

cell.o: cell.cpp cell.h
	g++ cell.cpp -c $(paths)

clean:
	rm *.o
	rm stream
