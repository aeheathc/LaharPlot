/* LaharPlot is an application that can calculate and map out the inundation
   zone of a lahar (volcanic mudslide) given an elevation map and some starting
   parameters about the lahar itself.
   Copyright 2009: Anthony Heathcoat, Jason Anderson, Tim Root

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAIN_H
#define MAIN_H

#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/date_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/multi_array.hpp>
#include <boost/program_options.hpp> 
#include <boost/thread.hpp>

#include <gdal_priv.h>

#include "cell.h"

using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;
namespace ip = boost::interprocess;

//Alias an STL compatible allocator of ints that allocates ints from the managed
//shared memory segment.  This allocator will allow to place containers
//in managed shared memory segments.
typedef ip::allocator<Cell, ip::managed_shared_memory::segment_manager> ShmemAllocator;

typedef boost::multi_array<Cell,2,ShmemAllocator> matrix;

struct Metadata
{
	public:
	double physicalSize, originX, originY;
	string projection;	
};

unsigned int nXSize, nYSize;
bool cmdIn = false, fileOut = false, cmdOut = false, verbose = false;

int main(int argc, char* argv[]);

// Fills the DEM matrix using data provided in linear form.
void linearTo2d(unsigned int firstRow, unsigned int end, ip::managed_shared_memory::handle_t linearHandle);

/* Updates the DEM with correct flow direction information in rows
 firstRow to (end-1). Usually called multiple times in parallel, on different
 parts of the DEM.
*/
void flowDirection(unsigned int firstRow, unsigned int end);
direction greatestSlope(matrix* dem, unsigned int x, unsigned int y, unsigned int radius);

void writeFiles(matrix* dem, fs::ofstream& sdem, fs::ofstream& meta,
				fs::ofstream& fdir, fs::ofstream& ftotal, Metadata& iniData);
void writeStdOut(Metadata iniData);

#endif
