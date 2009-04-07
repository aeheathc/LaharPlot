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

#include "main.h"

int main(int argc, char* argv[])
{
	// Declare the supported options.
	po::options_description desc("Usage: stream [OPTION]...");
	desc.add_options()
		("help", "Display this help and exit")
		("input-file,f", po::value<string>(), "Read topography from input file <arg>")
		("std-in,i", "Read topography from standard-in. Can't be used with --input-file.")
		("output-file,o", po::value<string>(), "Output to files using the base name <arg>.")
		("std-out,t", "Output to standard-out. May be used with --output-file. Overrides --verbose.")
		("threads,r", po::value<int>(),
			"Set number of threads for parallel calculations. Default is 4.")
		("verbose,v", "Display detailed progress.")
	;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);    

	//Show usage information when the user asks for help.
	if(vm.count("help"))
	{
		cout << desc << "\n";
		return 0;
	}
	
	//check for various contradictory option settings, or missing required options.
	string optError = "";
	string infile = "", outfile = "";
	cmdIn = vm.count("std-in");
	cmdOut = vm.count("std-out");
	verbose = vm.count("verbose");
	int threads = vm.count("threads") ? abs(vm["threads"].as<int>()) : 4;
	fs::ofstream sDem, meta, flowDir, flowTotal;
	
	if(vm.count("input-file"))
	{
		infile = vm["input-file"].as<string>();
		if(!fs::exists(infile))
		{
			optError = infile + ": No such file or directory\n";
		}else if(fs::is_directory(infile)){
			optError = infile + ": Is a directory\n";
		}
		if(cmdIn)
			optError = "can't read from both input-file and cmdIn at once\n";
	}else{
		if(!cmdIn)
			optError = "no input source specified\nTry 'stream --help' for more information.\n";
	}
	
	if(vm.count("output-file"))
	{
		fileOut = true;
		outfile = vm["output-file"].as<string>();
		if(outfile.empty())
		{
			optError = "invalid filename\n";
		}else{
			sDem.open(outfile+"-sdem.tsv");
			meta.open(outfile+".ini");
			flowDir.open(outfile+"-fdir.tsv");
			flowTotal.open(outfile+"-ftotal.tsv");
			if(!(sDem && meta && flowDir && flowTotal))
				optError = string("couldn't open output files\n(Is the filename valid?)")
							+"\n(Is there a permissions issue?)\n";
		}
	}else{
		if(!cmdOut)
			optError = "no output method specified\nTry 'stream --help' for more information.\n";
	}

	if(optError != "")
	{
		cout << "stream: " << optError << "\n";
		return 1;
	}
	
	//Done setting up. Now, start reading the DEM.
	if(verbose) cout << "Reading file...\n";
	GDALDataset  *poDataset;
	GDALAllRegister();
	poDataset = (GDALDataset*)GDALOpen(infile.c_str(), GA_ReadOnly);
	if(poDataset == NULL)
	{
		cout << "There was a problem opening the topography file.\n";
		return 1;
	}

	Metadata iniData;
	double	adfGeoTransform[6];
	int	cellsX = poDataset->GetRasterXSize(),
		cellsY = poDataset->GetRasterYSize();
		//layers = poDataset->GetRasterCount();
			
    if(poDataset->GetProjectionRef() != NULL)
	{
		iniData.projection = poDataset->GetProjectionRef();
	}

    if(poDataset->GetGeoTransform( adfGeoTransform ) == CE_None)
    {
		iniData.originX = adfGeoTransform[0];
		iniData.originY = adfGeoTransform[3];
		iniData.physicalSize = adfGeoTransform[1];
		//above size is X-size. Y-size is in adfGeoTransform[5]
	}
	//set up shared memory
	Cell *dem = NULL;
	float *pafScanline = NULL;
	ip::shared_memory_object::remove("stream_finder_dem");
	ip::managed_shared_memory *segment=NULL;
	ip::managed_shared_memory::handle_t linearHandle;
	try{
		size_t matrixBytes = sizeof(Cell) * cellsX * cellsY;
		size_t linearBytes = sizeof(float) * cellsX * cellsY;
		size_t flowBytes = (sizeof(Point) * cellsX * cellsY * (cellsX * cellsY / 10000000 + 1)) - linearBytes;
		//size_t flowBytes = (sizeof(Point) * cellsX * cellsY * 25);
		segment = new ip::managed_shared_memory(ip::create_only,
							"stream_finder_dem",		//segment name
							matrixBytes+linearBytes+flowBytes+1000);	//segment size in bytes
		Cell::alloc_inst = new PointShmemAllocator(segment->get_segment_manager());
		dem = segment->construct<Cell>("DEM")[cellsX * cellsY]();
		pafScanline = (float*)segment->allocate(linearBytes);
		linearHandle = segment->get_handle_from_address(pafScanline);
	}
	catch(ip::interprocess_exception& e)
	{
		ip::shared_memory_object::remove("stream_finder_dem");
		cout << "Error creating/allocating main shared memory: " << e.what() << "\n";
		throw;
	}
	GDALRasterBand  *poBand;
	int             nBlockXSize, nBlockYSize;
	int             bGotMin, bGotMax;
	double          adfMinMax[2];
	string			demDT;
	poBand = poDataset->GetRasterBand(1);
	poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
	demDT = poBand->GetRasterDataType();
	adfMinMax[0] = poBand->GetMinimum( &bGotMin );
	adfMinMax[1] = poBand->GetMaximum( &bGotMax );
	if(!(bGotMin && bGotMax))
		GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
	nXSize = abs(poBand->GetXSize());
	nYSize = abs(poBand->GetYSize());
	poBand->RasterIO( GF_Read, 0, 0, nXSize, nYSize, pafScanline, nXSize, nYSize, GDT_Float32, 0, 0 );
	GDALClose((GDALDatasetH*)poDataset);
	
	if(nXSize < 2 || nYSize < 2)
	{
		cout << "Something is wrong with the input DEM. Aborting.\n";
		segment->destroy<Cell>("DEM");
		segment->deallocate((void*)pafScanline);
		delete segment;
		ip::shared_memory_object::remove("stream_finder_dem");
		return 1;
	}
	
	linear(pafScanline,0,0,nXSize);	//initialize static variable inside linear()
	linear(dem,0,0,nXSize);
		
	//Fill in the sinkholes!
	if(verbose) cout << "Filling sinkholes...\n";
	FillSinks filler(pafScanline, nYSize, nXSize, 1);
	filler.fill();
	
	//We have the data from the file, now we make the data 2-dimensional for easier reading.
	//The cells on the outside are made with default outward flow directions.
	if(threads > nYSize) threads = nYSize;
	int rowsPerThread = nYSize / threads;
	if(verbose)
	{
		cout << "XSize=" << nXSize << ",YSize=" << nYSize << ",Cells="
			<< (nXSize*nYSize) << "\nBuilding DEM...\n";
	}
	boost::thread_group demFiller;
	
	for(int thread=0; thread<(threads-1); thread++)
	{
		int firstRow = thread*rowsPerThread;
		demFiller.add_thread(new boost::thread(linearTo2d, firstRow,
												firstRow+rowsPerThread, linearHandle));
	}
	demFiller.add_thread(new boost::thread(linearTo2d, (threads-1)*rowsPerThread,
											nYSize, linearHandle));
	
	demFiller.join_all();	//wait until all the data is in place before doing calcs on it
	segment->deallocate((void*)pafScanline);
	//Done reading DEM, now do calculations.

	if(verbose) cout << "\nCalculating...\nFinding flow direction...\n";
	//make flow direction grid
	boost::thread_group flowDirCalc;
	for(int thread=0; thread<(threads-1); thread++)
	{
		int firstRow = thread*rowsPerThread;
		flowDirCalc.add_thread(new boost::thread(flowDirection, firstRow, firstRow+rowsPerThread));
	}
	flowDirCalc.add_thread(new boost::thread(flowDirection, (threads-1)*rowsPerThread, nYSize));
	flowDirCalc.join_all();

	//make flow total grid
	if(verbose) cout << "\nFinding flow totals...\n";
	boost::thread_group flowTotalCalc;
	for(int thread=0; thread<(threads-1); thread++)
	{
		int firstRow = thread*rowsPerThread;
		flowTotalCalc.add_thread(new boost::thread(flowTrace, firstRow, firstRow+rowsPerThread));
		if(verbose) cout << "Assigned thread.\n";
	}
	flowTotalCalc.add_thread(new boost::thread(flowTrace, (threads-1)*rowsPerThread, nYSize));
	flowTotalCalc.join_all();

	if(verbose) cout << "Writing output...\n";
	
	//write output
	boost::thread_group writeout;
	if(fileOut)	writeFiles(dem, sDem, meta, flowDir, flowTotal, iniData);
	if(cmdOut)	writeout.add_thread(new boost::thread(writeStdOut, iniData));
	writeout.join_all();
	
	//Free shared memory
	delete Cell::alloc_inst;
	segment->destroy<Cell>("DEM");
	delete segment;
	ip::shared_memory_object::remove("stream_finder_dem");
	cout << EOF;
	return 0;
}

