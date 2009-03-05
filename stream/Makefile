boostversion = 1_38
boostpath = /major/aeheathc/lib/boost/boost_$(boostversion)_0
boostlibs = -lboost_filesystem-mt-s -lboost_thread-mt-s -lboost_program_options-mt-s -lboost_date_time-mt-s

#we don't use all the recommended warns just because the 3rd party libraries
#generate a ton of warnings due to poor coding standards
warns = -Wall -ansi -Wcast-align -Wcast-qual -Wchar-subscripts -Winline \
		-Wpointer-arith -Wwrite-strings -Wno-long-long

paths = -I/major/aeheathc/lib/gdal/include -L/major/aeheathc/lib/gdal/lib \
		-I$(boostpath) -L$(boostpath)/libs

		
all: main.cpp cell.o
	g++ main.cpp cell.o -static $(warns) -o stream  $(paths) -lgdal $(boostlibs)

cell.o: cell.cpp cell.h
	g++ cell.cpp -c $(paths)

clean:
	rm *.o
	rm stream
