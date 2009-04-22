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

#ifndef CELL_H
#define CELL_H

#include <cmath>

#include <algorithm>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

#include "util.h"

using namespace std;

class Cell;

enum direction{north, northeast, east, southeast, south, southwest, west, northwest, none};
enum FlowMethod{cross, direct, dummy, lowest};

direction intDirection(int dirIn);

extern Cell *dem;
extern Logger lg;

//	Cell represents one space on the DEM.
class Cell
{
	public:
	static int cellsY, cellsX;
		
	float height;		//Elevation in meters
	int y,x;			//location of this cell within the DEM
	unsigned long long flowTotal;

	Cell(float elevation, int y, int x);
	Cell(float elevation, int y, int x, direction dir);
	Cell();
	~Cell();
	
	bool getFlowTotalReady() const;
	unsigned long long getFlowTotal();
	void setFlowDir(direction dir);
	direction getFlowDir();

	//fill out the basic data for this cell
	void fill(float elevation, int y, int x, direction dir = none);
	/* Calculate and store the flow total for this cell. Cascades out to all
		cells that flow into this one.
	*/
	void accumulate();
	/* Get all *possible* flow directions for this cell. If a single one has
		already been chosen, the set only contains that direction.
	*/
	const set<direction>& flowDirs(int radius = 1, FlowMethod method = cross);
	
	private:
	bool flowTotalReady;
	bool accumulated;
	set<direction>* flowDirSet;	//all possible directions where this cell can flow
	direction flowDir;	//the direction in which THIS cell flows
	boost::mutex flowDirs_mutex, accumulate_mutex;
};

#endif
