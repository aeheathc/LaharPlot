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
#include <gdal_priv.h>


using namespace std;

int main(int argc, char* argv[])
{
    GDALDataset  *poDataset;
    GDALAllRegister();

    poDataset = (GDALDataset *) GDALOpen( "D:/Download/1652948.DEM.SDTS/8345CATD.DDF", GA_ReadOnly );
    if( poDataset == NULL )
    {
        cout << "Problem opening!" << endl;
	}else{
		cout << "Open successful!" << endl;
	}
	
	    double        adfGeoTransform[6];

    printf( "Driver: %s/%s\n",
            poDataset->GetDriver()->GetDescription(), 
            poDataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );

    printf( "Size is %dx%dx%d\n", 
            poDataset->GetRasterXSize(), poDataset->GetRasterYSize(),
            poDataset->GetRasterCount() );

    if( poDataset->GetProjectionRef()  != NULL )
        printf( "Projection is `%s'\n", poDataset->GetProjectionRef() );

    if( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
    {
        printf( "Origin = (%.6f,%.6f)\n",
                adfGeoTransform[0], adfGeoTransform[3] );

        printf( "Pixel Size = (%.6f,%.6f)\n",
                adfGeoTransform[1], adfGeoTransform[5] );
	}

	        GDALRasterBand  *poBand;
        int             nBlockXSize, nBlockYSize;
        int             bGotMin, bGotMax;
        double          adfMinMax[2];
        
        poBand = poDataset->GetRasterBand( 1 );
        poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
        printf( "Block=%dx%d Type=%s, ColorInterp=%s\n",
                nBlockXSize, nBlockYSize,
                GDALGetDataTypeName(poBand->GetRasterDataType()),
                GDALGetColorInterpretationName(
                    poBand->GetColorInterpretation()) );

        adfMinMax[0] = poBand->GetMinimum( &bGotMin );
        adfMinMax[1] = poBand->GetMaximum( &bGotMax );
        if( ! (bGotMin && bGotMax) )
            GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);

        printf( "Min=%.3fd, Max=%.3f\n", adfMinMax[0], adfMinMax[1] );
        
        if( poBand->GetOverviewCount() > 0 )
            printf( "Band has %d overviews.\n", poBand->GetOverviewCount() );

        if( poBand->GetColorTable() != NULL )
            printf( "Band has a color table with %d entries.\n", 
                     poBand->GetColorTable()->GetColorEntryCount() );


		float *pafScanline;
        int   nXSize = poBand->GetXSize(), nYSize = poBand->GetYSize();
        pafScanline = (float *) CPLMalloc(sizeof(float)*nXSize*nYSize);
        poBand->RasterIO( GF_Read, 0, 0, nXSize, nYSize, pafScanline, nXSize, nYSize, GDT_Float32, 0, 0 );

		for(int yp = 0; yp<nYSize; yp++)
		{
			for(int xp = 0; xp<nXSize; xp++)
				cout << pafScanline[yp * nXSize + xp] << ",\t";
			cout << endl;
		}			  
					  
					  
					  
					  
	GDALClose((GDALDatasetH*)poDataset);
	return 0;
}