void flowDirection(int row, int end)
{
	Cell *dem=NULL;
	ip::managed_shared_memory *segment=NULL;
	const unsigned short max_att = 10;
	unsigned short attempt = 0;
	ip::interprocess_exception* e = NULL;
	for(attempt = 0; attempt < max_att; attempt++)
	{
		try{
			segment = new ip::managed_shared_memory(ip::open_only, "stream_finder_dem");
			dem = segment->find<Cell>("DEM").first;
			break;
		}
		catch(ip::interprocess_exception& ex){
			cout << "Error accessing the shared memory objects in flowDirection for attempt "
				<< attempt << ":\n" << ex.what() << "\n";
			e = &ex;
			sleep(2);
		}
	}
	if(attempt == max_att)
	{
		cout << "Can't get shared memory in flowDirection.\n";
		throw *e;
	}
	
	//We don't deal with cells on the outer boundary because their default
	//flow direction is outward and that is what we want.
	if(!row) row++;
	if(end == nYSize) end--;
	
	for(;row < end; row++)
	{
		for(int column = 1; column < (nXSize-1); column++)
		{
			if(verbose) cout << '.';
			linear(dem,row,column).flowDir = greatestSlope(dem, row, column);
		}
	}
	delete segment;
}

direction greatestSlope(Cell* dem, int row, int column)
{
	vector<float> slopes(8,0);
	//method 1 (rudiger)
	/*slopes[north]		= linear(dem,row+1,column).height	- linear(dem,row-1,column).height;
	slopes[northeast]	= linear(dem,row+1,column-1).height	- linear(dem,row-1,column+1).height;
	slopes[east]		= linear(dem,row,column-1).height	- linear(dem,row,column+1).height;
	slopes[southeast]	= linear(dem,row-1,column-1).height	- linear(dem,row+1,column+1).height;
	slopes[south]		= -slopes[north];
	slopes[southwest]	= -slopes[northeast];
	slopes[west]		= -slopes[east];
	slopes[northwest]	= -slopes[southeast];*/
	
	//method 2 (tony)
	slopes[north]		= linear(dem,row,column).height	- linear(dem,row-1,column).height;
	slopes[northeast]	= linear(dem,row,column).height	- linear(dem,row-1,column+1).height;
	slopes[east]		= linear(dem,row,column).height	- linear(dem,row,column+1).height;
	slopes[southeast]	= linear(dem,row,column).height	- linear(dem,row+1,column+1).height;
	slopes[south]		= linear(dem,row,column).height - linear(dem,row+1,column).height;
	slopes[southwest]	= linear(dem,row,column).height - linear(dem,row+1,column-1).height;
	slopes[west]		= linear(dem,row,column).height - linear(dem,row,column-1).height;
	slopes[northwest]	= linear(dem,row,column).height - linear(dem,row-1,column-1).height;
	
	//method 3 (null)
	/*slopes[north]		= 0;
	slopes[northeast]	= 0;
	slopes[east]		= 0;
	slopes[southeast]	= 0;
	slopes[south]		= 1;
	slopes[southwest]	= 0;
	slopes[west]		= 0;
	slopes[northwest]	= 0;*/
	
	int max=north;
	for(int dir=northeast; dir < none; dir++)
	{
		if(slopes[dir] > slopes[max])
			max = dir;
	}
	return intDirection(max);
}

