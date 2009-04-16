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

#ifndef FILLSINKS_H
#define FILLSINKS_H

#include "util.h"

using namespace std;

extern Logger lg;

/*	This algorithm fo fill sinkholes is an implementation of:
	Planchon, O. & F. Darboux (2001):
	"A fast, simple and versatile algorithm to fill the depressions of digital
	elevation models."
	Catena 46: 159-176
*/
class FillSinks
{
	public:
	FillSinks(float* linearDem, int nYSize, int nXSize, double minimumSlope = 0);
	~FillSinks();
	void fill();
	static int neighborX(int direction, int column);
	static int neighborY(int direction, int row);
	
	private:
	double minslope;
	int cellsY, cellsX;
	
	int			R, C, R0[8], C0[8], dR[8], dC[8], fR[8], fC[8];
	double		epsilon[8];
	float		*pW, *pDEM;
	int 		*pBorder;
	bool		nextCell(int i);
	void		initAltitude();
	void		dryUpwardCell(int x, int y);
};

#endif
