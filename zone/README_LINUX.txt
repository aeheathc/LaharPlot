~~ Written by Jason Anderson ~~
-------------------------------

~ Compiling in Linux ~
----------------------

To compile zone.cpp, you will need:
  - Boost.
  - Boost's compiled Program Options and Thread libraries.

Make changes to Makefile_win
  - Supply a path to Program Options and Thread libraries.
  - Supply a path to Boost's library directory.
  - Supply a path to Boost's root directory.

Finally, in the directory with the makefile, simply type:

   make

~ Running zone_*.exe ~
----------------------

For a detailed set of notes/instructions, simply type 

   ./zone_linux.exe --help 

This will provide a detailed explanation of each option.

The default directory to search for the necessary SDEM, fdir, and 
ini files is the directory containing the executable.  To run it, 
the minimal options you need is a simple name for all files, a 
starting x and y location, and a volume.  It will look something like 
this:

  -n test -x 20 -y 20 -v 100000

