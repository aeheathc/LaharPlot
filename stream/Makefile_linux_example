boostlibs = -lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt -lboost_program_options-mt -lboost_date_time-mt
			
linuxlibs = -pthread -lcurl -lpng -ljpeg -lgif

#we don't use all the recommended warns just because the 3rd party libraries
#generate a ton of warnings due to poor coding standards
warns = -Wall -ansi -Wcast-align -Wcast-qual -Wchar-subscripts -Winline \
		-Wpointer-arith -Wwrite-strings -Wno-long-long

paths = -I/major/aeheathc/lib/gdal/include -L/major/aeheathc/lib/gdal/lib \
		-I/major/aeheathc/lib/boost -L/major/aeheathc/lib/boost/lib

all: main.cpp main.h cell.o fill.o util.o
	g++ main.cpp cell.o fill.o util.o $(warns) -static -o stream  $(paths) -lgdal $(boostlibs) $(linuxlibs)

fill.o: fill.cpp fill.h util.h
	g++ fill.cpp -c $(warns) $(paths)

cell.o: cell.cpp cell.h util.h
	g++ cell.cpp -c $(warns) $(paths)

util.o: util.h util.cpp
	g++ util.cpp -c $(warns) $(paths)

clean:
	rm *.o
	rm stream
