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

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <exception>
#include <stdlib.h>
#include <math.h>

using namespace std;
namespace po = boost::program_options;

// INI parsed parameters
double cellWidth;
int xCells;
int yCells;

// Enumerator for cross section calculation
enum CrossDir { N_S, NE_SW, E_W, SE_NW };

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
 * 		flowDirGrid - A grid containing the flow directions of an SDEM
 * 		elevGrid - An SDEM containing elevations of map
 * 		startX - The x position to start the lahar from
 * 		startY - The y position to start the lahar from
 * 		coefA - The coefficient to determine the maximum cross sectional area
 * 		coefB - The coefficient to determine the maximum planimetric area
 * 		volume - The volume of the expected lahar
 * 		outName - The name of the output file to write the inundation grid to
 * 		v - Verbose boolean
 *
 * Return:
 * 		Returns 1 if successful, 0 otherwise.
 */
int createIZM(double ** flowDirGrid, double ** elevGrid, int startX, int startY,
		      double coefA, double coefB, double volume, string outName, bool v);

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

#endif
