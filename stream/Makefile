#we don't use all the recommended warns just because our libraries generate a
#ton of warnings due to poor coding standards
warns = -Wall -ansi -Wcast-align -Wcast-qual -Wchar-subscripts -Winline \
		-Wpointer-arith -Wredundant-decls -Wwrite-strings

paths = -I/major/aeheathc/lib/gdal/include -L/major/aeheathc/lib/gdal/lib \
		-L/major/aeheathc/lib/boost/libs		

		
all: main.cpp cell.o
	g++ main.cpp cell.o -static $(warns) -o stream  $(paths) -lgdal -lboost_filesystem-mt-s

cell.o: cell.cpp cell.h
	g++ -c cell.cpp

clean:
	rm *.o
	rm stream