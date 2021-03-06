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
#include <boost/program_options.hpp> 
#include <boost/thread.hpp>

#include <gdal_priv.h>

#include "cell.h"
#include "util.h"
#include "fill.h"

using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

struct Metadata
{
	public:
	double physicalSize, originX, originY;
	string projection;	
};

extern Logger lg;
extern Cell *dem;
float *pafScanline;
fs::ofstream *sDem, *meta, *flowDir, *flowTotal;

bool cmdIn = false, fileOut = false, cmdOut = false, sendEOF = false;

int main(int argc, char* argv[]);

// Fills the DEM matrix using data provided in linear form.
void linearTo2d(int firstRow, int end);

/*	Creates flow records for the Flow Total Grid in a certain part of the DEM.
	Usually called multiple times in parallel, on different
	parts of the DEM.
	start and end refer to positions in a linear collection of all the
	EDGE cells of the DEM.
*/
void flowTrace(unsigned long start, unsigned long end);

void writeSdem();
void writeMeta(Metadata& iniData);
void writeFlowDir();
void writeFlowTotal();

void writeStdOut(Metadata& iniData);

#endif
