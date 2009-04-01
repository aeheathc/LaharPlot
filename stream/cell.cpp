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

#include "cell.h"

// TODO: use chained constructors when that functionality comes in C++09
Cell::Cell(float elevation, int yIn, int xIn)
	: height(elevation), flowDir(none), x(xIn), y(yIn), flowTotal(*alloc_inst) {}

Cell::Cell(float elevation, int yIn, int xIn, direction dir)
	: height(elevation), flowDir(dir), x(xIn), y(yIn), flowTotal(*alloc_inst) {}

Cell::Cell() : height(-10000), flowDir(none), x(-1), y(-1), flowTotal(*alloc_inst) {}

Cell::~Cell()
{}

void Cell::fill(float elevation, int y, int x, direction dir)
{
	height = elevation;
	this->y = y;
	this->x = x;
	flowDir = dir;
}

Cell& Cell::operator=(const Cell& src)
{
	height = src.height;
	x = src.x;
	y = src.y;
	flowDir = src.flowDir;
	flowTotal.clear();
	copy( src.flowTotal.begin(), src.flowTotal.end(), flowTotal.begin() );
	return *this;
}

direction intDirection(int dirIn)
{
	switch(dirIn)
	{
		case 0: return north;
		case 1: return northeast;
		case 2: return east;
		case 3: return southeast;
		case 4: return south;
		case 5: return southwest;
		case 6: return west;
		case 7: return northwest;
		case 8: return none;
	}
}

void flow(Point& current, direction dir)
{
	switch(dir)
	{
		case north:
		case northeast:
		case northwest:
		current.row--;
		break;
		
		case southeast:
		case south:
		case southwest:
		current.row++;
		break;
		
		default: break;
	}
	switch(dir)
	{
		case northeast:
		case east:
		case southeast:
		current.column++;
		break;
		
		case southwest:
		case west:
		case northwest:
		current.column--;
		break;
		
		default: break;
	}
}
