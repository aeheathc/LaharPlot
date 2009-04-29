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
// File that creates a map containing the inundated zone created by a lahar
//
// February 9, 2009
// Jason Anderson
//
//=============================================================================

#include "zone.h"

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
int main(int ac, char *av[]) {

	// define option character texts
	char * initText = (char*) "Zone Options\n"
					  "  Notes:\n"
					  "    - '*' denotes a required option\n"
					  "    - All input files must be in the same directory\n"
					  "    - If directory is specified, it must contain no\n"
					  "      spaces\n"
					  "    - The # symbol in the output file name represents\n"
					  "      which given volume the file is associated with\n"
					  "    - Volume(s) must be unique and specified last\n"
					  "    - The ending point must be exactly on a stream cell\n"
					  "      that the lahar path is following\n"
					  "    - The program will terminate when:\n"
					  "         ~ The maximum planimetric area is reached\n"
					  "         ~ The edge of the SDEM is reached \n"
					  "         ~ The optional ending (x, y) coordinate\n"
					  "           specified in the options is reached\n";

	char * helpText = (char*) "Display this help\n";

	char * nameText = (char*) "Use the given simple name to open <arg>.ini, <arg>-sdem.tsv, "
			          "and <arg>-fdir.tsv files.  All files must "
			          "have same common name and be in the same directory.  The "
			          "program will output the Inundated Zone file using the same name.  "
			          "Can't be used with any other *-name options\n";

	char * dirText  = (char*) "Use the given directory <arg> to direct the program to all files.  "
					  "The Inundated Zone file will be output to the same directory.  "
					  "Default directory is the working directory containing the "
					  "executable.\n";

	char * metaText = (char*) "Read information about SDEM data from ini file with given name.  "
					  "Can't be used with simple_name.  (File type = <arg>.ini)\n";

	char * sdemText = (char*) "Read topography from file with given SDEM file name.  "
					  "Can't be used with simple_name.  (File type = <arg>-sdem.tsv)\n";

	char * fdirText = (char*) "Read flow directions for each cell from file with given name.  "
					  "Can't be used with simple_name.  (File type = <arg>-fdir.tsv)\n";

	char * outText  = (char*) "Output Inundated Zone to file using given name.  Can't be "
					  "used with simple_name. (Result file = <arg>-zone#.tsv)\n";

	char * coAText  = (char*) "Desired coefficient for cross sectional area.  A = <arg> * V ^ (2/3) "
					  "The default value of 0.05 is set in accordance to Lahar prediction models.\n";

	char * coBText  = (char*) "Desired coefficient for planimetric area.  A = <arg> * V ^ (2/3) "
					  "The default value of 200 is set in accordance to Lahar prediction models.\n";

	char * stxText  = (char*) "'*' The simulated lahar's starting x value, <arg>.\n";

	char * styText  = (char*) "'*' The simulated lahar's starting y value, <arg>.\n";

	char * endxText = (char*) "The simulated lahar's ending x value, <arg>.  This x "
					  "value *must* be exactly on the stream the lahar is following "
					  "or it will be ignored.  Must be used in conjunction with "
					  "end_y.\n";

	char * endyText = (char*) "The simulated lahar's ending y value, <arg>.  This y "
					  "value *must* be exactly on the stream the lahar is following "
					  "or it will be ignored.  Must be used in conjunction with "
					  "end_x.\n";

	char * volText  = (char*) "'*' The simulated lahar's volumes, <arg>.  <arg> must contain "
					  "at least 1 volume in integer form.  Keep in mind that the more "
					  "volumes supplied, the longer this algorithm will take.  This calculation "
					  "performs independent of the value's units, but keep in mind that SDEMs "
					  "usually display their values in meters.\n";

	char * verbText = (char*) "Display detailed information.  Due to multi-threading, it is highly "
					  "recommended that you only use verbose mode when there is only one volume.\n\n"
					  "Verbose Output Key:\n"
					  " - FL = Current fill level\n"
					  " - RL = Current right fill level height\n"
					  " - LL = Current left fill level height\n"
					  " - CSW = Current cross section width\n"
					  " - CellW = The width of a single cell\n";


	char * statText = (char*) "Display a status output every <arg> seconds while calculating the "
					  "inundation zone.  The default print out time is three seconds.  To "
					  "disable the status, set this option to zero.\n";

	// define variables to store command line information
	string simpleName;
	string directory;
	string metaName;
	string sdemName;
	string fdirName;
	string outName;
	double coeffA = .05;
	double coeffB = 200;
	int statTime = 3;
	int startX;
	int startY;
	int endX;
	int endY;
	double * volumes;
	vector <double> tempVolumes;
	int numVolumes;

	// booleans to determine if information has been set
	bool simpleNameOn = false;
	bool directorySet = false;
	bool metaNameSet = false;
	bool sdemNameSet = false;
	bool fdirNameSet = false;
	bool outNameSet = false;
	bool startXSet = false;
	bool startYSet = false;
	bool endXSet = false;
	bool endYSet = false;
	bool volSet = false;
	bool verboseOn = false;

	// file extensions
	string metaExt = ".ini";
	string sdemExt = "-sdem.tsv";
	string fdirExt = "-fdir.tsv";

	// File parsed grids
	double ** flowDirGrid;
	double ** elevGrid;

	// +-+-+-+-+-+-+-+ Parse Program Options +-+-+-+-+-+-+-+
	try {
		// Declare the supported options.
		po::options_description desc(initText);
		desc.add_options()
			("help", helpText)
			("verbose,e", verbText)
			("status_timer,t", po::value<int>(), statText)
			("simple_name,n", po::value<string>(), nameText)
			("directory,p", po::value<string>(), dirText)
			("meta_data_file_name,m", po::value<string>(), metaText)
			("SDEM_file_name,s", po::value<string>(), sdemText)
			("flow_direction_grid_name,d", po::value<string>(), fdirText)
			("output_file_name,o", po::value<string>(), outText)
			("coefficient_A,a", po::value<double>(), coAText)
			("coefficient_B,b", po::value<double>(), coBText)
			("start_x,x", po::value<int>(), stxText)
			("start_y,y", po::value<int>(), styText)
			("end_x,f", po::value<int>(), endxText)
			("end_y,z", po::value<int>(), endyText)
			("volume,v", po::value< vector<double> >(&tempVolumes)->multitoken(), volText)
		;

		po::variables_map vm;
		po::store(po::parse_command_line(ac, av, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			std::cout << desc << "\n";
			return 1;
		}

		if (vm.count("simple_name")) {
			simpleName = vm["simple_name"].as<string>();
			metaName = vm["simple_name"].as<string>() + metaExt;
			sdemName = vm["simple_name"].as<string>() + sdemExt;
			fdirName = vm["simple_name"].as<string>() + fdirExt;
			outName = vm["simple_name"].as<string>();
			simpleNameOn = true;

			cout << "Simple name set to " << simpleName << ".  File names are as follows:\n"
				 << "  Meta File Name:            " << metaName << "\n"
				 << "  SDEM File Name:            " << sdemName << "\n"
				 << "  Flow Direction File Name:  " << fdirName << "\n"
				 << "  Output file name(s):       " << outName  << "-zone#.tsv" << endl;
		}

		if (vm.count("directory")) {
			directory = vm["directory"].as<string>();
			directorySet = true;

			cout << "Directory set to " << directory << endl;
		}

		if (vm.count("meta_data_file_name")) {

			if (simpleNameOn)
				cout << "Meta data file name not set -> Simple name already designated" << endl;

			else {
				metaName = vm["meta_data_file_name"].as<string>() + metaExt;
				metaNameSet = true;

				cout << "Meta data file name set to: " << metaName << endl;
			}
		}

		if (vm.count("SDEM_file_name")) {

			if (simpleNameOn)
				cout << "SDEM file name not set -> Simple name already designated" << endl;

			else {
				sdemName = vm["SDEM_file_name"].as<string>() + sdemExt;
				sdemNameSet = true;

				cout << "SDEM file name set to: " << sdemName << endl;
			}
		}

		if (vm.count("flow_direction_grid_name")) {

			if (simpleNameOn)
				cout << "Flow direction grid name not set -> Simple name already designated" << endl;

			else {
				fdirName = vm["flow_direction_grid_name"].as<string>() + fdirExt;
				fdirNameSet = true;

				cout << "Flow direction file name set to: " << fdirName << endl;
			}
		}

		if (vm.count("output_file_name")) {

			if (simpleNameOn)
				cout << "Output name not set -> Simple name already designated" << endl;

			else {
				outName = vm["output_file_name"].as<string>();
				outNameSet = true;

				cout << "Output name set to: " << outName << endl;
			}
		}

		if (vm.count("coefficient_A")) {
			coeffA = vm["coefficient_A"].as<double>();

			cout << "Coefficient A set to " << coeffA << ".\n";
		}

		if (vm.count("coefficient_B")) {
			coeffB = vm["coefficient_B"].as<double>();

			cout << "Coefficient B set to " << coeffB << ".\n";
		}

		if (vm.count("start_x")) {
			startX = vm["start_x"].as<int>();
			startXSet = true;

			cout << "Starting x cell set to " << startX << ".\n";
		}

		if (vm.count("start_y")) {
			startY = vm["start_y"].as<int>();
			startYSet = true;

			cout << "Starting y cell set to " << startY << ".\n";
		}

		if (vm.count("end_x")) {
			endX = vm["end_x"].as<int>();
			endXSet = true;

			cout << "Ending x cell set to " << endX << ".\n";
		}

		if (vm.count("end_y")) {
			endY = vm["end_y"].as<int>();
			endYSet = true;

			cout << "Ending y cell set to " << endY << ".\n";
		}

		if (vm.count("volume")) {

			numVolumes = tempVolumes.size();
			volumes = new double[numVolumes];

			// copy volumes to our array
			for (int i = 0; i < numVolumes; i++) {
				volumes[i] = tempVolumes[i];
			}
			volSet = true;

			cout << "Lahar volume(s) set to:" << endl;
			for (int i = 0; i < numVolumes; i++)
				cout << "  Volume " << (i + 1) << ":  " << volumes[i] << endl;
		}

		if (vm.count("verbose")) {
			verboseOn = true;

			cout << "Verbose mode has been set." << endl;
		}

		if (vm.count("status_timer")) {

			statTime = vm["status_timer"].as<int>();
			if (statTime == 0)
				cout << "Status timer disabled" << endl;
			else
				cout << "Status timer set to " << statTime << "." << endl;

		}
	}
	catch(const std::exception& e)
	{
		std::cout << "Exception: " << e.what() <<"\nProgram Exiting";
		return 1;
	}

	// new line
	cout << endl;

	// If the directory is set, add it to the file names
	if (directorySet) {
		metaName = directory + "/" + metaName;
		sdemName = directory + "/" + sdemName;
		fdirName = directory + "/" + fdirName;
		outName  = directory + "/" + outName;
	}

    // Input Rules:
	//	- name or all individual file names, start x, start y, and at least 1 volume must be set
	//  - there must be no duplicate volumes
    //  - volumes, x, y, status timer must be positive
    //  - x and y must be valid sizes in the 2d array
    bool inputMissing = false;
	bool invalidInput = false;

    // +-+-+-+-+-+-+-+ Set checks +-+-+-+-+-+-+-+
    if (!simpleNameOn) {
    	if (!metaNameSet) {
    		cout << "Error:  " << "Simple name not used and Meta data file name not set" << endl;
    		inputMissing = true;
    	}
    	if (!sdemNameSet) {
    		cout << "Error:  " << "Simple name not used and SDEM file name not set" << endl;
    		inputMissing = true;
    	}
    	if (!fdirNameSet) {
    		cout << "Error:  " << "Simple name not used and Flow direction file name not set" << endl;
    		inputMissing = true;
    	}
    	if (!outNameSet) {
    		cout << "Error:  " << "Simple name not used and Output file name not set" << endl;
    		inputMissing = true;
    	}
    }
    if (!startXSet) {
    	cout << "Error:  " << "Starting x cell not set" << endl;
		inputMissing = true;
    }
    if (!startYSet) {
    	cout << "Error:  " << "Starting y cell not set" << endl;
		inputMissing = true;
    }
    if (!volSet) {
    	cout << "Error:  " << "Volume not set" << endl;
		inputMissing = true;
    }
    if (endXSet && !endYSet) {
    	cout << "Error:  " << "Ending x cell set, but ending y cell not set" << endl;
		inputMissing = true;
    }
    if (!endXSet && endYSet) {
    	cout << "Error:  " << "Ending y cell set, but ending x cell not set" << endl;
		inputMissing = true;
    }
    if (inputMissing) {
    	cout << "Required input is missing\nProgram Exiting" << endl;
    	return 1;
    }
    else if (verboseOn)
    	cout << "All necessary input received" << endl;

    // +-+-+-+-+-+-+-+ Parse INI file +-+-+-+-+-+-+-+
    // cellSize, xCells, yCells initialized here
    if ( !parseINI(metaName) )
    	return 1;
    else
    	if (verboseOn)
    		cout << "INI file read successfully" << endl;

    // +-+-+-+-+-+-+-+ Input Validity Checks +-+-+-+-+-+-+-+
    if (startX < 0) {
    	cout << "Error:  " << startX << " is an invalid starting x position." << endl;
    	invalidInput = true;
    }
    if (startY < 0) {
    	cout << "Error:  " << startY << " is an invalid starting y position." << endl;
    	invalidInput = true;
    }
    if (endX < 0 && endXSet) {
    	cout << "Error:  " << endX << " is an invalid ending x position." << endl;
    	invalidInput = true;
    }
    if (endY < 0 && endYSet) {
    	cout << "Error:  " << endY << " is an invalid ending y position." << endl;
    	invalidInput = true;
    }
    if (startX > (xCells - 1)) {
    	cout << "Error:  " << "X start value, " << startX << ", exceeds the number of columns." << endl;
    	invalidInput = true;
    }
    if (startY > (yCells - 1)) {
    	cout << "Error:  " << "Y start value, " << startY << ", exceeds the number of rows." << endl;
    	invalidInput = true;
    }
    if (endX > (xCells - 1) && endXSet) {
    	cout << "Error:  " << "X end value, " << endX << ", exceeds the number of columns." << endl;
    	invalidInput = true;
    }
    if (endY > (yCells - 1) && endYSet) {
    	cout << "Error:  " << "Y end value, " << endY << ", exceeds the number of rows." << endl;
    	invalidInput = true;
    }
    if (statTime < 0) {
    	cout << "Error:  Status timer value, " << statTime << " is invalid." << endl;
    	invalidInput = true;
    }
    for (int i = 0; i < numVolumes; i++) {
    	if (volumes[i] <= 0) {
    		cout << "Error:  " << volumes[i] << " is an invalid volume." << endl;
    		invalidInput = true;
    	}

    	for (int j = i + 1; j < numVolumes; j++ ) {
    		if (volumes[i] == volumes[j]) {
    			cout << "Error:  " << volumes[i] << " is a duplicate volume." << endl;
    			invalidInput = true;
    			break;
    		}
    	}
    }

    if (invalidInput) {
    	cout << "Input given is invalid\nProgram Exiting" << endl;
    	return 1;
    }
    else if (verboseOn)
    	cout << "All program input is valid" << endl;

    // +-+-+-+-+-+-+-+ Parse Grids +-+-+-+-+-+-+-+

    // x is the row coord, y is the column coord

    //set up grids with ini file data
    elevGrid    = new double*[xCells];
    flowDirGrid = new double*[xCells];
    for (int i = 0; i < xCells; i++) {
    	elevGrid[i]    = new double [yCells];
    	flowDirGrid[i] = new double [yCells];
    }

    // parse each file; exit program if the return value is 0
    if ( !parseTSV(sdemName, elevGrid) )
    	return 1;

    if ( !parseTSV(fdirName, flowDirGrid) )
    	return 1;

    // +-+-+-+-+-+-+-+ Create IZM +-+-+-+-+-+-+-+
    IZMData data;
    data.flowDirGrid = flowDirGrid;
    data.elevGrid    = elevGrid;
    data.startX      = startX;
    data.startY 	 = startY;
    data.endX		 = endX;
    data.endY 		 = endY;
    data.coeffA		 = coeffA;
    data.coeffB		 = coeffB;
    data.outName     = outName;
    data.v  		 = verboseOn;

    MapperStatus * status = new MapperStatus(numVolumes, volumes);

    // Create the thread group, start each thread (including monitor status thread), and join them all
    boost::thread_group threads;
    for (int i = 0; i < numVolumes; i++)
    	threads.create_thread(boost::bind(&createIZM, data, volumes[i], i, status));
    threads.create_thread(boost::bind(&monitorStatus, status, statTime));
	threads.join_all( );

	status->printEndConditions( );

	cout << "Finished" << endl;
	return 1;
}

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
void monitorStatus(MapperStatus * status, int statTime) {

	if (statTime == 0) {
		return;
	}

	int interval = statTime * 1000;
	while(!status->finished( )) {
		status->printStatus( );
		boost::this_thread::sleep(boost::posix_time::milliseconds(interval));
	}

}

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
int parseINI(string name) {

	namespace pod = boost::program_options::detail;

	ifstream file;
	file.open(name.c_str());
	if(!file) {
		cout << "Attempted to load " << name << " but was unsuccessful\nProgram Exiting" << endl;
		return 0;
	}

	//parameters
	set<string> options;
	options.insert("Core.pixel_size");
	options.insert("Core.x_pixels");
	options.insert("Core.y_pixels");
	options.insert("Display.origin_x");
	options.insert("Display.origin_y");
	options.insert("Display.projection");

	try {
		for (pod::config_file_iterator i(file, options), e ; i != e; i++)
		{
			if (i->string_key == "Core.pixel_size")
				cellWidth = atof(i->value[0].c_str());

			else if (i->string_key == "Core.x_pixels")
				xCells = atoi(i->value[0].c_str());

			else if (i->string_key == "Core.y_pixels")
				yCells = atoi(i->value[0].c_str());

		}
	}
	catch(const std::exception& e)
	{
		std::cout << "Exception: " << e.what() <<"\nProgram Exiting";
		return 0;
	}

	file.close();
	return 1;
}

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
int parseTSV(string name, double ** grid) {

	ifstream file;
	file.open(name.c_str());
    if(!file) {
        cout << "Attempted to load " << name << " but was unsuccessful\nProgram Exiting" << endl;
        return 0;
    }

    // vars for parsing
	string line;
	string segment;
	stringstream stream;
	int lineCounter = 0;
	int segCounter = 0;

	// vars for progress bar
	int curPercentComplete = 0;
	int lastPercentComplete = 0;
	int change = 0;
	int changeCounter = 0;

	cout << endl << "Reading data from " << name << endl;
	cout << "0%____________________________________________100%" << endl; // __________________________________________________
	while (getline(file, line)) {

		stream << line;
		while (getline(stream, segment, '\t')) {
			grid[segCounter][lineCounter] = atof(segment.c_str());
			segCounter ++;
		}

		// Progress Bar *Maybe needs some optimization*
		curPercentComplete = ( (lineCounter + 1) * 100 / yCells);
		change = curPercentComplete - lastPercentComplete;
		lastPercentComplete = curPercentComplete;
		if ( change >= 1 )
			for (int i = 0; i < change; i++)
				changeCounter++;
		if ( changeCounter >= 2 ) {
			int asterisks = changeCounter / 2;
			changeCounter = changeCounter % 2;
			for (int i = 0; i < asterisks; i++) {
				cout << "*";
			}
		}
		cout.flush( );

		stream.clear();
		segCounter = 0;
		lineCounter ++;
	}

	cout << endl << name + " has been successfully read" << endl;

	/* Print
	for (int i = 0; i < yCells; i++) {
		cout << endl;
		for (int j = 0; j < xCells; j++) {
			cout << grid[j][i] << " ";
		}
	}*/

	file.close();
	return 1;
}

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
void outputInunGrid(double ** inunGrid, double volume, string outName, bool v) {

	string volString;
	stringstream strStream;
	strStream << volume;
	volString = strStream.str();

	outName = outName + "-zone" + volString + ".tsv";
	if (v) {
		cout << endl << "Writing inundation grid to " << outName << endl;
		cout << "0%____________________________________________100%" << endl;
	}
	ofstream file;
	file.open(outName.c_str());
    if(!file)
    	cout << "*ERROR* Attempted to create/load " << outName << " but was unsuccessful *ERROR*\n"
             <<	"Inundation grid for this volume has been lost, moving to next volume" << endl;


	// vars for progress bar
	int curPercentComplete = 0;
	int lastPercentComplete = 0;
	int change = 0;
	int changeCounter = 0;

    for (int i = 0; i < yCells; i++) {
    	for (int j = 0; j < xCells; j++) {
    		if ( j < (xCells - 1) )
    			file << inunGrid[j][i] << "\t";
    		else
    			file << inunGrid[j][i];
    	}
    	if ( i < (yCells - 1) )
    		file << "\r\n";

    	if (v) {

			// Progress Bar *Maybe needs some optimization*
			curPercentComplete = ( (i + 1) * 100 / yCells);
			change = curPercentComplete - lastPercentComplete;
			lastPercentComplete = curPercentComplete;
			if ( change >= 1 )
				for (int i = 0; i < change; i++)
					changeCounter++;
			if ( changeCounter >= 2 ) {
				int asterisks = changeCounter / 2;
				changeCounter = changeCounter % 2;
				for (int i = 0; i < asterisks; i++) {
					cout << "*";
				}
			}
			cout.flush( );
		}
    }

    file.close();

    if (v)
    	cout << endl << "Finished writing inundation grid" << endl;
}

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
int createIZM(IZMData data, double volume, int ID, MapperStatus * status) {


	// Locally store struct variables
	double ** flowDirGrid = data.flowDirGrid;
	double ** elevGrid    = data.elevGrid;
	int startX 			  = data.startX;
	int startY			  = data.startY;
	int endX			  = data.endX;
	int endY			  = data.endY;
	double coeffA		  = data.coeffA;
	double coeffB		  = data.coeffB;
	string outName 		  = data.outName;
	bool v				  = data.v;

	// Declare Process Variables
	double ** inunGrid;
	double maxCrossArea;
	double maxPlanArea;
	double curPlanArea = 0;
	int inunCellCounter = 0;
	int newRX = startX;
	int newRY = startY;

	// Set Variables
	maxCrossArea = getVolEqResult(coeffA, volume);
	maxPlanArea  = getVolEqResult(coeffB, volume);
    inunGrid = new double*[xCells];
    for (int i = 0; i < xCells; i++)
    	inunGrid[i] = new double [yCells];

	for (int i = 0; i < yCells; i++)
		for (int j = 0; j < xCells; j++)
			inunGrid[j][i] = 0;

	if (v) {
		cout << "Using coefficient " << coeffA << " and volume " << volume
		     << ", the cross sectional area is " << maxCrossArea << "." << endl;
		cout << "Using coefficient " << coeffB << " and volume " << volume
		     << ", the planimetric area is " << maxPlanArea << "." << endl;
		cout << "Inundation grid calculation starting with volume " << volume << "." << endl;
	}

	while(1) {

		int rX = newRX;
		int rY = newRY;
		int flowDir = (int) flowDirGrid[rX][rY];
		bool mapOverflowed = false;

		// Convert current position markers to strings to use for status updates
		string rXStr;
		string rYStr;
		stringstream streamx;
		stringstream streamy;
		streamx << rX;
		rXStr = streamx.str();
		streamy << rY;
		rYStr = streamy.str();

		// store the return values of the cross sections; this makes the code much more clean
		int cs1 = 0;
		int cs2 = 0;
		int cs3 = 0;
		int cs4 = 0;

		// Notes:  Each direction has three cross section calculations (excludes cross section in line with flow direction)
		//   NC = new stream cell based off current cell and flow direction
		switch(flowDir) {
		case 0: // North:     NSC[00][-1]

			if (v)
				cout << "Flow direction is N @ [" << rX << "][" << rY <<"]." << endl;

			// Cross section direction: E_W, NE_SW, SE_NW
			cs1 = calcCrossSection(inunGrid, elevGrid, rX, rY, maxCrossArea, E_W,   false, v);
			cs2 = calcCrossSection(inunGrid, elevGrid, rX, rY, maxCrossArea, NE_SW, false, v);
			cs3 = calcCrossSection(inunGrid, elevGrid, rX, rY, maxCrossArea, SE_NW, false, v);

			// Set the new stream cells for the next calculation
			newRX = rX;
			newRY = rY - 1;
			break;
		case 1: // Northeast: NSC[+1][-1]

			if (v)
				cout << "Flow direction is NE @ [" << rX << "][" << rY <<"]." << endl;

			// Cross section direction: SE_NW, E_W, N_S
			cs1 = calcCrossSection(inunGrid, elevGrid, rX, rY,     maxCrossArea, SE_NW, false, v);
			cs2 = calcCrossSection(inunGrid, elevGrid, rX, rY,     maxCrossArea, E_W,   false, v);
			cs3 = calcCrossSection(inunGrid, elevGrid, rX, rY,     maxCrossArea, N_S,   false, v);
			cs4 = calcCrossSection(inunGrid, elevGrid, rX, rY - 1, maxCrossArea, SE_NW, true,  v);


			// Set the new stream cells for the next calculation
			newRX = rX + 1;
			newRY = rY - 1;
			break;
		case 2: // East:      NSC[+1][00]

			if (v)
				cout << "Flow direction is E @ [" << rX << "][" << rY <<"]." << endl;

			// Cross section direction: N_S, NE_SW, SE_NW
			cs1 = calcCrossSection(inunGrid, elevGrid, rX, rY, maxCrossArea, N_S,   false, v);
			cs2 = calcCrossSection(inunGrid, elevGrid, rX, rY, maxCrossArea, NE_SW, false, v);
			cs3 = calcCrossSection(inunGrid, elevGrid, rX, rY, maxCrossArea, SE_NW, false, v);

			// Set the new stream cells for the next calculation
			newRX = rX + 1;
			newRY = rY;
			break;
		case 3: // Southeast: NSC[+1][+1]

			if (v)
				cout << "Flow direction is SE @ [" << rX << "][" << rY <<"]." << endl;

			// Cross section direction: NE_SW, E_W, N_S
			cs1 = calcCrossSection(inunGrid, elevGrid, rX, rY,     maxCrossArea, NE_SW, false, v);
			cs2 = calcCrossSection(inunGrid, elevGrid, rX, rY,     maxCrossArea, E_W,   false, v);
			cs3 = calcCrossSection(inunGrid, elevGrid, rX, rY,     maxCrossArea, N_S,   false, v);
			cs4 = calcCrossSection(inunGrid, elevGrid, rX, rY + 1, maxCrossArea, NE_SW, true,  v);


			// Set the new stream cells for the next calculation
			newRX = rX + 1;
			newRY = rY + 1;
			break;
		case 4: // South:     NSC[00][+1]

			if (v)
				cout << "Flow direction is S @ [" << rX << "][" << rY <<"]." << endl;

			// Cross section direction: E_W, NE_SW, SE_NW
			cs1 = calcCrossSection(inunGrid, elevGrid, rX, rY, maxCrossArea, E_W,   false, v);
			cs2 = calcCrossSection(inunGrid, elevGrid, rX, rY, maxCrossArea, NE_SW, false, v);
			cs3 = calcCrossSection(inunGrid, elevGrid, rX, rY, maxCrossArea, SE_NW, false, v);

			// Set the new stream cells for the next calculation
			newRX = rX;
			newRY = rY + 1;
			break;
		case 5: // Southwest: NSC[-1][+1]


			if (v)
				cout << "Flow direction is SW @ [" << rX << "][" << rY <<"]." << endl;

			// Cross section direction: SE_NW, E_W, N_S
			cs1 = calcCrossSection(inunGrid, elevGrid, rX, rY,     maxCrossArea, SE_NW, false, v);
			cs2 = calcCrossSection(inunGrid, elevGrid, rX, rY,     maxCrossArea, E_W,   false, v);
			cs3 = calcCrossSection(inunGrid, elevGrid, rX, rY,     maxCrossArea, N_S,   false, v);
			cs4 = calcCrossSection(inunGrid, elevGrid, rX, rY + 1, maxCrossArea, SE_NW, true,  v);

			// Set the new stream cells for the next calculation
			newRX = rX - 1;
			newRY = rY + 1;
			break;
		case 6: // West:      NSC[-1][00]

			if (v)
				cout << "Flow direction is W @ [" << rX << "][" << rY <<"]." << endl;

			// Cross section direction: N_S, NE_SW, SE_NW
			cs1 = calcCrossSection(inunGrid, elevGrid, rX, rY, maxCrossArea, N_S,   false, v);
			cs2 = calcCrossSection(inunGrid, elevGrid, rX, rY, maxCrossArea, NE_SW, false, v);
			cs3 = calcCrossSection(inunGrid, elevGrid, rX, rY, maxCrossArea, SE_NW, false, v);

			// Set the new stream cells for the next calculation
			newRX = rX - 1;
			newRY = rY;
			break;
		case 7: // Northwest: NSC[-1][-1]

			if (v)
				cout << "Flow direction is NW @ [" << rX << "][" << rY <<"]." << endl;

			// Cross section direction: NE_SW, E_W, N_S
			cs1 = calcCrossSection(inunGrid, elevGrid, rX, rY,     maxCrossArea, NE_SW, false, v);
			cs2 = calcCrossSection(inunGrid, elevGrid, rX, rY,     maxCrossArea, E_W,   false, v);
			cs3 = calcCrossSection(inunGrid, elevGrid, rX, rY,     maxCrossArea, N_S,   false, v);
			cs4 = calcCrossSection(inunGrid, elevGrid, rX, rY - 1, maxCrossArea, NE_SW, true,  v);

			// Set the new stream cells for the next calculation
			newRX = rX - 1;
			newRY = rY - 1;
			break;
		default:
			cout << "The Flow Direction Grid has supplied an unknown direction type.\nProgram exiting." << endl;
			return 0;

		}

		// Add up all the inundated cells from the cross section calculations
		if ( cs1 >= 0 )
			inunCellCounter += cs1;
		else {
			inunCellCounter += cs1 * -1;
			mapOverflowed = true;
		}
		if ( cs2 >= 0 )
			inunCellCounter += cs2;
		else {
			inunCellCounter += cs2 * -1;
			mapOverflowed = true;
		}
		if ( cs3 >= 0 )
			inunCellCounter += cs3;
		else {
			inunCellCounter += cs3 * -1;
			mapOverflowed = true;
		}
		if ( cs4 >= 0 )
			inunCellCounter += cs4;
		else {
			inunCellCounter += cs4 * -1;
			mapOverflowed = true;
		}


		curPlanArea = inunCellCounter * cellWidth * cellWidth;
		double percentComplete = curPlanArea / maxPlanArea * 100;

		status->setStatus(ID, percentComplete, false, false);

		if (v)
			cout << "	" << "The current planimetric area is " << curPlanArea << " and the max is "
				 << maxPlanArea << " (" << percentComplete << "%)" << endl;

		// Break condtions:
		// 		- The planimetric area is exceeded
		// 		- A cross section travels off the edge of the map
		//		- A stream cell travels off the edge of the map
		//		- The stream cell reaches the (endX, endY) location on the map
		if (curPlanArea >= maxPlanArea) {

			if (v)
				cout << "The maximum planimetric area has been reached @ [" << rX << "][" << rY <<"]." << endl;

			string end = "The maximum planimetric area has been reached @ [" + rXStr + "][" + rYStr + "].";
			status->setEndCondion(ID, end);
			break;
		}
		if (mapOverflowed) {

			if (v)
				cout << "A cross section has traveled off the map @ [" << rX << "][" << rY <<"]." << endl
				     << percentComplete << "% of the planimetric area was covered before the calculation has ended." << endl;

			string end = "A cross section has traveled off the map @ [" + rXStr + "][" + rYStr + "].";
			status->setEndCondion(ID, end);
			break;
		}
		if ( newRX < 0 || newRX > (xCells - 1) || newRY < 0 || newRY > (yCells -1) ) {

			if (v)
				cout << "Stream cell has traveled off the map @ [" << newRX << "][" << newRY <<"]." << endl
				     << percentComplete << "% of the planimetric area was covered before the calculation has ended." << endl;

			string end = "A Stream cell has traveled off the map @ [" + rXStr + "][" + rYStr + "].";
			status->setEndCondion(ID, end);
			break;
		}
		if ( (newRX == endX) && (newRY == endY) ) {

			if (v)
				cout << "Stream cell has reached the ending cell @ [" << newRX << "][" << newRY <<"]." << endl
				     << percentComplete << "% of the planimetric area was covered before the calculation has ended." << endl;

			string end = "A Stream cell has reached the ending cell @ [" + rXStr + "][" + rYStr + "].";
			status->setEndCondion(ID, end);
			break;
		}
	}

	if (v)
		cout << "Inundation grid calculation ending for volume " << volume <<  endl;

	status->setStatus(ID, 0, true, false);
	outputInunGrid(inunGrid, volume, outName, v);
	status->setStatus(ID, 0, false, true);

	return 1;
}

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
double getVolEqResult(double coeff, double volume) {

	double exp = (double) 2/3;
	double result = coeff * pow(volume, exp);
	return result;
}

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
 * 		If a cross section is successfully completed, the number of new inundated
 * 		cells from the cross section is returned.  If unsuccessful because the cross
 * 		section went off the map the negative number of new inundated cells from the
 * 		cross section is returned.
 */
int calcCrossSection(double ** inunGrid, double ** elevGrid, int rX, int rY, double maxCrossArea, CrossDir cD, bool gapCS, bool v) {

	// Define process variables
	double curCrossArea = 0;
	int crossWidth = 0;
	int newInunCounter = 0;
	int curRX = rX;
	int curRY = rY;
	int curLX;
	int curLY;
	int outRX;
	int outRY;
	int outLX;
	int outLY;
	double cWidth;
	double fillLevel;
	bool goingLeft = false;
	bool goingRight = false;


	// Set variables
	// Notes:
	//   LC  = Left cell position based off given stream cell position
	//	 ORC = The outbound modifiers for the right cell
	//   OLC = The outbound modifiers for the left cell
	switch (cD) {
	case N_S:   // LC: [00][+1]  ORC: [00][-1]  OLC[00][+1]

		if (v)
			cout << "	Calculating a N_S cross section" << endl;

		curLX = curRX;
		curLY = curRY + 1;
		outRX = 0;
		outRY = -1;
		outLX = 0;
		outLY = 1;
		cWidth = cellWidth;
		break;
	case NE_SW: // LC: [+1][-1]  ORC: [-1][+1]  OLC[+1][-1]

		if (v)
			cout << "	Calculating a NE_SW cross section" << endl;

		curLX = curRX + 1;
		curLY = curRY - 1;
		outRX = -1;
		outRY = 1;
		outLX = 1;
		outLY = -1;
		cWidth = sqrt(2 * cellWidth * cellWidth);
		break;
	case E_W:   // LC: [-1][00]  ORC: [+1][00]  OLC[-1][0]

		if (v)
			cout << "	Calculating a E_W cross section" << endl;

		curLX = curRX - 1;
		curLY = curRY;
		outRX = 1;
		outRY = 0;
		outLX = -1;
		outLY = 0;
		cWidth = cellWidth;
		break;
	case SE_NW: // LC: [-1][-1]  ORC: [+1][+1]  OLC[-1][-1]

		if (v)
 			cout << "	Calculating a SE_NW cross section" << endl;

		curLX = curRX - 1;
		curLY = curRY - 1;
		outRX = 1;
		outRY = 1;
		outLX = -1;
		outLY = -1;
		cWidth = sqrt(2 * cellWidth * cellWidth);
		break;
	default:
		cout << "*ERROR* Received a CrossDirection that does not exist *ERROR*" << endl;
	}

	fillLevel = elevGrid[curRX][curRY];

	// Set the stream cell to negative one for now so we can track the traversal
	// Note this could cause an index out of bounds on the gap calculation, so for
	// obvious reasons beyond causing an error state, it will not execute on a gap
	// calculation
	if ( !gapCS ) {
		if (inunGrid[curRX][curRY] == 0) {
			inunGrid[curRX][curRY] = -1;
			newInunCounter++;
		}
		else
			inunGrid[curRX][curRY] = -1;
	}

	while(1) {

		goingLeft = false;
		goingRight = false;
		double rightLevel;
		double leftLevel;

		// Determine if either cell has reached the edge of the map or hit an edge cell, and if so, return
		if ( curRX < 0 || curRX > (xCells - 1) || curRY < 0 || curRY > (yCells - 1) ||
			 curLX < 0 || curLX > (xCells - 1) || curLY < 0 || curLY > (yCells - 1)	||
			 fillLevel < -200 || elevGrid[curRX][curRY] < -200 || elevGrid[curLX][curLY] < -200 ) {

			if (v)
				cout << "Cross section for stream cell has extended off the map.  Inundation zone calculation is finished." << endl;

			// We need to differentiate that this happened in the return value, therefore, it must be negative
			if ( newInunCounter == 0 )
				newInunCounter++;
			return -newInunCounter;
		}
		else {
			rightLevel = elevGrid[curRX][curRY];
			leftLevel = elevGrid[curLX][curLY];
		}

		if (v)
			cout << "		At Right " << "[" << curRX << "][" << curRY << "]" << " and Left " "[" << curLX << "][" << curLY << "]"
			     << " :: FL = " << fillLevel << " || RL = " << rightLevel << " || LL = " << leftLevel << endl;

		// When both are greater than fill level, calculate area of difference between fill level and lowest elevation
		if ( rightLevel > fillLevel && leftLevel > fillLevel ) {

			double gapLevel;
			if ( rightLevel < leftLevel ) {
				gapLevel = rightLevel - fillLevel;
				goingRight = true;

				if (v)
					cout << "			Moving Right from " << "[" << curRX << "][" << curRY << "]" << endl;
			}
			else if ( rightLevel > leftLevel ) {
				gapLevel = leftLevel - fillLevel;
				goingLeft = true;

				if (v)
					cout << "			Moving Left from " << "[" << curLX << "][" << curLY << "]" << endl;
			}

			// If they are equal we will default to the right
			else if ( rightLevel == leftLevel ) {
				gapLevel = rightLevel - fillLevel;
				goingRight = true;

				if (v)
					cout << "			Moving Right from" << "[" << curRX << "][" << curRY << "] (default case, right and left levels equal)" << endl;
			}
			else {
				cout << "*ERROR* Gap level not set because previous conditions not met *ERROR*" << endl;
				return 0;
			}

			double tierArea = crossWidth * cWidth * gapLevel;
			curCrossArea += tierArea;


			if (v)
				cout << "				This tier area from filling gap is " << tierArea << " :: FL = " << fillLevel
			         << " || RL = " << rightLevel << " || LL = " << leftLevel
			         << " || CSW = " << crossWidth << " || CellW = " << cWidth << endl;
			crossWidth++;

			if (goingRight) {

				fillLevel = rightLevel;

				if (inunGrid[curRX][curRY] == 0) {
					inunGrid[curRX][curRY] = 1;
					newInunCounter++;
				}
				curRX += outRX;
				curRY += outRY;
			}
			else if (goingLeft) {

				fillLevel = leftLevel;

				if (inunGrid[curLX][curLY] == 0) {
					inunGrid[curLX][curLY] = 1;
					newInunCounter++;
				}
				curLX += outLX;
				curLY += outLY;
			}

		} // end of both are greater

		// Both left and right cells could be equal or lower than fill level
		else {

			if (rightLevel == fillLevel) {

				crossWidth++;

				if (v)
					cout << "			Moving Right from " << "[" << curRX << "][" << curRY << "]" << endl
						 << "				The right level is equal to the fill level so extending cross section." << endl;

				if (inunGrid[curRX][curRY] == 0) {
					inunGrid[curRX][curRY] = 1;
					newInunCounter++;
				}
				curRX += outRX;
				curRY += outRY;
			}

			else if (rightLevel < fillLevel) {

				crossWidth++;
				double gapLevel = fillLevel - rightLevel;
				double tierArea = cWidth * gapLevel;
				curCrossArea += tierArea;

				if (v)
					cout << "			Moving Right from " << "[" << curRX << "][" << curRY << "]" << endl
						 << "				Tier area is from fall is " << tierArea << " :: FL = " << fillLevel
						 << " || RL = " << rightLevel << " || LL = " << leftLevel
						 << " || CSW = " << crossWidth << " || CellW = " << cWidth << endl;

				if (inunGrid[curRX][curRY] == 0) {
					inunGrid[curRX][curRY] = 1;
					newInunCounter++;
				}
				curRX += outRX;
				curRY += outRY;
			}

			if (leftLevel == fillLevel) {

				crossWidth++;

				if (v)
					cout << "			Moving Left from " << "[" << curLX << "][" << curLY << "]" << endl
					     << "				The left level is equal to the fill level so extending cross section." << endl;

				if (inunGrid[curLX][curLY] == 0) {
					inunGrid[curLX][curLY] = 1;
					newInunCounter++;
				}
				curLX += outLX;
				curLY += outLY;
			}
			else if (leftLevel < fillLevel) {

				crossWidth++;
				double gapLevel = fillLevel - leftLevel;
				double tierArea = cWidth * gapLevel;
				curCrossArea += tierArea;

				if (v)
					cout << "			Moving Left from " << "[" << curLX << "][" << curLY << "]" << endl
						 << "				Tier area is from fall is " << tierArea << " :: FL = " << fillLevel
						 << " || RL = " << rightLevel << " || LL = " << leftLevel
						 << " || CSW = " << crossWidth << " || CellW = " << cWidth << endl;

				if (inunGrid[curLX][curLY] == 0) {
					inunGrid[curLX][curLY] = 1;
					newInunCounter++;
				}
				curLX += outLX;
				curLY += outLY;
			}
		}

		if (curCrossArea >= maxCrossArea) {

			if (v)
				cout << "		The maximum cross sectional area has been reached for stream cell ["
				     << rX << "][" << rY <<"].  The cross section width was " << crossWidth << endl;

			return newInunCounter;
		}
	} // end of while

	// never reached because while loop never breaks; here to make compiler happy :D
	return 1;
}
