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

#include <set>

using namespace std;

enum direction{north, northeast, east, southeast, south, southwest, west, northwest, none};

direction intDirection(int dirIn);

struct Point
{
	int x,y;
};

class Cell
{
	public:
	float height;
	direction flowDir;
	set<Point> flowTotal;
	Cell(float elevation);
	Cell(float elevation, direction dir);
	Cell();
	~Cell();
};

#endif