void linearTo2d(int firstRow, int end, ip::managed_shared_memory::handle_t linearHandle)
{
	float* linearData;
	Cell *dem=NULL;
	ip::managed_shared_memory *segment=NULL;
	const unsigned short max_att = 10;
	unsigned short attempt = 0;
	ip::interprocess_exception* e = NULL;
	for(attempt = 0; attempt < max_att; attempt++)
	{
		try{
			segment = new ip::managed_shared_memory(ip::open_only, "stream_finder_dem");
			dem = segment->find<Cell>("DEM").first;
			linearData = (float*)segment->get_address_from_handle(linearHandle);
			break;
		}
		catch(ip::interprocess_exception& ex){
			cout << "Error accessing the shared memory objects in linearTo2d for attempt "
				<< attempt << ":\n" << ex.what() << "\n";
			e = &ex;
			sleep(2);
		}
	}
	if(attempt == max_att)
	{
		cout << "Can't get shared memory in linearTo2d.\n";
		throw *e;
	}
	
	int yp = firstRow;
	if(firstRow == 0)
	{
		linear(dem,yp,0).fill(linear(linearData, yp, 0), yp, 0, northwest);
		for(int xp = 1; xp<(nXSize-1); xp++)
		{
			linear(dem,yp,xp).fill(linear(linearData, yp, xp), yp, xp, north);
		}
		linear(dem,yp,nXSize-1).fill(linear(linearData, yp, nXSize-1), yp, nXSize-1, northeast);
		yp++;
	}
	int lastNormRow = (end==nYSize) ? end-1 : end;
	for(; yp<lastNormRow; yp++)
	{
		linear(dem,yp,0).fill(linear(linearData, yp, 0), yp, 0, west);
		for(int xp = 1; xp<(nXSize-1); xp++)
		{
			if(verbose) cout << '-';
			linear(dem,yp,xp).fill(linear(linearData, yp, xp), yp, xp);
		}
		linear(dem,yp,nXSize-1).fill(linear(linearData, yp, nXSize-1), yp, nXSize-1, east);
	}
	if(lastNormRow != end)
	{
		linear(dem,yp,0).fill(linear(linearData, yp, 0), yp, 0, southwest);
		for(int xp = 1; xp<(nXSize-1); xp++)
		{
			linear(dem,yp,xp).fill(linear(linearData, yp, xp), yp, xp, south);
		}
		linear(dem,yp,nXSize-1).fill(linear(linearData, yp, nXSize-1), yp, nXSize-1, southeast);
	}
	delete segment;
}

