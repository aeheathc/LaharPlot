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
	sDem = new fs::ofstream;
	meta = new fs::ofstream;
	flowDir = new fs::ofstream;
	flowTotal = new fs::ofstream;
	
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
			sDem->open(outfile+"-sdem.tsv");
			meta->open(outfile+".ini");
			flowDir->open(outfile+"-fdir.tsv");
			flowTotal->open(outfile+"-ftotal.tsv");
			if(!(*sDem && *meta && *flowDir && *flowTotal))
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
	Cell::cellsX = poDataset->GetRasterXSize(),
	Cell::cellsY = poDataset->GetRasterYSize();
	//int layers = poDataset->GetRasterCount();
			
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

	boost::thread_group writeout;	
	if(fileOut)	writeout.add_thread(new boost::thread(writeMeta, iniData));
	
	//set up globals for interthread data sharing
	dem = new Cell[Cell::cellsX * Cell::cellsY];
	pafScanline = new float[Cell::cellsX * Cell::cellsY];

	GDALRasterBand  *poBand;
	int             nBlockXSize, nBlockYSize;
	int             bGotMin, bGotMax;
	int				inXSize, inYSize;
	double          adfMinMax[2];
	string			demDT;
	poBand = poDataset->GetRasterBand(1);
	poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
	demDT = poBand->GetRasterDataType();
	adfMinMax[0] = poBand->GetMinimum( &bGotMin );
	adfMinMax[1] = poBand->GetMaximum( &bGotMax );
	if(!(bGotMin && bGotMax))
		GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
	inXSize = abs(poBand->GetXSize());
	inYSize = abs(poBand->GetYSize());
	poBand->RasterIO( GF_Read, 0, 0, inXSize, inYSize, pafScanline, inXSize, inYSize, GDT_Float32, 0, 0 );
	GDALClose((GDALDatasetH*)poDataset);
	
	if(Cell::cellsX < 2 || Cell::cellsY < 2 || inXSize != Cell::cellsX || inYSize != Cell::cellsY)
	{
		cout << "Something is wrong with the input DEM. Aborting.\n";
		delete[] dem;
		delete[] pafScanline;
		return 1;
	}
	
	linear(pafScanline,0,0,Cell::cellsX);	//initialize static variable inside linear()
	linear(dem,0,0,Cell::cellsX);
		
	//Fill in the sinkholes!
	if(verbose) cout << "Filling sinkholes...\n";
	FillSinks filler(pafScanline, Cell::cellsY, Cell::cellsX, 1);
	filler.fill();
	
	//We have the data from the file, now we make the data 2-dimensional for easier reading.
	//The cells on the outside are made with default outward flow directions.
	if(threads > Cell::cellsY) threads = Cell::cellsY;
	int rowsPerThread = Cell::cellsY / threads;
	if(verbose)
	{
		cout << "XSize=" << Cell::cellsX << ",YSize=" << Cell::cellsY << ",Cells="
			<< (Cell::cellsX*Cell::cellsY) << "\nBuilding DEM...\n";
	}
	boost::thread_group demFiller;
	
	for(int thread=0; thread<(threads-1); thread++)
	{
		int firstRow = thread*rowsPerThread;
		demFiller.add_thread(new boost::thread(linearTo2d, firstRow,
												firstRow+rowsPerThread));
	}
	demFiller.add_thread(new boost::thread(linearTo2d, (threads-1)*rowsPerThread,
											Cell::cellsY));
	
	demFiller.join_all();	//wait until all the data is in place before doing calcs on it
	delete[] pafScanline;
	//Done reading DEM...
	
	if(fileOut)	writeout.add_thread(new boost::thread(writeSdem));
	
	//Now do calculations.
	if(verbose) cout << "\nCalculating...\nFinding flow direction...\n";
	//make flow direction grid
	boost::thread_group flowDirCalc;
	for(int thread=0; thread<(threads-1); thread++)
	{
		int firstRow = thread*rowsPerThread;
		flowDirCalc.add_thread(new boost::thread(flowDirection, firstRow, firstRow+rowsPerThread));
	}
	flowDirCalc.add_thread(new boost::thread(flowDirection, (threads-1)*rowsPerThread, Cell::cellsY));
	flowDirCalc.join_all();
	if(fileOut)	writeout.add_thread(new boost::thread(writeFlowDir));

	//make flow total grid
	if(verbose) cout << "\nFinding flow totals...\n";
	boost::thread_group flowTotalCalc;
	const unsigned long edgeCells = (2*Cell::cellsX + 2*Cell::cellsY - 4);
	const unsigned long cellsPerThread = edgeCells / threads;
	edge(dem,0,Cell::cellsX,Cell::cellsY);	//initialize width and height in function
	for(int thread=0; thread<(threads-1); thread++)
	{
		unsigned long firstCell = thread * cellsPerThread;
		flowTotalCalc.add_thread(new boost::thread(flowTrace, firstCell, firstCell+cellsPerThread));
		if(verbose) cout << "Assigned thread.\n";
	}
	flowTotalCalc.add_thread(new boost::thread(flowTrace, (threads-1)*cellsPerThread, edgeCells));
	flowTotalCalc.join_all();

	if(verbose) cout << "Writing output...\n";
	
	//write output
	if(fileOut)	writeout.add_thread(new boost::thread(writeFlowTotal));
	if(cmdOut)	writeout.add_thread(new boost::thread(writeStdOut, iniData));
	writeout.join_all();
	
	//Free heap memory
	delete sDem;
	delete meta;
	delete flowDir;
	delete flowTotal;
	delete[] dem;
	
	//tell any stdout-captors that we are done
	cout << EOF;
	return 0;
}

