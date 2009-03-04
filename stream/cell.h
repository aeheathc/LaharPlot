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

#include <sstream>
#include <string>

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/set.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

using namespace std;
namespace ip = boost::interprocess;

class Cell;
class Point;

typedef ip::allocator<Point, ip::managed_shared_memory::segment_manager> PointShmemAllocator;

enum direction{north, northeast, east, southeast, south, southwest, west, northwest, none};

direction intDirection(int dirIn);

class Point
{
	public:
	int x,y;
	Point(int xIn, int yIn): x(xIn),y(yIn) {}
	//Point(): x(-1),y(-1) {}
};

//	Cell represents one space on the DEM.
class Cell
{
	public:
	float height;		//Elevation in meters
	Point location;		//x,y location of this cell within the DEM
	direction flowDir;	//the direction in which THIS cell flows
	
	/*
		Grab a reference to the list of cells that flow into this one.
		Guarantee enough space to add a Point.
	*/
	ip::set<Point,PointShmemAllocator>& flowTotal();
	//same as above, but ensure there is extra space to add reserveSpace points
	ip::set<Point,PointShmemAllocator>& flowTotal(int reserveSpace);
	
	Cell(float elevation, int x, int y);
	Cell(float elevation, direction dir, int x, int y);
	Cell();
	~Cell();
	
	private:
	//The name of the shared memory space that holds the total flow list
	string shmemName;
};

#endif
