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
	cout << "test1\n";
	//set up shared memory
	Matrix *dem = NULL;
	float *pafScanline;
	ip::shared_memory_object::remove("stream_finder_dem");
	ip::managed_shared_memory *seg=NULL;
	ip::managed_shared_memory::handle_t linearHandle;
	try{
		size_t matrixBytes = (sizeof(Cell) * cellsX + sizeof(ip::vector<Cell, CellShmemAllocator>))
								* cellsY + sizeof(Matrix) + 100;
		size_t linearBytes = sizeof(float) * cellsX * cellsY + 100;
		ip::managed_shared_memory segment(ip::create_only,
							"stream_finder_dem",		//segment name
							matrixBytes+linearBytes);	//segment size in bytes
		seg = &segment;
							
		//Initialize shared memory STL-compatible allocator
		const VecShmemAllocator alloc_inst(segment.get_segment_manager());
		
		//Construct a shared memory
		dem = segment.construct<Matrix>("DEM")(alloc_inst);
		dem->reserve(cellsY);
		for(Matrix::iterator iter=dem->begin(); iter!=dem->end(); iter++) (*iter).reserve(cellsX);
		pafScanline = (float*)segment.allocate(linearBytes);
		linearHandle = segment.get_handle_from_address(pafScanline);
	}
	catch(...){
		ip::shared_memory_object::remove("stream_finder_dem");
		throw;
	}
	cout << "test2\n";
	pafScanline[12]=3.5;
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
	cout << "test2.5\n";
	//pafScanline = (float *) CPLMalloc(sizeof(float)*nXSize*nYSize);
	poBand->RasterIO( GF_Read, 0, 0, nXSize, nYSize, pafScanline, nXSize, nYSize, GDT_Float32, 0, 0 );
	GDALClose((GDALDatasetH*)poDataset);
	
	if(nXSize < 2 || nYSize < 2)
	{
		//CPLFree(pafScanline);
		cout << "Something is wrong with the input DEM. Aborting.\n";
		return 1;
	}
	
	//We have the data from the file, now we make the data 2-dimensional for easier reading.
	//The cells on the outside are made with default outward flow directions.
	cout << "test3\n";
	if(threads > nYSize) threads = nYSize;
	int rowsPerThread = nYSize / threads;

	boost::thread_group demFiller;
	for(int thread=0; thread<(threads-1); thread++)
	{
		int firstRow = thread*rowsPerThread;
		demFiller.add_thread(new boost::thread(linearTo2d, firstRow,
												firstRow+rowsPerThread, linearHandle));
	}
	demFiller.add_thread(new boost::thread(linearTo2d, (threads-1)*rowsPerThread,
											nYSize, linearHandle));
	
	//CPLFree(pafScanline);
	demFiller.join_all();	//wait until all the data is in place before doing calcs on it
	cout << "test4\n";
	//Done reading DEM, now do calculations.
	/* This loop structure is here in case we find any sinkholes, in which case
		we would have to redo all the calculations after adjusting the heights.
	*/
	bool sinkholes = false;
	do
	{
		sinkholes = false;
		//make flow direction grid
		boost::thread_group flowDirCalc;
		for(int thread=0; thread<(threads-1); thread++)
		{
			int firstRow = thread*rowsPerThread;
			flowDirCalc.add_thread(new boost::thread(flowDirection, firstRow, firstRow+rowsPerThread));
		}
		flowDirCalc.add_thread(new boost::thread(flowDirection, (threads-1)*rowsPerThread, nYSize));
		flowDirCalc.join_all();
		
		//make flow total grid, which will also reveal sinkholes.
		
	}while(sinkholes == true);
	
	//write output
	boost::thread_group writeout;
	if(fileOut)	writeFiles(dem, sDem, meta, flowDir, flowTotal, iniData);
	if(cmdOut)	writeout.add_thread(new boost::thread(writeStdOut, iniData));
	writeout.join_all();
	
	//Free shared memory
	seg->destroy<Matrix>("DEM");
	seg->deallocate((void*)pafScanline);
	ip::shared_memory_object::remove("stream_finder_dem");
	return 0;
}

