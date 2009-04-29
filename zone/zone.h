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

//=============================================================================
//
// zone.cpp
//
// Function that creates a map containing the inundated zone created by a lahar
//
// February 9, 2009
// Jason Anderson
//
//=============================================================================

#ifndef ZONE_H
#define ZONE_H

#include <boost/config.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <exception>
#include <stdlib.h>
#include <math.h>
#include <time.h>

using namespace std;
namespace po = boost::program_options;

// INI parsed parameters
double cellWidth;
int xCells;
int yCells;

// Enumerator for cross section calculation
enum CrossDir { N_S, NE_SW, E_W, SE_NW };

// Struct containing the data to be passed to create IZM
// Makes function call more readable, and allows boost's bind function to work (max args were 10)
struct IZMData {
	double ** flowDirGrid;
	double ** elevGrid;
	int startX;
	int startY;
	int endX;
	int endY;
	double coeffA;
	double coeffB;
	string outName;
	bool v;
};

/**
 * This class defines functions for a mapper status object that can store information
 * about simultaneously running threads that are doing inundation zone calculations.
 */
class MapperStatus
{
public:

	/**
	 * Mapper Status constructor that initializes status reporter and information containers.
	 *
	 * Parameters:
	 * 		size - The number of threads running inundation calculations.
	 * 		vols - The different volumes to run calculations with.
	 */
    MapperStatus( int size, double * vols ) {
    	volumes       = new double[size];
    	statuses      = new int[size];
    	printing      = new bool[size];
    	done          = new bool[size];
    	endConditions = new string[size];

    	numVolumes = size;

    	for(int i = 0; i < numVolumes; i++)
    		volumes[i] = vols[i];
    	for(int i = 0; i < numVolumes; i++)
    		statuses[i] = 0;
    	for(int i = 0; i < numVolumes; i++)
    		printing[i] = false;
    	for(int i = 0; i < numVolumes; i++)
    		done[i] = false;
    	for(int i = 0; i < numVolumes; i++)
    		endConditions[i] = "";
    }

	/**
	 * Function that sets the current status of a thread based ont he given parameters.
	 *
	 * Parameters:
	 * 		threadID - The ID of the thread that is changing.
	 * 		status - The percentage of completion for the given thread.
	 * 		print - Boolean for whether or not the thread is currently printing out to a file.
	 * 		finish - Boolean for whether or not the thread is finished.
	 */
    void setStatus( int threadID, double status, bool print, bool finish ) {
        boost::mutex::scoped_lock scoped_lock(mutex);
        if (!finish && !print )
        	statuses[threadID] = (int) status;
        else {
        	printing[threadID] = print;
        	done[threadID] = finish;
        }
    }

    /**
     * Prints a status report containing information about each thread.
     */
    void printStatus() {
        boost::mutex::scoped_lock scoped_lock(mutex);
    	cout << "~~~~~Status Report~~~~~" << endl;
    	for(int i = 0; i < numVolumes; i++) {
    		if (printing[i])
    			cout << "Volume " << volumes[i] << " is currently printing to output file." << endl;
    		else if (done[i])
    			cout << "Volume " << volumes[i] << " has finished and consumed " << statuses[i] << "% of the planimetric area." << endl;
    		else
    			cout << "Volume " << volumes[i] << " is running and has consumed " << statuses[i] << "% of the planimetric area." << endl;
    	}
    	cout << endl;
    }

    /**
     * Stores the given ending condition for future reference.
     *
     * Parameters:
	 * 		threadID - The ID of the thread that is changing.
	 * 		condition - A string containing why the thread has ended
     */
    void setEndCondion( int threadID, string condition ) {
    	endConditions[threadID] = condition;
    }


    /**
     * Prints an ending report containing information about each thread.
     */
    void printEndConditions() {
    	cout << endl << "~~~~~Final Report~~~~~" << endl;
    	for(int i = 0; i < numVolumes; i++) {
    		cout << "For volume " << volumes[i] << " " << endConditions[i] << "  " << statuses[i] << "% of the volume was consumed before completion." << endl;
    	}
    	cout << endl;
    }


    /**
     * Returns whether the thread is finished or not.
     *
     * Return:
     * 		Returns true if the thread has finished execution, false otherwise.
     */
    bool finished() {

        boost::mutex::scoped_lock scoped_lock(mutex);
    	bool finished = true;
    	for(int i = 0; i < numVolumes; i++)
    		if (!done[i])
    			finished = false;

    	return finished;

    }

private:


