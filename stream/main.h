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
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/multi_array.hpp>
#include <boost/program_options.hpp> 
#include <boost/thread.hpp>

#include <gdal_priv.h>

#include "cell.h"
#include "util.h"
#include "fill.h"

using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;
namespace ip = boost::interprocess;

struct Metadata
{
	public:
	double physicalSize, originX, originY;
	string projection;	
};

int nXSize, nYSize;
bool cmdIn = false, fileOut = false, cmdOut = false, verbose = false;

int main(int argc, char* argv[]);

// Fills the DEM matrix using data provided in linear form.
void linearTo2d(int firstRow, int end, ip::managed_shared_memory::handle_t linearHandle);

/*	Updates the DEM with correct flow direction information in rows
	firstRow to (end-1). Usually called multiple times in parallel, on different
	parts of the DEM.
*/
void flowDirection(int firstRow, int end);
direction greatestSlope(Cell* dem, int x, int y);

/*	Creates flow records for the Flow Total Grid, with starting cells in rows
	firstRow to (end-1). Usually called multiple times in parallel, on different
	parts of the DEM.
*/
void flowTrace(int firstRow, int end);
void follow(Cell* dem, int row, int column);
class BadFlowGrid : public exception
{
	public:
	virtual const char *what() const throw()
		{return "Flow direction grid provides impossible situation.";}
} oneBFG;

void writeFiles(Cell* dem, fs::ofstream& sdem, fs::ofstream& meta,
				fs::ofstream& fdir, fs::ofstream& ftotal, Metadata& iniData);
void writeStdOut(Metadata iniData);

#endif
