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
	: height(elevation), y(yIn), x(xIn), flowTotal(0), flowTotalReady(false), flowDir(none)
{}

Cell::Cell(float elevation, int yIn, int xIn, direction dir)
	: height(elevation), y(yIn), x(xIn), flowTotal(0), flowTotalReady(false), flowDir(dir)
{}

Cell::Cell()
	: height(-10000), y(-1), x(-1), flowTotal(0), flowTotalReady(false), flowDir(none)
{}

Cell::~Cell()
{}

void Cell::fill(float elevation, int y, int x, direction dir)
{
	height = elevation;
	this->y = y;
	this->x = x;
	flowDir = dir;
	//if a direction is specified, prevent it from being recalculated later
	if(dir != none) flowDirSet.insert(dir);
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

void Cell::setFlowDir(direction dir)
{
	flowDir = dir;
	flowDirSet.clear();
	flowDirSet.insert(dir);
}

direction Cell::getFlowDir() {return flowDir;}

void Cell::accumulate()
{
	ostringstream oss;
	oss << "Calling accumulate on " << y << ',' << x << '\n';  //debug
	lg.write(debug, oss.str());
	
	Cell *adj = NULL;							//an adjacent cell
	if(y > 0)
	{
		adj = &linear(dem,y-1,x);	//north
		if(adj->flowDirs().find(south) != adj->flowDirs().end())
		{
			adj->setFlowDir(south);
			oss.str("");
			oss << "Going north to " << adj->y << ',' << adj->x << '\n';
			lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}
	}
	if(y > 0 && x < (cellsX-1))
	{
		adj = &linear(dem,y-1,x+1);	//northeast
		if(adj->flowDirs().find(southwest) != adj->flowDirs().end())
		{
			adj->setFlowDir(southwest);
			oss.str("");
			oss << "Going northeast to " << adj->y << ',' << adj->x << '\n';
			lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}
	}
	if(x < (cellsX-1))
	{
		adj = &linear(dem,y,x+1);	//east
		if(adj->flowDirs().find(west) != adj->flowDirs().end())
		{
			adj->setFlowDir(west);
			oss.str("");
			oss << "Going east to " << adj->y << ',' << adj->x << '\n';
			lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}
	}
	if(x < (cellsX-1) && y < (cellsY-1))
	{
		adj = &linear(dem,y+1,x+1);	//southeast
		if(adj->flowDirs().find(northwest) != adj->flowDirs().end())
		{
			adj->setFlowDir(northwest);
			oss.str("");
			oss << "Going southeast to " << adj->y << ',' << adj->x << '\n';
			lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}
	}
	if(y < (cellsY-1))
	{
		adj = &linear(dem,y+1,x);	//south
		if(adj->flowDirs().find(north) != adj->flowDirs().end())
		{
			adj->setFlowDir(north);
			oss.str("");
			oss << "Going south to " << adj->y << ',' << adj->x << '\n';
			lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}	
	}
	if(x>0 && y < (cellsY-1))
	{
		adj = &linear(dem,y+1,x-1);	//southwest
		if(adj->flowDirs().find(northeast) != adj->flowDirs().end())
		{
			adj->setFlowDir(northeast);
			oss.str("");
			oss << "Going southwest to " << adj->y << ',' << adj->x << '\n';
			lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}
	}
	if(x>0)
	{
		adj = &linear(dem,y,x-1);	//west
		if(adj->flowDirs().find(east) != adj->flowDirs().end())
		{
			adj->setFlowDir(east);
			oss.str("");
			oss << "Going west to " << adj->y << ',' << adj->x << '\n';
			lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}
	}
	if(y>0 && x>0)
	{
		adj = &linear(dem,y-1,x-1);	//northwest
		if(adj->flowDirs().find(southeast) != adj->flowDirs().end())
		{
			adj->setFlowDir(southeast);
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

const set<direction>& Cell::flowDirs(FlowMethod method)
{
	//if this was already calculated, don't do it again
	if(flowDirSet.size()) return flowDirSet;
	
	vector<float> slopes(8,0);
	switch(method)
	{
		//method 1 (rudiger: compare cross slopes)
		case cross:
		slopes[north]		= linear(dem,y+1,x).height	- linear(dem,y-1,x).height;
		slopes[northeast]	= linear(dem,y+1,x-1).height- linear(dem,y-1,x+1).height;
		slopes[east]		= linear(dem,y,x-1).height	- linear(dem,y,x+1).height;
		slopes[southeast]	= linear(dem,y-1,x-1).height- linear(dem,y+1,x+1).height;
		slopes[south]		= -slopes[north];
		slopes[southwest]	= -slopes[northeast];
		slopes[west]		= -slopes[east];
		slopes[northwest]	= -slopes[southeast];
		break;
	
		//method 2 (compare radial slopes)
		case direct:
		slopes[north]		= linear(dem,y,x).height - linear(dem,y-1,x).height;
		slopes[northeast]	= linear(dem,y,x).height - linear(dem,y-1,x+1).height;
		slopes[east]		= linear(dem,y,x).height - linear(dem,y,x+1).height;
		slopes[southeast]	= linear(dem,y,x).height - linear(dem,y+1,x+1).height;
		slopes[south]		= linear(dem,y,x).height - linear(dem,y+1,x).height;
		slopes[southwest]	= linear(dem,y,x).height - linear(dem,y+1,x-1).height;
		slopes[west]		= linear(dem,y,x).height - linear(dem,y,x-1).height;
		slopes[northwest]	= linear(dem,y,x).height - linear(dem,y-1,x-1).height;
		break;
	
		//method 3 (null)
		case dummy:
		slopes[north]		= 0;
		slopes[northeast]	= 0;
		slopes[east]		= 0;
		slopes[southeast]	= 0;
		slopes[south]		= 1;
		slopes[southwest]	= 0;
		slopes[west]		= 0;
		slopes[northwest]	= 0;
		break;
		
		//method 4 (use lowest elevation)
		case lowest:
		slopes[north]		= -linear(dem,y-1,x	 ).height;
		slopes[northeast]	= -linear(dem,y-1,x+1).height;
		slopes[east]		= -linear(dem,y  ,x+1).height;
		slopes[southeast]	= -linear(dem,y+1,x+1).height;
		slopes[south]		= -linear(dem,y+1,x	 ).height;
		slopes[southwest]	= -linear(dem,y+1,x-1).height;
		slopes[west]		= -linear(dem,y  ,x-1).height;
		slopes[northwest]	= -linear(dem,y-1,x-1).height;	
		break;
	}
	
	float max = *max_element(slopes.begin(),slopes.end());
	for(int dir=north; dir<none; dir++)
	{
		if(slopes[dir] == max)
			flowDirSet.insert(intDirection(dir));
	}
	if(!flowDirSet.size()) lg.write(normal, "flowDirs tried to return an empty set, oops\n");
	return flowDirSet;
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

