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
	: height(elevation), y(yIn), x(xIn), flowDir(none), flowTotal(0), flowTotalReady(false)
{}

Cell::Cell(float elevation, int yIn, int xIn, direction dir)
	: height(elevation), y(yIn), x(xIn), flowDir(dir), flowTotal(0), flowTotalReady(false)
{}

Cell::Cell()
	: height(-10000), y(-1), x(-1), flowDir(none), flowTotal(0), flowTotalReady(false)
{}

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
	flowTotal = src.flowTotal;
	flowTotalReady = src.getFlowTotalReady();
	return *this;
}

bool Cell::getFlowTotalReady() const {return flowTotalReady;}

unsigned long long Cell::getFlowTotal()
{
	if(!flowTotalReady)
	{
		accumulate();
		flowTotalReady = true;
	}
	return flowTotal;
}

void Cell::accumulate()
{
	ostringstream oss;
	oss << "Calling accumulate on " << y << ',' << x << '\n';  //debug
	//cout << oss.str();
	
	Cell *adj = NULL;							//an adjacent cell
	if(y > 0)
	{
		adj = this - (sizeof(Cell) * cellsX);		//north
		if(adj->flowDir == south)		flowTotal += adj->getFlowTotal();
	}
	if(y > 0 && x < (cellsX-1))
	{
		adj = this - (sizeof(Cell) * (cellsX-1));	//northeast
		if(adj->flowDir == southwest)	flowTotal += adj->getFlowTotal();
	}
	if(x < (cellsX-1))
	{
		adj = this + sizeof(Cell);					//east
		if(adj->flowDir == west)  		flowTotal += adj->getFlowTotal();
	}
	if(x < (cellsX-1) && y < (cellsY-1))
	{
		adj = this + (sizeof(Cell) * (cellsX+1));	//southeast
		if(adj->flowDir == northwest)	flowTotal += adj->getFlowTotal();
	}
	if(y < (cellsY-1))
	{
		adj = this + (sizeof(Cell) * cellsX);		//south
		if(adj->flowDir == north)		flowTotal += adj->getFlowTotal();
	}
	if(x>0 && y < (cellsY-1))
	{
		adj = this + (sizeof(Cell) * (cellsX-1));	//southwest
		if(adj->flowDir == northeast)	flowTotal += adj->getFlowTotal();
	}
	if(x>0)
	{
		adj = this - sizeof(Cell);					//west
		if(adj->flowDir == east)		flowTotal += adj->getFlowTotal();
	}
	if(y>0 && x>0)
	{
		adj = this - (sizeof(Cell) * (cellsX+1));	//northwest
		if(adj->flowDir == southeast)	flowTotal += adj->getFlowTotal();
	}
}

int Cell::cellsY;
int Cell::cellsX;

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
	return none;
}

