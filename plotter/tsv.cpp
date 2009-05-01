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

/***************************************************************
 * Name:      tsv.cpp
 * Purpose:   Handles tsv files, and functions
 **************************************************************/

#include "tsv.h"

#include <wx/msgdlg.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

string tsvFileName;
float tsvMax = 0, tsvMin = 0;
int tsvLen = 0;

tsv::tsv(wxString filename)
{
    tsvFileName = filename.mb_str(wxConvUTF8);
}

tsv::~tsv()
{
}

float tsv::getMin()
{
	return tsvMin;
}

float tsv::getMax()
{
	return tsvMax;
}

void tsv::setFileName(wxString filename, wxProgressDialog* progDlg, float start, float end)
{
    tsvFileName = filename.mb_str(wxConvUTF8);
    progDlg->Update(start,_("Finding Length"));
    findLength();
    progDlg->Update(start,_("Parsing ") + filename);
    loadtsv(progDlg, start, end);
}

const char* tsv::getFileName()
{
	return tsvFileName.c_str();
}

void tsv::findLength()
{
	int i = 0;
	string line;
	ifstream file (tsvFileName.c_str());
	while (!file.eof())
	{
		getline (file,line);
		i++;
	}
	tsvLen = i;
	file.close();
}

void tsv::loadtsv(wxProgressDialog* progDlg, float start, float end)
{
    // Initialize variables
    int k = 0;
	float max = 0, min = 0;
	bool haveMin = false;
	string line;
	clear();

	// Input filestream
	ifstream file (tsvFileName.c_str());

	// Parse SDEM
	while (!file.eof())
	{
		// Get line of file.
		getline (file,line);

		// temp vector to convert from tsv
		list<float> tsvLine;

		// counters
		unsigned int i = 0, j = 0;

		// seperate loops for initial char, and final
		for (; i < line.length(); i = j)
		{
			for (; j < line.length() && line.at(j) != '\t'; j++){}

			// take string from i to j, and conversound volume doesnt savet it to a float
			string num = line.substr(i, j - i);
			istringstream buffer(num);
			float temp;
			buffer >> temp;

			// keep track of max and min
			if (temp > max) { max = temp; }
			if (!haveMin || temp < min)
			{
				// non-existent point correction
				if (temp > -10000)
				{
					min = temp;
					haveMin = true;
				}
			}

			// store and incriment
			tsvLine.push_back(temp);
			j++;
		}

		k++;
		if (k % 50 == 1) progDlg->Update(start + (k * ((end - start) / tsvLen)));

		// insert line to sdem
		if (tsvLine.size() > 0)
			push_back(tsvLine);
	}

	// close the sdem
	file.close();

	// store max/min
	tsvMax = max;
	tsvMin = min;
}
