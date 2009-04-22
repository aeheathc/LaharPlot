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

#include "fill.h"

FillSinks::FillSinks(float* linearDem, int nYSize, int nXSize, double minimumSlope)
	: minslope(minimumSlope), cellsY(nYSize), cellsX(nXSize), pDEM(linearDem)
{}

FillSinks::~FillSinks() {}

void FillSinks::fill()
{
	static bool runBefore = false;
	if(runBefore) return;
	runBefore = true;
	
	bool	something_done;
	int		x, y, scan, ix, iy, i, it;
	double	z, wz, wzn;
	pW		= new float[cellsX*cellsY];
	pBorder = new int[cellsX*cellsY];
	
	//initialize static variable inside linear()
	linear(pBorder,0,0,cellsX);
	linear(pW,0,0,cellsX);

	for(i=0; i<8; i++)
	{
		//diagonal cells are slightly more distant than N,S,E,W neighbors, so
		//an equal slope gives a greater elevation difference.
		if(i%2)
			epsilon[i] = minslope * 1.41421;
		else
			epsilon[i] = minslope;
	}

	R0[0] = 0; R0[1] = cellsY-1;	R0[2] = 0;			R0[3] = cellsY-1;	R0[4] = 0;			R0[5] = cellsY-1;	R0[6] = 0; R0[7] = cellsY-1;
	C0[0] = 0; C0[1] = cellsX-1;	C0[2] = cellsX-1;	C0[3] = 0;			C0[4] = cellsX-1;	C0[5] = 0;			C0[6] = 0; C0[7] = cellsX-1;
	dR[0] = 0; dR[1] = 0;			dR[2] = 1;			dR[3] = -1;			dR[4] = 0;			dR[5] = 0;			dR[6] = 1; dR[7] = -1;
	dC[0] = 1; dC[1] = -1;			dC[2] = 0;			dC[3] = 0;			dC[4] = -1;			dC[5] = 1;			dC[6] = 0; dC[7] = 0;
	fR[0] = 1; fR[1] = -1;			fR[2] = -cellsY+1;	fR[3] = cellsY-1;	fR[4] = 1;			fR[5] = -1;			fR[6] = -cellsY+1; fR[7] = cellsY-1;
	fC[0] = -cellsX+1, fC[1] = cellsX-1; fC[2] = -1;	fC[3] = 1;			fC[4] = cellsX-1;	fC[5] = -cellsX+1;	fC[6] = 1; fC[7] = -1;

	initAltitude();															// Stage 1

	for(x=0; x<cellsX; x++)													// Stage 2, Section 1
	{
		for(y=0; y<cellsY; y++)
		{
			if(linear(pBorder, y, x) == 1 )
				dryUpwardCell(x, y);
		}
	}
	for(it=0; it<1000; it++)
	{
		for(scan=0; scan<8; scan++)					// Stage 2, Section 2
		{
			R = R0[scan];
			C = C0[scan];
			something_done = false;

			do 
			{
				if((wz = linear(pW, R, C)) > (z = linear(pDEM, R, C)))
				{
					for(i=0; i<8; i++)
					{
						ix	= neighborX(i, C);
						iy	= neighborY(i, R);	

						if(ix>=0 && iy>=0 && ix<cellsX && iy<cellsY)
						{
							if( z >= (wzn = (linear(pW, iy, ix) + epsilon[i])) )	// operation 1
							{
								linear(pW, R, C) = z;
								something_done = true;
								dryUpwardCell(C, R);
								break;
							}
							if( wz > wzn )											// operation 2
							{
								linear(pW, R, C) = wzn;
								something_done = true;
							}
						}
					}
				}
			}while(nextCell(scan));
			if(something_done == false) break;
		}
		if(something_done == false) break;
	}

	for(long long count = 0; count < cellsX*cellsY; count++) pDEM[count] = pW[count];

	delete pW;
	delete pBorder;

	return;
}

void FillSinks::dryUpwardCell(int x, int y)
{
	int const	MAX_DEPTH = 32000;	//arbitrary limit to prevent runaway recursion
	int			depth = 0;
	int			ix, iy, i;
	double		zn;

	depth += 1;
	
	if( depth <= MAX_DEPTH )
	{
		for(i=0; i<8; i++)
		{
			ix	= neighborX(i, x);		
			iy	= neighborY(i, y);	
			
			if(ix>=0 && iy>=0 && ix<cellsX && iy<cellsY && linear(pW, iy, ix) == 50000 )
			{
				if( (zn = linear(pDEM, iy, ix)) >= (linear(pW, y, x) + epsilon[i]) )
				{
					linear(pW, iy, ix) = zn;
					dryUpwardCell(ix, iy);
				}
			}
		}
	}
	depth -= 1;
}

void FillSinks::initAltitude()
{
	bool	border;
	int		x, y, i, ix, iy;
	for(x=0; x<cellsX; x++)
	{
		for(y=0; y<cellsY; y++)
		{
			border = false;
			for(i=0; i<8; i++)
			{
				ix	= neighborX(i, x);
				iy	= neighborY(i, y);
				if(ix<0 || iy<0 || ix>=cellsX || iy>=cellsY)
				{
					border = true;
					break;
				}
			}

			if( border == true )
			{
				linear(pBorder, y, x) = 1;
				linear(pW, y, x) = linear(pDEM, y, x);
			}
			else
			{
				linear(pW, y, x) = 50000.0;
			}
		}
	}
}	

bool FillSinks::nextCell(int i)
{
	R = R + dR[i];
	C = C + dC[i];

	if( R < 0 || C < 0 || R >= cellsY || C >= cellsX )
	{
		R = R + fR[i];
		C = C + fC[i];
		if( R < 0 || C < 0 || R >= cellsY || C >= cellsX )
			return false;
	}
	return true;
}

int FillSinks::neighborX(int direction, int column)
{
	switch(direction)
	{
		case 0:
		case 4:
		return column;
		
		case 1:
		case 2:
		case 3:
		return column+1;
			
		case 5:
		case 6:
		case 7:
		return column-1;
		
		default:
		return -1;
	}
}

int FillSinks::neighborY(int direction, int row)
{
	switch(direction)
	{
		case 2:
		case 6:
		return row;
		
		case 3:
		case 4:
		case 5:
		return row+1;
			
		case 0:
		case 1:
		case 7:
		return row-1;
		
		default:
		return -1;
	}
}
