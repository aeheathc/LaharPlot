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

#include <iostream>
#include <sstream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/program_options.hpp> 
#include <boost/thread.hpp>

#include <gdal_priv.h>

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using namespace std;

int main(int argc, char* argv[])
{
	// Declare the supported options.
	po::options_description desc("Usage: stream [OPTION]...");
	desc.add_options()
		("help", "Display this help and exit")
		("input-file,f", po::value<string>(), "Read topography from input file <arg>")
		("std-in,i", "Read topography from standard-in. Can't be used with input-file.")
		("output-file,o", po::value<string>(), "Output to files using the base name <arg>.")
		("std-out,t", "Output to standard-out. May be used with output-file.")
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
	string optError="";
	string infile="",outfile="";
	bool cmdIn=vm.count("std-in");
	bool cmdOut=vm.count("std-out");
	bool verbose=vm.count("verbose");
	fs::ofstream sDem,meta,flowDir,flowTotal;
	
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
		cout << "stream: " << optError << "\nTry 'stream --help' for more information.\n";
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

	double	adfGeoTransform[6], origin_x = -1, origin_y = -1, physicalSize = -1;
	int		cellsX = poDataset->GetRasterXSize(),
			cellsY = poDataset->GetRasterYSize(),
			layers = poDataset->GetRasterCount(),
			zone = -1;

    if(poDataset->GetProjectionRef() != NULL)
	{
		string projection = poDataset->GetProjectionRef();
		string::size_type zoneStart = projection.find("UTM Zone ");
		if(zoneStart != string::npos)
		{
			istringstream projZone(projection.substr(zoneStart+9,projection.
				find(",",zoneStart+9)-(zoneStart+10)));
			projZone >> zone;
		}
	}

    if(poDataset->GetGeoTransform( adfGeoTransform ) == CE_None)
    {
		origin_x = adfGeoTransform[0];
		origin_y = adfGeoTransform[3];
		physicalSize = adfGeoTransform[1];
		//above size is X-size. Y-size is in adfGeoTransform[5]
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
	float *pafScanline;
	int   nXSize = poBand->GetXSize(), nYSize = poBand->GetYSize();
	pafScanline = (float *) CPLMalloc(sizeof(float)*nXSize*nYSize);
	poBand->RasterIO( GF_Read, 0, 0, nXSize, nYSize, pafScanline, nXSize, nYSize, GDT_Float32, 0, 0 );
	GDALClose((GDALDatasetH*)poDataset);

	/*for(int yp = 0; yp<nYSize; yp++)
	{
		for(int xp = 0; xp<nXSize; xp++)
			cout << pafScanline[yp * nXSize + xp] << ",\t";
		cout << endl;
	}*/			  
					  
	//Done reading DEM, now make a flow direction grid and fill sinkholes if necessary.
					  
					  

	return 0;
}
