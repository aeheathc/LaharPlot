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
#include <sstream>
#include <string>

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/set.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

#include "util.h"

using namespace std;
namespace ip = boost::interprocess;

class Cell;
class Point;

typedef ip::allocator<Point, ip::managed_shared_memory::segment_manager> PointShmemAllocator;

enum direction{north, northeast, east, southeast, south, southwest, west, northwest, none};

direction intDirection(int dirIn);
void flow(Point& current, direction dir);

//	Cell represents one space on the DEM.
class Cell
{
	public:
	static PointShmemAllocator* alloc_inst;
	
	float height;		//Elevation in meters
	int y,x;			//x,y location of this cell within the DEM
	direction flowDir;	//the direction in which THIS cell flows

	//the list of cells that flow into this one.
	ip::set<Point, Pred, PointShmemAllocator> flowTotal;

	void fill(float elevation, int y, int x, direction dir = none);	
	
	Cell(float elevation, int y, int x);
	Cell(float elevation, int y, int x, direction dir);
	Cell();
	~Cell();
	
	Cell& operator=(const Cell& src);	
};

class Point
{
	public:
	int row,column;
	Point(int rIn, int cIn): row(rIn),column(cIn) {}
	Point(): row(-30000),column(-30000) {}
	Point(const Point& src): row(src.row),column(src.column) {}
	Point& operator=(const Point& src)
		{row = src.row; column = src.column; return *this;}
	bool operator==(const Point& src) const
		{return row==src.row && column==src.column;}
	bool operator<(const Point& src) const
		{return (row+column)<(src.row+src.column);}
};

//Use the values of a Point as a 2D index into a 1D array.
template<typename T>
T& linear(T *array, const Point& x, int width = -1)
{
	return linear(array, x.row, x.column, width);
}

#endif
