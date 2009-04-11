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
	lg.write(debug, oss.str());
	
	Cell *adj = NULL;							//an adjacent cell
	if(y > 0)
	{
		adj = &linear(dem,y-1,x);	//north
		if(adj->flowDir == south)
		{
			oss.str("");
			oss << "Going north to " << adj->y << ',' << adj->x << '\n';
			lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}
	}
	if(y > 0 && x < (cellsX-1))
	{
		adj = &linear(dem,y-1,x+1);	//northeast
		if(adj->flowDir == southwest)
		{
			oss.str("");
			oss << "Going northeast to " << adj->y << ',' << adj->x << '\n';
			lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}
	}
	if(x < (cellsX-1))
	{
		adj = &linear(dem,y,x+1);	//east
		if(adj->flowDir == west)
		{
			oss.str("");
			oss << "Going east to " << adj->y << ',' << adj->x << '\n';
			lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}
	}
	if(x < (cellsX-1) && y < (cellsY-1))
	{
		adj = &linear(dem,y+1,x+1);	//southeast
		if(adj->flowDir == northwest)
		{
			oss.str("");
			oss << "Going southeast to " << adj->y << ',' << adj->x << '\n';
			lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}
	}
	if(y < (cellsY-1))
	{
		adj = &linear(dem,y+1,x);	//south
		if(adj->flowDir == north)
		{
			oss.str("");
			oss << "Going south to " << adj->y << ',' << adj->x << '\n';
			lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}	
	}
	if(x>0 && y < (cellsY-1))
	{
		adj = &linear(dem,y+1,x-1);	//southwest
		if(adj->flowDir == northeast)
		{
			oss.str("");
			oss << "Going southwest to " << adj->y << ',' << adj->x << '\n';
			lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}
	}
	if(x>0)
	{
		adj = &linear(dem,y,x-1);	//west
		if(adj->flowDir == east)
		{
			oss.str("");
			oss << "Going west to " << adj->y << ',' << adj->x << '\n';
			lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}
	}
	if(y>0 && x>0)
	{
		adj = &linear(dem,y-1,x-1);	//northwest
		if(adj->flowDir == southeast)
		{
			oss.str("");
			oss << "Going northwest to " << adj->y << ',' << adj->x << '\n';
			lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}
	}
	oss.str("");
	oss << "Done calling accumulate on " << y << ',' << x << '\n';
	lg.write(debug, oss.str());
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