void flowDirection(int row, int end)
{
	Matrix *dem=NULL;
	try{
		//Special shared memory where we can construct objects associated with a name.
		//Connect to the already created shared memory segment+initialize needed resources
		ip::managed_shared_memory segment(ip::open_only, "stream_finder_dem");  //segment name

		//Find the vector using the c-string name
		dem = segment.find<Matrix>("DEM").first;
   }
   catch(...){
      throw;
   }
	//We don't deal with cells on the outer boundary because their default
	//flow direction is outward and that is what we want.
	if(!row) row++;
	if(end == nYSize) end--;
	
	for(;row < end; row++)
	{
		for(int column = 1; column < (nXSize-1); column++)
		{
			direction celldir = none;
			for(int radius = 1; celldir == none; radius++)
			{
				celldir = greatestSlope(dem, row, column, radius);
			}
			(*dem)[row][column].flowDir = celldir;
		}
	}
}

direction greatestSlope(Matrix* dem, int row, int column, int radius)
{
	/*
	  When this function claims that no single direction has the greatest slope,
	  it is called again with a larger search radius. If the radius gets so
	  large that it goes past the edge of the DEM, we stop and use the direction
	  toward the nearest edge as this cell's flow direction.
	*/
	if(radius > row) return north;
	if(radius > column) return west;
	if(radius + row >= nYSize) return south;
	if(radius + column >= nXSize) return east;
	vector<float>* slopes = new vector<float>(8,0);
	(*slopes)[north]	= (*dem)[row+radius][column].height			- (*dem)[row-radius][column].height;
	(*slopes)[northeast]= (*dem)[row+radius][column-radius].height	- (*dem)[row-radius][column+radius].height;
	(*slopes)[east]		= (*dem)[row][column-radius].height			- (*dem)[row][column+radius].height;
	(*slopes)[southeast]= (*dem)[row-radius][column-radius].height	- (*dem)[row+radius][column+radius].height;
	(*slopes)[south]	= -(*slopes)[north];
	(*slopes)[southwest]= -(*slopes)[northeast];
	(*slopes)[west]		= -(*slopes)[east];
	(*slopes)[northwest]= -(*slopes)[southeast];
	unsigned short maxcount=1;
	int max=north;
	for(int dir=northeast; dir < none; dir++)
	{
		if((*slopes)[dir] > (*slopes)[max])
		{
			max = dir;
			maxcount = 1;
		}else if((*slopes)[dir] == (*slopes)[max]){
			maxcount++;
		}
	}
	delete slopes;
	if(maxcount > 1) return none;
	return intDirection(max);
}

void linearTo2d(int firstRow, int end, ip::managed_shared_memory::handle_t linearHandle)
{
	float* linearData;
	Matrix *dem=NULL;
	try{
		//Special shared memory where we can construct objects associated with a name.
		//Connect to the already created shared memory segment+initialize needed resources
		ip::managed_shared_memory segment(ip::open_only, "stream_finder_dem");  //segment name

		//Find the vector using the c-string name
		dem = segment.find<Matrix>("DEM").first;
		linearData = (float*)segment.get_address_from_handle(linearHandle);
   }
   catch(...){
      throw;
   }

	int yp = firstRow;
	if(!firstRow)
	{
		(*dem)[yp][0] = Cell(linearData[yp * nXSize], northwest, yp, 0);
		for(int xp = 1; xp<(nXSize-1); xp++)
		{
			(*dem)[yp][xp] = Cell(linearData[yp * nXSize + xp], north, yp, xp);
		}
		(*dem)[yp][nXSize-1] = Cell(linearData[yp * nXSize + (nXSize-1)], northeast, yp, nXSize-1);
		yp++;
	}
	//boost::xtime xt;
	//boost::xtime_get (&xt, boost::TIME_UTC);
	//xt.sec += 300;
	//boost::this_thread::sleep(xt); // debug sleep
	int lastNormRow = (end==nYSize) ? end-1 : end;
	for(; yp<lastNormRow; yp++)
	{
		(*dem)[yp][0] = Cell(linearData[yp * nXSize], west, yp, 0);
		for(int xp = 1; xp<(nXSize-1); xp++)
		{
			(*dem)[yp][xp] = Cell(linearData[yp * nXSize + xp], yp, xp);
		}
		(*dem)[yp][nXSize-1] = Cell(linearData[yp * nXSize + (nXSize-1)], east, yp, nXSize-1);
	}
	if(lastNormRow != end)
	{
		(*dem)[yp][0] = Cell(linearData[yp * nXSize], southwest, yp, 0);
		for(int xp = 1; xp<(nXSize-1); xp++)
		{
			(*dem)[yp][xp] = Cell(linearData[yp * nXSize + xp], south, yp, xp);
		}
		(*dem)[yp][nXSize-1] = Cell(linearData[yp * nXSize + (nXSize-1)], southeast, yp, nXSize-1);
	}
}

