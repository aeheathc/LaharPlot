To compile zone.exe, you will need to edit the makefiles to link
to your local boost libraries.  You will also need to supply the 
binary with the necessary SDEM and grid files for it to run.  They
can simply be placed in the directory with the executable.  Simply
run it with the following objects to get it to run.  Insert the 
common name for all files for the <name> tag.

-n <name> -x 3 -y 9 -v 10000

