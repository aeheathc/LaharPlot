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

#ifndef UTIL_H
#define UTIL_H

#include <boost/thread.hpp>

//Use a 2D index over a 1D array.
template<typename T>
T& linear(T *array, int y, int x, int width = -1)
{
	static int cellsX = -1;
	if(width != -1) cellsX = width;
	return array[y*cellsX+x];
}

//Index into a linear collection of elements on the OUTER EDGE of a matrix
//stored in a 1D array
template<typename T>
T& edge(T *array, int x, int width = -1, int height = -1)
{
	static int cellsX = -1, cellsY = -1;
	if(width != -1) cellsX = width;
	if(height != -1) cellsY = height;
	if(x<cellsY) return array[x];
	if(x > cellsX+cellsY*2-5) return array[x + (cellsX-2)*(cellsY-2)];
	x -= width;
	if(x%2 == 0)
	{
		return array[cellsX*(x/2+1)];
	}
	return array[cellsX*((x-1)/2+1) + (cellsX-1)];
}
//Trivial predicate that just uses the < operator of the subject
struct Pred
{
	template<typename P>
	bool operator()(const P& l, const P& r) const {return l<r;}
};

void sleep(int sec);

#endif