void writeFiles(Matrix* dem, fs::ofstream& sdem, fs::ofstream& meta, fs::ofstream& fdir, fs::ofstream& ftotal, Metadata& iniData)
{
	//write Simplified DEM
	for(int row=0; row<nYSize; row++)
	{
		for(int column=0; column<(nXSize-1); column++)
		{
			sdem << (*dem)[row][column].height << '\t';
		}
		sdem << (*dem)[row][nXSize-1].height << '\n';
	}
	sdem.close();

	//write Metadata INI
	meta << "[Core]\npixel_size=" << iniData.physicalSize << "\nx_pixels="
			<< nXSize << "\ny_pixels=" << nYSize << "\n[Display]\norigin_x="
			<< iniData.originX << "\norigin_y=" << iniData.originY
			<< "\nprojection=" << iniData.projection << "\n";
	meta.close();

	//Write Flow Direction Grid
	for(int row=0; row<nYSize; row++)
	{
		for(int column=0; column<(nXSize-1); column++)
		{
			fdir << (int)((*dem)[row][column].flowDir) << '\t';
		}
		fdir << (int)((*dem)[row][nXSize-1].flowDir) << '\n';
	}
	fdir.close();

	//write Flow Total Grid
	for(int row=0; row<nYSize; row++)
	{
		int numOut = 0;
		for(int column=0; column<(nXSize-1); column++)
		{
			numOut = (*dem)[row][column].flowTotal().size();
			ftotal << numOut << '\t';
		}
		numOut = (*dem)[row][nXSize-1].flowTotal().size();
		ftotal << numOut << '\n';
	}
	ftotal.close();
}

void writeStdOut(Metadata iniData)
{
	Matrix *dem=NULL;
	try{
		//Special shared memory where we can construct objects associated with a name.
		//Connect to the already created shared memory segment+initialize needed resources
		ip::managed_shared_memory segment(ip::open_only, "stream_finder_dem");  //segment name

		//Find the vector using the c-string name
		dem = segment.find<Matrix>("DEM").first;
	}
	catch(...){
		throw;
	}
   
	//write Simplified DEM
	for(int row=0; row<nYSize; row++)
	{
		for(int column=0; column<(nXSize-1); column++)
		{
			cout << (*dem)[row][column].height << '\t';
		}
		cout << (*dem)[row][nXSize-1].height << '\n';
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
			cout << (int)((*dem)[row][column].flowDir) << '\t';
		}
		cout << (int)((*dem)[row][nXSize-1].flowDir) << '\n';
	}
	cout << '\n';
	//write Flow Total Grid
	for(int row=0; row<nYSize; row++)
	{
		int numOut = 0;
		for(int column=0; column<(nXSize-1); column++)
		{
			numOut = (*dem)[row][column].flowTotal().size();
			cout << numOut << '\t';
		}
		numOut = (*dem)[row][nXSize-1].flowTotal().size();
		cout << numOut << '\n';
	}
}
