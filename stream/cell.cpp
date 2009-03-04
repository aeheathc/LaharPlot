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

/*
	In these constructors, we have to create a seperate shared memory space to
	store the Set of points "flowTotal" (see definition comments), because the
	shared memory space that holds the Cells will be full and flowTotal needs
	to have variable size. The shared memory space is named based on the
	x,y location of this Cell.
*/
Cell::Cell(float elevation, int x, int y)
	: height(elevation), flowDir(none), location(Point(x,y))
{
	ostringstream shmemNameBuild;
	shmemNameBuild << "Cell." << x << ',' << y;
	shmemName = shmemNameBuild.str();
	ip::managed_shared_memory segment(	ip::open_or_create,shmemName.c_str(),
										sizeof(ip::set<Point,PointShmemAllocator>)+sizeof(Point)*20);
	const PointShmemAllocator alloc_inst(segment.get_segment_manager());
	segment.construct<ip::set<Point,PointShmemAllocator> >("points")(alloc_inst);
}

// TODO: use chained constructors when that functionality comes in C++09
Cell::Cell(float elevation, direction dir, int x, int y)
	: height(elevation), flowDir(dir), location(Point(x,y))
{
	ostringstream shmemNameBuild;
	shmemNameBuild << "Cell." << x << ',' << y;
	shmemName = shmemNameBuild.str();
	ip::managed_shared_memory segment(	ip::open_or_create,shmemName.c_str(),
										sizeof(ip::set<Point,PointShmemAllocator>)+sizeof(Point)*20);
	const PointShmemAllocator alloc_inst(segment.get_segment_manager());
	segment.construct<ip::set<Point,PointShmemAllocator> >("points")(alloc_inst);
}

Cell::Cell()
	: height(0), flowDir(none), location(Point(-1,-1))
{}

Cell::~Cell()
{
	/*
		if this is a dummy object don't bother trying to delete a 
		shared memory space that isn't going to exist
	*/
	if(!shmemName.length()) return;
	
	/*
		create inner scope so the segment handle is destroyed before the actual
		shared memory space is destroyed
	*/
	{
		ip::managed_shared_memory segment(ip::open_only,shmemName.c_str());
		segment.destroy<ip::set<Point,PointShmemAllocator> >("points");
	}
	ip::shared_memory_object::remove(shmemName.c_str());
}

/*
	We must regulate access to the set so that we can expand the size of the
	shared memory as items are added. We can't keep a pointer around because
	resizing the shared memory requires that nobody has their hands in it.
*/
ip::set<Point,PointShmemAllocator>& Cell::flowTotal()
{
	return flowTotal(0);
}

ip::set<Point,PointShmemAllocator>& Cell::flowTotal(int reserveSpace)
{
	/*
		segment is created on the stack so we can delete it right away in case
		we need to resize the shared memory space
	*/
	ip::managed_shared_memory* segment =
		new ip::managed_shared_memory(ip::open_only,shmemName.c_str());
	ip::set<Point,PointShmemAllocator>* flowTot =
		segment->find<ip::set<Point,PointShmemAllocator> >("points").first;
	delete segment;
	if(segment->get_size() < (sizeof(flowTot)+sizeof(Point)*(reserveSpace+5)))
	{
		flowTot = NULL;
		ip::managed_shared_memory::grow(shmemName.c_str(), sizeof(Point)*(reserveSpace+20));
		return flowTotal();
	}
	return *flowTot;
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