void writeFiles(Cell* dem, fs::ofstream& sdem, fs::ofstream& meta, fs::ofstream& fdir, fs::ofstream& ftotal, Metadata& iniData)
{
	//write Simplified DEM
	for(int row=0; row<nYSize; row++)
	{
		for(int column=0; column<(nXSize-1); column++)
		{
			sdem << linear(dem,row,column).height << '\t';
		}
		sdem << linear(dem,row,nXSize-1).height << '\n';
	}
	sdem.close();

	//write Metadata INI
	meta << fixed << setprecision(0) << "[Core]\npixel_size=" << iniData.physicalSize
			<< "\nx_pixels=" << nXSize << "\ny_pixels=" << nYSize
			<< "\n[Display]\norigin_x=" << iniData.originX << "\norigin_y="
			<< iniData.originY << "\nprojection=" << iniData.projection << "\n";
	meta.close();

	//Write Flow Direction Grid
	for(int row=0; row<nYSize; row++)
	{
		for(int column=0; column<(nXSize-1); column++)
		{
			fdir << (int)(linear(dem,row,column).flowDir) << '\t';
		}
		fdir << (int)(linear(dem,row,nXSize-1).flowDir) << '\n';
	}
	fdir.close();

	//write Flow Total Grid
	ftotal << fixed << setprecision(0);
	for(int row=0; row<nYSize; row++)
	{
		int numOut = 0;
		for(int column=0; column<(nXSize-1); column++)
		{
			numOut = linear(dem,row,column).flowTotal.size();
			ftotal << numOut << '\t';
		}
		numOut = linear(dem,row,nXSize-1).flowTotal.size();
		ftotal << numOut << '\n';
	}
	ftotal.close();
}

