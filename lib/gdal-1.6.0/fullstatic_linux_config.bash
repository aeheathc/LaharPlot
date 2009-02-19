#!/bin/bash
./configure  --prefix=/major/aeheathc/lib/gdal \
			--enable-static \
            --with-threads \
            --with-ogr \
            --with-geos \
            --without-libtool \
            --without-ld-shared \
            --with-libz=internal \
            --with-libtiff=internal \
            --with-geotiff=internal \
            --without-pg \
            --without-grass \
            --without-libgrass \
            --without-cfitsio \
            --without-pcraster \
            --without-netcdf \
            --without-ogdi \
            --without-fme \
            --without-hdf4 \
            --without-hdf5 \
            --without-jasper \
            --without-ecw \
            --without-kakadu \
            --without-mrsid \
            --without-jp2mrsid \
            --without-bsb \
            --without-grib \
            --without-dwgdirect \
            --without-panorama \
            --without-idb \
            --without-sde \
            --without-perl \
            --without-php \
            --without-ruby \
            --without-python \
            --without-ogpython \
            --with-hide-internal-symbols