void flowDirection(int row, int end)
{
	//We don't deal with cells on the outer boundary because their default
	//flow direction is outward and that is what we want.
	if(!row) row++;
	if(end == Cell::cellsY) end--;
	
	for(;row < end; row++)
	{
		for(int column = 1; column < (Cell::cellsX-1); column++)
		{
			if(verbose) cout << '.';
			linear(dem,row,column).flowDir = greatestSlope(row, column);
		}
	}
}

direction greatestSlope(int row, int column)
{
	vector<float> slopes(8,0);
	//method 1 (rudiger: compare cross slopes)
	/*slopes[north]		= linear(dem,row+1,column).height	- linear(dem,row-1,column).height;
	slopes[northeast]	= linear(dem,row+1,column-1).height	- linear(dem,row-1,column+1).height;
	slopes[east]		= linear(dem,row,column-1).height	- linear(dem,row,column+1).height;
	slopes[southeast]	= linear(dem,row-1,column-1).height	- linear(dem,row+1,column+1).height;
	slopes[south]		= -slopes[north];
	slopes[southwest]	= -slopes[northeast];
	slopes[west]		= -slopes[east];
	slopes[northwest]	= -slopes[southeast];*/
	
	//method 2 (compare radial slopes)
	/*slopes[north]		= linear(dem,row,column).height	- linear(dem,row-1,column).height;
	slopes[northeast]	= linear(dem,row,column).height	- linear(dem,row-1,column+1).height;
	slopes[east]		= linear(dem,row,column).height	- linear(dem,row,column+1).height;
	slopes[southeast]	= linear(dem,row,column).height	- linear(dem,row+1,column+1).height;
	slopes[south]		= linear(dem,row,column).height - linear(dem,row+1,column).height;
	slopes[southwest]	= linear(dem,row,column).height - linear(dem,row+1,column-1).height;
	slopes[west]		= linear(dem,row,column).height - linear(dem,row,column-1).height;
	slopes[northwest]	= linear(dem,row,column).height - linear(dem,row-1,column-1).height;*/
	
	//method 3 (null)
	/*slopes[north]		= 0;
	slopes[northeast]	= 0;
	slopes[east]		= 0;
	slopes[southeast]	= 0;
	slopes[south]		= 1;
	slopes[southwest]	= 0;
	slopes[west]		= 0;
	slopes[northwest]	= 0;*/
		
	//method 4 (use lowest elevation)
	slopes[north]		= -linear(dem,row-1,column	).height;
	slopes[northeast]	= -linear(dem,row-1,column+1).height;
	slopes[east]		= -linear(dem,row,	column+1).height;
	slopes[southeast]	= -linear(dem,row+1,column+1).height;
	slopes[south]		= -linear(dem,row+1,column	).height;
	slopes[southwest]	= -linear(dem,row+1,column-1).height;
	slopes[west]		= -linear(dem,row,	column-1).height;
	slopes[northwest]	= -linear(dem,row-1,column-1).height;	
	
	int max=north;
	for(int dir=northeast; dir < none; dir++)
	{
		if(slopes[dir] > slopes[max])
			max = dir;
	}
	return intDirection(max);
}