void writeStdOut(Metadata iniData)
{
	Cell *dem=NULL;
	ip::managed_shared_memory *segment=NULL;
	try{
		//Special shared memory where we can construct objects associated with a name.
		//Connect to the already created shared memory segment+initialize needed resources
		segment = new ip::managed_shared_memory(ip::open_only, "stream_finder_dem");  //segment name

		//Find the vector using the c-string name
		dem = segment->find<Cell>("DEM").first;
	}
	catch(ip::interprocess_exception& e){
		cout << "Error accessing the shared memory objects in writeStdOut: " << e.what() << "\n";
		throw;
	}

	cout << fixed;
	//write Simplified DEM
	for(int row=0; row<nYSize; row++)
	{
		for(int column=0; column<(nXSize-1); column++)
		{
			cout << linear(dem,row,column).height << '\t';
		}
		cout << linear(dem,row,nXSize-1).height << '\n';
	}
	cout << '\n';

	//write Metadata INI
	cout << "[Core]\npixel_size=" << iniData.physicalSize << "\nx_pixels="
			<< nXSize << "\ny_pixels=" << nYSize << "\n[Display]\norigin_x="
			<< iniData.originX << "\norigin_y=" << iniData.originY
			<< "\nprojection=" << iniData.projection << "\n";
	cout << '\n';
	
	//Write Flow Direction Grid
	for(int row=0; row<nYSize; row++)
	{
		for(int column=0; column<(nXSize-1); column++)
		{
			cout << (int)(linear(dem,row,column).flowDir) << '\t';
		}
		cout << (int)(linear(dem,row,nXSize-1).flowDir) << '\n';
	}
	cout << '\n';
	//write Flow Total Grid
	for(int row=0; row<nYSize; row++)
	{
		int numOut = 0;
		for(int column=0; column<(nXSize-1); column++)
		{
			numOut = linear(dem,row,column).flowTotal.size();
			cout << numOut << '\t';
		}
		numOut = linear(dem,row,nXSize-1).flowTotal.size();
		cout << numOut << '\n';
	}
	delete segment;
}

void flowTrace(int firstRow, int end)
{
	Cell *dem=NULL;
	ip::managed_shared_memory *segment=NULL;
	const unsigned short max_att = 10;
	unsigned short attempt = 0;
	ip::interprocess_exception* e = NULL;
	for(attempt = 0; attempt < max_att; attempt++)
	{
		try{
			segment = new ip::managed_shared_memory(ip::open_only, "stream_finder_dem");
			dem = segment->find<Cell>("DEM").first;
			break;
		}
		catch(ip::interprocess_exception& ex){
			cout << "Error accessing the shared memory objects in flowTrace for attempt "
				<< attempt << ":\n" << ex.what() << "\n";
			e = &ex;
			sleep(2);
		}
	}
	if(attempt == max_att)
	{
		cout << "Can't get shared memory in flowTrace.\n";
		throw *e;
	}
	for(int row = firstRow;row < end; row++)
	{
		for(int column = 1; column < (nXSize-1); column++)
		{
			if(verbose) cout << '~';
			try{
				follow(dem, row, column);
			}catch(BadFlowGrid e){
				cout << e.what() << '\n';
				exit(2);
			}catch(...){
				cout << "Could not follow.\n";
				exit(1);
			}
		}
	}
	delete segment;
}

void follow(Cell* dem, int row, int column)
{
	//If this cell was already part of another run's flow path,
	//we have nothing to do.
	cout << "follow.1\n";
	if(linear(dem,row,column).flowTotal.size()) return;
	cout << "follow.2\n";
	Point current(row, column);
	Point last(current);
	cout << "follow.3\n";
	for(flow(current, linear(dem, current).flowDir);
		//make sure we are still inside the DEM
		current.row>=0 && current.row<nYSize && current.column>=0 && current.column<nYSize;
		//go to the next cell as per current flow direction
		flow(current, linear(dem, current).flowDir) )
	{
		cout << "follow.4\n";
		//insert the last cell's flow records into this one
		linear(dem, current).flowTotal.insert(last);
		linear(dem, current).flowTotal.insert(linear(dem, last).flowTotal.begin(),linear(dem, last).flowTotal.end());
		cout << "follow.5\n";
		//if we flow into the same cell again we know the flow grid is corrupt
		if(linear(dem, current).flowTotal.find(Point(current.row, current.column)) != linear(dem, current).flowTotal.end())
			throw oneBFG;
		cout << "follow.6\n";
		if(verbose)
		{
			ostringstream oss;
			oss << "row=" << current.row << ", col=" << current.column << ", setsize=" << linear(dem, current).flowTotal.size() << '\n';
			cout << oss.str();
		}
		cout << "follow.7\n";
		last = current;
		cout << "follow.8\n";
	}
	cout << "follow.9\n";
}