    boost::mutex mutex;
    int numVolumes;

    // storage containers for information about threads
    double * volumes;
    int * statuses;
    bool * printing;
    bool * done;
    string * endConditions;

};

/**
 * 	  main
 *
 * This function handles all program options, parsing of files, and
 * function calls to create the inundation zone map.
 *
 * Parameters:
 * 		ac - The number of command line arguments
 * 		av - Array containing command line arguments
 *
 * Return:
 * 		Returns 1 if successful, 0 otherwise.
 */
int main(int ac, char *av[]);

/**
 *    parseINI
 *
 * This function takes the name of an INI file and parses it for
 * the number of x and y cells, and the width of each cell.
 *
 * Parameters:
 * 		name - The name of an INI file.
 *
 * Return:
 * 		Returns 1 if successful, 0 otherwise.
 */
int parseINI(string name);

/**
 *    parseTSV
 *
 * This function takes the name of an TSV file, reads in the data,
 * and then stores it in the given grid.
 *
 * Parameters:
 * 		name - The name of an INI file.
 *
 * Return:
 * 		Returns 1 if successful, 0 otherwise.
 */
int parseTSV(string name, double ** grid);

/**
 *    createIZM
 *
 * This function creates an inundation grid with the given parameters.
 *
 * Parameters:
 * 	 In IZMData Struct:
 * 		flowDirGrid - A grid containing the flow directions of an SDEM
 * 		elevGrid - An SDEM containing elevations of map
 * 		startX - The x position to start the lahar from
 * 		startY - The y position to start the lahar from
 * 		endX - The x position to end a lahar at
 * 		endY - The y position to end a lahar at
 * 		coefA - The coefficient to determine the maximum cross sectional area
 * 		coefB - The coefficient to determine the maximum planimetric area
 * 		outName - The name of the output file to write the inundation grid to
 * 		v - Verbose boolean
 *   Outside IZMData Struct:
 *      volume - The volume of the expected lahar
 *      ID - The thread ID calling this function
 *      status - The status object tracking all threads
 *
 * Return:
 * 		Returns 1 if successful, 0 otherwise.
 */
//int createIZM(double ** flowDirGrid, double ** elevGrid, int startX, int startY, int endX, int endY,
//		      double coefA, double coefB, double volume, string outName, bool v);
int createIZM(IZMData data, double volume, int ID, MapperStatus * status);

/**
 *    getVolEqResult
 *
 * This function returns the result of the equation C * V ^ (2/3).
 *
 * Parameters:
 * 		coeff (C) - The coefficient used for the equation
 * 		volume (V) - The volume used for the equation
 *
 * Return:
 * 		Returns the result of the equation.
 */
double getVolEqResult(double coeff, double volume);

/**
 *    calcCrossSection
 *
 * This function calculates a cross sectional area from a given position
 * and each cell it extends to will be marked in an inundation grid.
 *
 * Parameters:
 * 		inunGrid - A grid to store cells affected by a lahar
 * 		elevGrid - An SDEM containing elevations of map
 * 		rX - The x position of the stream cell
 * 		rY - The y position of the stream cell
 * 		maxCrossArea - The maximum cross sectional area to calculate up to
 * 		cD - The direction to calculate the cross section in
 * 		gapCS - Used to determine if this is a stream cell
 * 		v - Verbose mode
 *
 * Return:
 * 		Returns 1 if successful, 0 otherwise.
 */
int calcCrossSection(double ** inunGrid, double ** elevGrid, int rX, int rY, double maxCrossArea, CrossDir cD, bool gapCS, bool v);

/**
 *    outputInunGrid
 *
 * This function prints an inundation grid to a file.
 *
 * Parameters:
 * 		inunGrid - A grid storing the cells affected by a lahar
 * 		volume - The volume the lahar was calculated for
 * 		outName - The name of the output file to write the inundation grid to
 * 		v - Verbose boolean
 */
void outputInunGrid(double ** inunGrid, double volume, string outName, bool v);

/**
 *    monitorSttaus
 *
 * This function prints out a status message of all threads and sleeps
 * for a given interval.
 *
 * Parameters:
 * 		status - The mapper status object linking all the threads together.
 *      statTime - The interval between status prints to the console.
 */
void monitorStatus(MapperStatus * status, int statTime);

#endif