void linearTo2d(int firstRow, int end)
{
	int yp = firstRow;
	if(firstRow == 0)
	{
		linear(dem,yp,0).fill(linear(pafScanline, yp, 0), yp, 0, northwest);
		for(int xp = 1; xp<(Cell::cellsX-1); xp++)
		{
			linear(dem,yp,xp).fill(linear(pafScanline, yp, xp), yp, xp, north);
		}
		linear(dem,yp,Cell::cellsX-1).fill(linear(pafScanline, yp, Cell::cellsX-1), yp, Cell::cellsX-1, northeast);
		yp++;
	}
	int lastNormRow = (end==Cell::cellsY) ? end-1 : end;
	for(; yp<lastNormRow; yp++)
	{
		linear(dem,yp,0).fill(linear(pafScanline, yp, 0), yp, 0, west);
		for(int xp = 1; xp<(Cell::cellsX-1); xp++)
		{
			if(verbose) cout << '-';
			linear(dem,yp,xp).fill(linear(pafScanline, yp, xp), yp, xp);
		}
		linear(dem,yp,Cell::cellsX-1).fill(linear(pafScanline, yp, Cell::cellsX-1), yp, Cell::cellsX-1, east);
	}
	if(lastNormRow != end)
	{
		linear(dem,yp,0).fill(linear(pafScanline, yp, 0), yp, 0, southwest);
		for(int xp = 1; xp<(Cell::cellsX-1); xp++)
		{
			linear(dem,yp,xp).fill(linear(pafScanline, yp, xp), yp, xp, south);
		}
		linear(dem,yp,Cell::cellsX-1).fill(linear(pafScanline, yp, Cell::cellsX-1), yp, Cell::cellsX-1, southeast);
	}
}

void writeSdem()
{
	for(int row=0; row<Cell::cellsY; row++)
	{
		for(int column=0; column<(Cell::cellsX-1); column++)
		{
			*sDem << linear(dem,row,column).height << '\t';
		}
		*sDem << linear(dem,row,Cell::cellsX-1).height << '\n';
	}
	sDem->close();
}

void writeMeta(Metadata& iniData)
{
	*meta << fixed << setprecision(0) << "[Core]\npixel_size=" << iniData.physicalSize
			<< "\nx_pixels=" << Cell::cellsX << "\ny_pixels=" << Cell::cellsY
			<< "\n[Display]\norigin_x=" << iniData.originX << "\norigin_y="
			<< iniData.originY << "\nprojection=" << iniData.projection << "\n";
	meta->close();
}

void writeFlowDir()
{
	for(int row=0; row<Cell::cellsY; row++)
	{
		for(int column=0; column<(Cell::cellsX-1); column++)
		{
			*flowDir << (int)(linear(dem,row,column).flowDir) << '\t';
		}
		*flowDir << (int)(linear(dem,row,Cell::cellsX-1).flowDir) << '\n';
	}
	flowDir->close();
}

void writeFlowTotal()
{
	*flowTotal << fixed << setprecision(0);
	for(int row=0; row<Cell::cellsY; row++)
	{
		int numOut = 0;
		for(int column=0; column<(Cell::cellsX-1); column++)
		{
			numOut = linear(dem,row,column).flowTotal;
			*flowTotal << numOut << '\t';
		}
		numOut = linear(dem,row,Cell::cellsX-1).flowTotal;
		*flowTotal << numOut << '\n';
	}
	flowTotal->close();
}

void writeStdOut(Metadata iniData)
{
	cout << fixed;
	//write Simplified DEM
	for(int row=0; row<Cell::cellsY; row++)
	{
		for(int column=0; column<(Cell::cellsX-1); column++)
		{
			cout << linear(dem,row,column).height << '\t';
		}
		cout << linear(dem,row,Cell::cellsX-1).height << '\n';
	}
	cout << '\n';

	//write Metadata INI
	cout << "[Core]\npixel_size=" << iniData.physicalSize << "\nx_pixels="
			<< Cell::cellsX << "\ny_pixels=" << Cell::cellsY << "\n[Display]\norigin_x="
			<< iniData.originX << "\norigin_y=" << iniData.originY
			<< "\nprojection=" << iniData.projection << "\n";
	cout << '\n';
	
	//Write Flow Direction Grid
	for(int row=0; row<Cell::cellsY; row++)
	{
		for(int column=0; column<(Cell::cellsX-1); column++)
		{
			cout << (int)(linear(dem,row,column).flowDir) << '\t';
		}
		cout << (int)(linear(dem,row,Cell::cellsX-1).flowDir) << '\n';
	}
	cout << '\n';
	//write Flow Total Grid
	for(int row=0; row<Cell::cellsY; row++)
	{
		int numOut = 0;
		for(int column=0; column<(Cell::cellsX-1); column++)
		{
			numOut = linear(dem,row,column).flowTotal;
			cout << numOut << '\t';
		}
		numOut = linear(dem,row,Cell::cellsX-1).flowTotal;
		cout << numOut << '\n';
	}
}

void flowTrace(unsigned long start, unsigned long end)
{
	if(verbose)
	{
		ostringstream oss;
		oss << "Calling flowTrace from " << start << " to " << end << '\n';
		cout << oss.str();
	}
	
	for(unsigned long cell = start; cell < end; cell++)
	{
		if(verbose) cout << '#';
		edge(dem,cell).accumulate();
	}
	if(verbose) cout << '\n';
}

