#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([LaharPlot], [1.0], [https://sourceforge.net/projects/laharplot/])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([zone/zone.cpp], [stream/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Set library paths provided by user.
AC_ARG_WITH(boost, [ AC_HELP_STRING([--with-boost=dir], [directory for boost. Should include both the boost header folder (named boost) and the boost lib folder.])], [boost_incl="${with_boost}" boost_lib="${with_boost}/lib"], [boost_incl="/usr/include" boost_lib="/usr/lib"])
AC_ARG_WITH(boost-incl, [ AC_HELP_STRING([--with-boost-incl=dir], [directory where boost header folder (named boost) is])], boost_incl="${with_boost_incl}")
AC_ARG_WITH(boost-lib, [ AC_HELP_STRING([--with-boost-lib=dir], [directory where boost static libraries are])], boost_lib="${with_boost_lib}")

AC_ARG_WITH(gdal, [ AC_HELP_STRING([--with-gdal=dir], [directory for gdal. Should have an include folder (named include) and a lib folder.])], [gdal_incl="${with_gdal}/include" gdal_lib="${with_gdal}/lib"], [gdal_incl="/usr/include" gdal_lib="/usr/lib"])
AC_ARG_WITH(gdal-incl, [ AC_HELP_STRING([--with-gdal-incl=dir], [directory where gdal headers are])], gdal_incl="${with_gdal_incl}")
AC_ARG_WITH(gdal-lib, [ AC_HELP_STRING([--with-gdal-lib=dir], [directory where gdal static library is])], gdal_lib="${with_gdal_lib}")

AC_ARG_WITH(linux, [ AC_HELP_STRING([--with-linux], [include the options that we wouldn't need on windows: -lcurl -lpng -ljpeg -lgif -pthread])], [linuxlibs="-lcurl -lpng -ljpeg -lgif" psflags="-pthread"], [linuxlibs="" psflags="-static"])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
AC_CHECK_LIB([gdal], [GDALClose])
AC_CHECK_LIB([boost_filesystem], [main])
AC_CHECK_LIB([boost_system], [main])
AC_CHECK_LIB([boost_thread], [main])
AC_CHECK_LIB([boost_program_options], [main])
AC_CHECK_LIB([boost_date_time], [main])
AC_CHECK_LIB([curl], [curl_version_info])
AC_CHECK_LIB([gif], [DGifOpenFileName])
AC_CHECK_LIB([jpeg], [jpeg_finish_compress])
AC_CHECK_LIB([png], [png_create_read_struct])

# Checks for header files.
AC_CHECK_HEADERS([gdal_priv.h])
AC_CHECK_HEADERS([boost/filesystem.hpp])
AC_CHECK_HEADERS([boost/filesystem/fstream.hpp])
AC_CHECK_HEADERS([boost/program_options.hpp]) 
AC_CHECK_HEADERS([boost/thread.hpp])
AC_CHECK_HEADERS([cstdlib])
AC_CHECK_HEADERS([cmath])
AC_CHECK_HEADERS([algorithm])
AC_CHECK_HEADERS([exception])
AC_CHECK_HEADERS([iomanip])
AC_CHECK_HEADERS([iostream])
AC_CHECK_HEADERS([list])
AC_CHECK_HEADERS([set])
AC_CHECK_HEADERS([sstream])
AC_CHECK_HEADERS([string])
AC_CHECK_HEADERS([vector])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_CHECK_TYPES([unsigned long long, long long])

# Checks for library functions.
AC_CHECK_FUNCS([pow sqrt])

AC_CONFIG_FILES([Makefile stream/Makefile zone/Makefile])
AC_SUBST([PATHS],["-I${boost_incl} -I${gdal_incl} -L${boost_lib} -L${gdal_lib}"])
AC_SUBST([LINUXLIBS],["${linuxlibs}"])
AC_SUBST([PSFLAGS],["${psflags}"])

AC_OUTPUT
