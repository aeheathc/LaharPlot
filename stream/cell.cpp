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
{
	flowDirSet = new set<direction>();
	flowTotalReady = false;
	accumulated = false;
}

Cell::Cell(float elevation, int yIn, int xIn, direction dir)
	: height(elevation), y(yIn), x(xIn), flowTotal(0), flowTotalReady(false), flowDir(dir)
{
	flowDirSet = new set<direction>();
	flowDirSet->insert(dir);
	flowTotalReady = false;
	accumulated = false;
}

Cell::Cell()
	: height(-10000), y(-1), x(-1), flowTotal(0), flowTotalReady(false), flowDir(none)
{
	flowDirSet = new set<direction>();
	flowTotalReady = false;
	accumulated = false;
}

Cell::~Cell()
{
	delete flowDirSet;
}

void Cell::fill(float elevation, int y, int x, direction dir)
{
	height = elevation;
	this->y = y;
	this->x = x;
	flowDir = dir;
	//if a direction is specified, prevent it from being recalculated later
	if(dir != none) flowDirSet->insert(dir);
}

bool Cell::getFlowTotalReady() const {return flowTotalReady;}

unsigned long long Cell::getFlowTotal()
{
	if(!flowTotalReady)
	{
		boost::mutex::scoped_lock lock(getFlowTotal_mutex);	
		if(flowTotalReady) return flowTotal;
		accumulate();
		flowTotalReady = true;
	}
	return flowTotal;
}

void Cell::setFlowDir(direction dir)
{
	flowDir = dir;
	flowDirSet->clear();
	flowDirSet->insert(dir);
}

direction Cell::getFlowDir() {return flowDir;}

void Cell::accumulate()
{
	ostringstream oss;
	oss << "Calling   accumulate on " << y << ',' << x << ':' << height <<
		" where flowTotalReady=" << (flowTotalReady?"true":"false")	<< '\n';
	//lg.write(debug, oss.str());
	
	if(flowTotalReady) return;
	
	{
		boost::mutex::scoped_lock lock(accumulate_mutex);	
		if(accumulated) return;
		accumulated = true;
	}
	
	oss.str("");
	oss << "Doing full   accumulate " << y << ',' << x << ':' << height << '\n';
	//lg.write(debug, oss.str());

	Cell *adj = NULL;				//an adjacent cell
	if(y > 0)
	{
		adj = &linear(dem,y-1,x);	//north
		if(adj->flowDirs().find(south) != adj->flowDirs().end())
		{
			adj->setFlowDir(south);
			oss.str("");
			oss << "Going north to " << adj->y << ',' << adj->x << '\n';
			//lg.write(debug, oss.str());
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
			//lg.write(debug, oss.str());
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
			//lg.write(debug, oss.str());
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
			//lg.write(debug, oss.str());
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
			//lg.write(debug, oss.str());
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
			//lg.write(debug, oss.str());
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
			//lg.write(debug, oss.str());
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
			//lg.write(debug, oss.str());
			flowTotal += adj->getFlowTotal() + 1;
		}
	}
	oss.str("");
	oss << "Done with accumulate on " << y << ',' << x << ':' << height << '\n';
	//lg.write(debug, oss.str());
	flowTotalReady = true;
}

