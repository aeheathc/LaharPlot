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
					  "    - All input files must be in the same directory\n"
					  "    - If directory is specified, it must contain no\n"
					  "      spaces\n"
					  "    - '*' denotes a required option\n"
					  "    - The # symbol in the output file name represents\n"
					  "      which given volume the file is associated with\n"
					  "    - Volume(s) must be specified last\n";

	char * helpText = (char*) "Display this help\n";

	char * nameText = (char*) "Use the given simple name to open <arg>.ini, <arg>-sdem.tsv, "
			          "<arg>-ftotal.tsv, and <arg>-fdir.tsv files.  All files must "
			          "have same common name and be in the same directory.  The "
			          "program will output the Inundated Zone file using the same name.  "
			          "Can't be used with any other *-name options\n";

	char * dirText  = (char*) "Use the given directory <arg> to direct the program to all files.  "
					  "The Inundated Zone file will be output to the same directory.  "
					  "Default directory is the working directory containing the "
					  "executable.\n";

	char * metaText = (char*) "Read information about SDEM data from ini file with given name.	"
					  "Can't be used with simple-name.  (File type = <arg>.ini)\n";

	char * sdemText = (char*) "Read topography from file with given SDEM-file name.  Can't "
					  "be used with simple-name.  (File type = <arg>-sdem.tsv)\n";

	char * ftotText = (char*) "Read flow totals for each cell from file with given name.  "
					  "Can't be	used with simple-name.  (File type = <arg>-ftotal.tsv)\n";

	char * fdirText = (char*) "Read flow directions for each cell from file with given name.  "
					  "Can't be used with simple-name.  (File type = <arg>-fdir.tsv)\n";

	char * outText  = (char*) "Output Inundated Zone to file using given name.  Can't be "
					  "used with simple-name. (Result file = <arg>-zone#.tsv)\n";

	char * coAText  = (char*) "Desired coefficient for cross sectional area.  A = <arg> * V ^ (2/3) "
					  "The default value of 0.05 is set in accordance to Lahar prediction models.\n";

	char * coBText  = (char*) "Desired coefficient for planimetric area.  A = <arg> * V ^ (2/3) "
					  "The default value of 200 is set in accordance to Lahar prediction models.\n";

	char * stthText = (char*) "Threshold <arg> that determines which cells are classified as a "
					  "stream.  The default threshold is 1000.\n";

	char * stxText  = (char*) "'*' The simulated lahar's starting x value, <arg>.\n";

	char * styText  = (char*) "'*' The simulated lahar's starting y value, <arg>.\n";

	char * volText  = (char*) "'*' The simulated lahar's volumes, <arg>.  <arg> must contain "
					  "at least 1 volume in integer form.  Keep in mind that the more "
					  "volumes supplied, the longer this algorithm will take.";

	char * verbText = "Display detailed information.\n";

	// define variables to store command line information
	string simpleName;
	string directory;
	string metaName;
	string sdemName;
	string ftotName;
	string fdirName;
	string outName;
	double coeffA = .05;
	double coeffB = 200;
	int stThresh = 1000;
	int startX;
	int startY;
	double * volumes;
	vector <double> tempVolumes;
	int numVolumes;

	// booleans to determine if information has been set
	bool simpleNameOn = false;
	bool directorySet = false;
	bool metaNameSet = false;
	bool sdemNameSet = false;
	bool ftotNameSet = false;
	bool fdirNameSet = false;
	bool outNameSet = false;
	bool startXSet = false;
	bool startYSet = false;
	bool volSet = false;
	bool verboseOn = false;

	// file extensions
	string metaExt = ".ini";
	string sdemExt = "-sdem.tsv";
	string ftotExt = "-ftotal.tsv";
	string fdirExt = "-fdir.tsv";

	// File parsed grids
	double ** flowDirGrid;
	double ** flowTotGrid;
	double ** elevGrid;

	// +-+-+-+-+-+-+-+ Parse Program Options +-+-+-+-+-+-+-+ TODO
	try {
		// Declare the supported options.
		po::options_description desc(initText);
		desc.add_options()
			("help", helpText)
			("verbose,e", verbText)
			("simple-name,n", po::value<string>(), nameText)
			("directory,p", po::value<string>(), dirText)
			("meta-data-file-name,m", po::value<string>(), metaText)
			("SDEM-file-name,s", po::value<string>(), sdemText)
			("flow-totals-grid-name,t", po::value<string>(), ftotText)
			("flow-direction-grid-name,d", po::value<string>(), fdirText)
			("output-file-name,o", po::value<string>(), outText)
			("stream-threshold,h", po::value<int>(), stthText)
			("coefficient-A,a", po::value<double>(), coAText)
			("coefficient-B,b", po::value<double>(), coBText)
			("start-x,x", po::value<int>(), stxText)
			("start-y,y", po::value<int>(), styText)
			("volume,v", po::value< vector<double> >(&tempVolumes)->multitoken(), volText)
		;

		po::variables_map vm;
		po::store(po::parse_command_line(ac, av, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			std::cout << desc << "\n";
			return 1;
		}

		if (vm.count("simple-name")) {
			simpleName = vm["simple-name"].as<string>();
			metaName = vm["simple-name"].as<string>() + metaExt;
			sdemName = vm["simple-name"].as<string>() + sdemExt;
			ftotName = vm["simple-name"].as<string>() + ftotExt;
			fdirName = vm["simple-name"].as<string>() + fdirExt;
			outName = vm["simple-name"].as<string>();
			simpleNameOn = true;

			cout << "Simple name set to " << simpleName << ".  File names are as follows:\n"
				 << "  Meta File Name:            " << metaName << "\n"
				 << "  SDEM File Name:            " << sdemName << "\n"
				 << "  Flow Totals File Name:     " << ftotName << "\n"
				 << "  Flow Direction File Name:  " << fdirName << "\n"
				 << "  Output file name(s):       " << outName  << "-zone#.tsv" << endl;
		}

		if (vm.count("directory")) {
			directory = vm["directory"].as<string>();
			directorySet = true;

			cout << "Directory set to " << directory << endl;
		}

		if (vm.count("meta-data-file-name")) {

			if (simpleNameOn)
				cout << "Meta data file name not set -> Simple name already designated" << endl;

			else {
				metaName = vm["meta-data-file-name"].as<string>() + metaExt;
				metaNameSet = true;

				cout << "Meta data file name set to: " << metaName << endl;
			}
		}

		if (vm.count("SDEM-file-name")) {

			if (simpleNameOn)
				cout << "SDEM file name not set -> Simple name already designated" << endl;

			else {
				sdemName = vm["SDEM-file-name"].as<string>() + sdemExt;
				sdemNameSet = true;

				cout << "SDEM file name set to: " << sdemName << endl;
			}
		}

		if (vm.count("flow-totals-grid-name")) {

			if (simpleNameOn)
				cout << "Flow totals grid name not set -> Simple name already designated" << endl;

			else {
				ftotName = vm["flow-totals-grid-name"].as<string>() + ftotExt;
				ftotNameSet = true;

				cout << "Flow totals file name set to: " << ftotName << endl;
			}
		}

		if (vm.count("flow-direction-grid-name")) {

			if (simpleNameOn)
				cout << "Flow direction grid name not set -> Simple name already designated" << endl;

			else {
				fdirName = vm["flow-direction-grid-name"].as<string>() + fdirExt;
				fdirNameSet = true;

				cout << "Flow direction file name set to: " << fdirName << endl;
			}
		}

		if (vm.count("output-file-name")) {

			if (simpleNameOn)
				cout << "Output name not set -> Simple name already designated" << endl;

			else {
				outName = vm["output-file-name"].as<string>();
				outNameSet = true;

				cout << "Output name set to: " << outName << endl;
			}
		}

		if (vm.count("coefficient-A")) {
			coeffA = vm["coefficient-A"].as<double>();

			cout << "Coefficient A set to " << coeffA << ".\n";
		}

		if (vm.count("coefficient-B")) {
			coeffB = vm["coefficient-B"].as<double>();

			cout << "Coefficient B set to " << coeffB << ".\n";
		}


		if (vm.count("stream-threshold")) {
			stThresh = vm["stream-threshold"].as<int>();

			cout << "Stream threshold set to " << stThresh << ".\n";
		}

		if (vm.count("start-x")) {
			startX = vm["start-x"].as<int>();
			startXSet = true;

			cout << "Starting x cell set to " << startX << ".\n";
		}

		if (vm.count("start-y")) {
			startY = vm["start-y"].as<int>();
			startYSet = true;

			cout << "Starting y cell set to " << startY << ".\n";
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

			cout << "Verbose mode has been set" << endl;
		}
	}
	catch(const std::exception& e)
	{
		std::cout << "Exception: " << e.what() <<"\nProgram Exiting";
		return 1;
	}

	// If the directory is set, add it to the file names
	if (directorySet) {
		metaName = directory + "/" + metaName;
		sdemName = directory + "/" + sdemName;
		ftotName = directory + "/" + ftotName;
		fdirName = directory + "/" + fdirName;
		outName  = directory + "/" + outName;
	}

    // Input Rules:
	//	- name or all individual file names, start x, start y, and at least 1 volume must be set
    //  - volumes, x, y, threshold must be positive
    //  - x and y must be valid sizes in the 2d array
    bool inputMissing = false;
	bool invalidInput = false;

    // +-+-+-+-+-+-+-+ Set checks +-+-+-+-+-+-+-+ TODO
    if (!simpleNameOn) {
    	if (!metaNameSet) {
    		cout << "Error:  " << "Simple name not used and Meta data file name not set" << endl;
    		inputMissing = true;
    	}
    	if (!sdemNameSet) {
    		cout << "Error:  " << "Simple name not used and SDEM file name not set" << endl;
    		inputMissing = true;
    	}
    	if (!ftotNameSet) {
    		cout << "Error:  " << "Simple name not used and Flow totals file name not set" << endl;
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

    if (invalidInput) {
    	cout << "Required input is missing\nProgram Exiting" << endl;
    	return 1;
    }


    // +-+-+-+-+-+-+-+ Parse INI file +-+-+-+-+-+-+-+ TODO
    // cellSize, xCells, yCells initialized here
    if ( !parseINI(metaName) ) {
    	return 1;
    }

    // +-+-+-+-+-+-+-+ Input Validity Checks +-+-+-+-+-+-+-+ TODO
    if (startX < 0) {
    	cout << "Error:  " << startX << " is an invalid starting x position." << endl;
    	invalidInput = true;
    }
    if (startY < 0) {
    	cout << "Error:  " << startY << " is an invalid starting y position." << endl;
    	invalidInput = true;
    }
    if (startX > (xCells - 1)) {
    	cout << "Error:  " << "X start value, " << startX << ", exceeds the number of columns" << endl;
    	invalidInput = true;
    }
    if (startY > (yCells - 1)) {
    	cout << "Error:  " << "Y start value, " << startY << ", exceeds the number of rows" << endl;
    	invalidInput = true;
    }
    if (stThresh <= 0) {
    	cout << "Error:  " << stThresh << " is an invalid stream threshold." << endl;
    }
    for (int i = 0; i < numVolumes; i++)
    	if (volumes[i] <= 0) {
    		cout << "Error:  " << volumes[i] << " is an invalid volume." << endl;
    		invalidInput = true;
    }
    if (invalidInput) {
    	cout << "Input given is invalid\nProgram Exiting" << endl;
    	return 1;
    }

    // +-+-+-+-+-+-+-+ Parse Grids +-+-+-+-+-+-+-+ TODO

    // x is the row coord, y is the column coord

    //set up grids with ini file data
    elevGrid    = new double*[xCells];
    flowTotGrid = new double*[xCells];
    flowDirGrid = new double*[xCells];
    for (int i = 0; i < xCells; i++) {
    	elevGrid[i]    = new double [yCells];
    	flowTotGrid[i] = new double [yCells];
    	flowDirGrid[i] = new double [yCells];
    }

    // parse each file; exit program if the return value is 0
    if ( !parseTSV(sdemName, elevGrid) ) {
    	return 1;
    }

    if ( !parseTSV(ftotName, flowTotGrid) ) {
    	return 1;
    }

    if ( !parseTSV(fdirName, flowDirGrid) ) {
    	return 1;
    }

    // +-+-+-+-+-+-+-+ Create IZM +-+-+-+-+-+-+-+ TODO

    for (int i = 0; i < numVolumes; i++)
    	if ( !createIZM(flowDirGrid, elevGrid, startX, startY, coeffA, coeffB, volumes[i], outName, cellWidth) )
    		return 1;

	cout << "Finished" << endl;
	return 1;
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
	options.insert("Display.zone");

	try {
		for (pod::config_file_iterator i(file, options), e ; i != e; i++)
		{
			if (i->string_key == "Core.pixel_size") {
				cellWidth = atof(i->value[0].c_str());
				// cout << "Cell size set to " << cellWidth << endl;
			}
			else if (i->string_key == "Core.x_pixels") {
				xCells = atoi(i->value[0].c_str());
				// cout << "Number of columns set to " << xCells << endl;
			}

			else if (i->string_key == "Core.y_pixels") {
				yCells = atoi(i->value[0].c_str());
				// cout << "Number of rows set to " << yCells << endl;
			}
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

	string line;
	string segment;
	stringstream stream;
	int lineCounter = 0;
	int segCounter = 0;

	while (getline(file, line)) {
		stream << line;
		while (getline(stream, segment, '\t')) {
			grid[segCounter][lineCounter] = atof(segment.c_str());
			segCounter ++;
		}
		stream.clear();
		segCounter = 0;
		lineCounter ++;
	}

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
	if (v)
		cout << "Writing inundation grid to " << outName << endl;

	ofstream file;
	file.open(outName.c_str());
    if(!file)
    	cout << "*ERROR* Attempted to create/load " << outName << " but was unsuccessful *ERROR*\n"
             <<	"Inundation grid for this volume has been lost, moving to next volume" << endl;


    for (int i = 0; i < yCells; i++) {
    	for (int j = 0; j < xCells; j++) {
    		if ( j < (xCells - 1) )
    			file << inunGrid[j][i] << "\t";
    		else
    			file << inunGrid[j][i];
    	}
    	if ( i < (yCells - 1) )
    		file << "\r\n";
    }

    file.close();

    if (v)
    	cout << "Finished writing inundation grid" << endl;
}

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
			  double coeffA, double coeffB, double volume, string outName, bool v) {

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

		// store the return values of the cross sections; this makes the code much more clean
		int cs1 = 0;
		int cs2 = 0;
		int cs3 = 0;
		int cs4 = 0;

		// Notes:  Each direction has three cross section calculations (excludes cross section in line with flow direction)
		//   NC = new stream cell based off current cell and flow direction
		switch(flowDir) {
		case 0: // North:     NSC[00][-1]
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


		// TODO Get rid of temp error check
		double tempCurPlanArea = curPlanArea;
		curPlanArea = inunCellCounter * cellWidth * cellWidth;
		if (tempCurPlanArea > curPlanArea)
			cout << "Hmmm... somehow planimetric area decreased... this is bad." << endl;

		double percentComplete = curPlanArea / maxPlanArea * 100;

		cout << "	" << curPlanArea << " || " << maxPlanArea << " || " << percentComplete << "%" << endl;
		if (curPlanArea >= maxPlanArea) {
			cout << "The maximum planimetric area has been reached @ [" << rX << "][" << rY <<"]." << endl;
			break;
		}
		if (mapOverflowed) {
			cout << "A cross section has traveled off the map @ [" << rX << "][" << rY <<"]." << endl;
			cout << percentComplete << "% of the planimetric area was covered before the calculation has ended." << endl;
			break;
		}
		if ( newRX < 0 || newRX > (xCells - 1) || newRY < 0 || newRY > (yCells -1) ) {
			cout << "Stream cell has traveled off the map @ [" << newRX << "][" << newRY <<"]." << endl;
			cout << percentComplete << "% of the planimetric area was covered before the calculation has ended." << endl;
			break;
		}
	}

	if (v)
		cout << "Inundation grid calculation ending for volume " << volume << endl << endl;

	outputInunGrid(inunGrid, volume, outName, v);

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

		// Determine if either cell has reached the edge of the map, and if so, return
		if ( curRX < 0 || curRX > (xCells - 1) || curRY < 0 || curRY > (yCells - 1) ||
			 curLX < 0 || curLX > (xCells - 1) || curLY < 0 || curLY > (yCells - 1)	) {

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



cout << "	Fill lvl: " << fillLevel << "Right lvl:" << rightLevel << "Left lvl:" << leftLevel << endl;

		// When both are greater than fill level, calculate area of difference between fill level and lowest elevation
		if ( rightLevel > fillLevel && leftLevel > fillLevel ) {

			double gapLevel;
			if ( rightLevel < leftLevel ) {
				gapLevel = rightLevel - fillLevel;
				goingRight = true;
cout << "	Moving Right from" << "[" << curRX << "][" << curRY << "]" << endl;
				}
			else if ( rightLevel > leftLevel ) {
				gapLevel = leftLevel - fillLevel;
				goingLeft = true;
cout << "	Moving Left from " << "[" << curLX << "][" << curLY << "]" << endl;
			}

			// If they are equal we will default to the right
			else if ( rightLevel == leftLevel ) {
				gapLevel = rightLevel - fillLevel;
				goingRight = true;
cout << "	Moving Right from" << "[" << curRX << "][" << curRY << "]" << endl;
			}
			else {
				cout << "*ERROR* Gap level not set because previous conditions not met *ERROR*" << endl;
				return 0;
			}

			double tierArea = crossWidth * cWidth * gapLevel;
			curCrossArea += tierArea;
			cout << "	Tier area is " << tierArea << " from FL " << fillLevel << ", RL " <<rightLevel << ", LL " << leftLevel
			     << ", CW " << crossWidth << ", cW " << cWidth << endl;	//TODO remove later
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
		else {  // Both left and right cells could be equal to or lower than fill level

			if (rightLevel == fillLevel) {

				crossWidth++;

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
cout << "	Tier area is from a fall: " << tierArea << " from FL " << fillLevel << ", RL " <<rightLevel << ", cW " << cWidth << endl;	//TODO remove later

				if (inunGrid[curRX][curRY] == 0) {
					inunGrid[curRX][curRY] = 1;
					newInunCounter++;
				}
				curRX += outRX;
				curRY += outRY;
			}

			if (leftLevel == fillLevel) {

				crossWidth++;

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
cout << "	Tier area is from a fall: " << tierArea << " from FL " << fillLevel << ", LL " << leftLevel << ", cW " << cWidth << endl;	//TODO remove later

				if (inunGrid[curLX][curLY] == 0) {
					inunGrid[curLX][curLY] = 1;
					newInunCounter++;
				}
				curLX += outLX;
				curLY += outLY;
			}
		}

		if (curCrossArea >= maxCrossArea) {
			cout << "The maximum cross sectional area has been reached for stream cell [" << rX << "][" << rY <<"]." << endl;
			return newInunCounter;
		}
	} // end of while

	// never reached because while loop never breaks; here to make compiler happy :D
	return 1;
}


