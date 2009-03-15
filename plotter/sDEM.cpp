/***************************************************************
 * Name:      sDEM.cpp
 * Purpose:   Handles SDEM files, and functions
 * Author:     ()
 * Created:   2009-02-19
 * Copyright:  ()
 * License:
 **************************************************************/

#include "sDEM.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

string sDEMFileName;
float demMax = 0, demMin = 0;

sDEM::sDEM(wxString filename)
{
    sDEMFileName = filename.mb_str(wxConvUTF8);
}

sDEM::~sDEM()
{
}

float sDEM::getMax()
{
    return demMax;
}

float sDEM::getMin()
{
    return demMin;
}

void sDEM::setFileName(wxString filename)
{
    sDEMFileName = filename.mb_str(wxConvUTF8);
    loadSDEM();
}

const char* sDEM::getFileName()
{
	return sDEMFileName.c_str();
}

void sDEM::loadSDEM()
{
    // Initialize variables
	float max = 0, min = 0;
	bool haveMin = false;
	string line;

	// Input filestream
	ifstream file (sDEMFileName.c_str());

	// Parse SDEM
	while (!file.eof())
	{
		// Get line of file.
		getline (file,line);

		// temp vector to convert from tsv
		vector<float> demLine;

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
				min = temp;
				haveMin = true;
			}

			// store and incriment
			demLine.push_back(temp);
			j++;
		}

		// insert line to sdem
		 if (demLine.size() > 0)
			push_back(demLine);
	}

	// close the sdem
	file.close();

	// store max/min
	demMax = max;
	demMin = min;
}