const set<direction>& Cell::flowDirs(FlowMethod method)
{
	ostringstream oss;
	oss << "Called flowDirs on " << y << ',' << x << '\n';
	//lg.write(debug, oss.str());
	if(flowDirSet->size()) return *flowDirSet;
	boost::mutex::scoped_lock lock(flowDirs_mutex);
	//if this was already calculated, don't do it again
	if(flowDirSet->size()) return *flowDirSet;

	oss.str("");
	oss << "passed initial check on flowDirs on " << y << ',' << x << '\n';
	//lg.write(debug, oss.str());
	
	//if we're in a no-data zone, flow off the edge of the DEM
	oss.str("");
	if(height < -500)
	{
		int to[8];		//the distance from the current cell TO each border
		to[north] = y;
		to[east]  = cellsX-x-1;
		to[south] = cellsY-y-1;
		to[west]  = x;

		if(to[north] < to[west] && to[north] < to[east])
		{
			flowDirSet->insert(north);
			oss << "cell " << y << ',' << x << " with no data goes north\n";
		}
		if(to[south] < to[west] && to[south] < to[east])
		{
			flowDirSet->insert(south);
			oss << "cell " << y << ',' << x << " with no data goes south\n";
		}
		if(to[west] < to[north] && to[west] < to[south])
		{
			flowDirSet->insert(west);
			oss << "cell " << y << ',' << x << " with no data goes west\n";
		}
		if(to[east] < to[north] && to[east] < to[south])
		{
			flowDirSet->insert(east);
			oss << "cell " << y << ',' << x << " with no data goes east\n";
		}
		if(to[north] == to[west] && x<(cellsX/2))
		{
			flowDirSet->insert(northwest);
			oss << "cell " << y << ',' << x << " with no data goes northwest\n";
		}
		if(to[south] == to[east] && x>=(cellsX/2))
		{
			flowDirSet->insert(southeast);
			oss << "cell " << y << ',' << x << " with no data goes southeast\n";
		}
		if(to[south] == to[west] && y>=(cellsY/2))
		{
			flowDirSet->insert(southwest);
			oss << "cell " << y << ',' << x << " with no data goes southwest\n";
		}
		if(to[north] == to[east] && y<(cellsY/2))
		{
			flowDirSet->insert(northeast);
			oss << "cell " << y << ',' << x << " with no data goes northeast\n";
		}
	
		if(flowDirSet->size())
		{
			if(flowDirSet->size()>1) oss << "no data, but multiple directions!\n";
			//lg.write(debug,oss.str());
			return *flowDirSet;
		}
	}
	
	vector<float> slopes(8,0);
	switch(method)
	{
		//method 2 (compare radial slopes)
		case direct:
		slopes[north]		= linear(dem,y,x).height - linear(dem,y-1,x  ).height;
		slopes[northeast]	= linear(dem,y,x).height - linear(dem,y-1,x+1).height;
		slopes[east]		= linear(dem,y,x).height - linear(dem,y,  x+1).height;
		slopes[southeast]	= linear(dem,y,x).height - linear(dem,y+1,x+1).height;
		slopes[south]		= linear(dem,y,x).height - linear(dem,y+1,x  ).height;
		slopes[southwest]	= linear(dem,y,x).height - linear(dem,y+1,x-1).height;
		slopes[west]		= linear(dem,y,x).height - linear(dem,y,  x-1).height;
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
		slopes[north]		= -linear(dem,y-1,x  ).height;
		slopes[northeast]	= -linear(dem,y-1,x+1).height;
		slopes[east]		= -linear(dem,y,  x+1).height;
		slopes[southeast]	= -linear(dem,y+1,x+1).height;
		slopes[south]		= -linear(dem,y+1,x  ).height;
		slopes[southwest]	= -linear(dem,y+1,x-1).height;
		slopes[west]		= -linear(dem,y,  x-1).height;
		slopes[northwest]	= -linear(dem,y-1,x-1).height;	
		break;
		
		//method 1 (rudiger: compare cross slopes)
		case cross:
		default:
		slopes[north]		= linear(dem,y+1,x	).height - linear(dem,y-1,x  ).height;
		slopes[northeast]	= linear(dem,y+1,x-1).height - linear(dem,y-1,x+1).height;
		slopes[east]		= linear(dem,y,	 x-1).height - linear(dem,y,  x+1).height;
		slopes[southeast]	= linear(dem,y-1,x-1).height - linear(dem,y+1,x+1).height;
		slopes[south]		= -slopes[north];
		slopes[southwest]	= -slopes[northeast];
		slopes[west]		= -slopes[east];
		slopes[northwest]	= -slopes[southeast];
		break;
	}
	
	float max = *max_element(slopes.begin(),slopes.end());
	oss.str("");
	oss << "flowDirs:" << y << ',' << x << " has max slope " << max << '\n';
	//lg.write(debug, oss.str());
	for(int dir=north; dir<none; dir++)
	{
		if(slopes[dir] == -0) slopes[dir]=0;
		if(slopes[dir] >= max)
		{
			flowDirSet->insert(intDirection(dir));
			oss.str("");
			oss << "flowDirs:" << y << ',' << x << " can flow " << intDirection(dir) << '\n';
			lg.write(debug, oss.str());
		}
	}
	
	if(!flowDirSet->size())
	{
		oss.str("");
		oss << "flowDirs tried to return an empty set for " << y << ',' << x << '\n';
		lg.write(normal, oss.str());
	}
	return *flowDirSet;
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

