~~ Written by Jason Anderson ~~
-------------------------------

~ Compiling in Windows ~
------------------------

To compile zone.cpp, you will need:
  - Boost.
  - Boost's compiled Program Options and Thread libraries.

Make changes to Makefile_win
  - Supply a path to Program Options and Thread libraries.
  - Supply a path to Boost's root directory.

Note, when compiling on windows using MinGW compiler, you will
need to have setup support for POSIX's pthreads, as it is a 
dependency for Boost's thread library.

Finally, in the directory with the makefile, simply type:

   make -f Makefile_win


~ Running zone_win32.exe ~
--------------------------

For a detailed set of notes/instructions, simply type 

   zone_win32.exe --help 

This will provide a detailed explanation of each option.

The default directory to search for the necessary SDEM, fdir, and 
ini files is the directory containing the executable.  To run it, 
the minimal options you need is a simple name for all files, a 
starting x and y location, and a volume.  It will look something like 
this:

  zone_win32.exe -n <name> -x 20 -y 20 -v 100000

Notes:  When running zone_win32.exe, you will need provided the 
cygwin1.dll in the same directory as the executable.  I am currently 
working on creating a binary independant of any third party sources, 
but for now, please make sure this requirement is satisfied.  Thank you.


