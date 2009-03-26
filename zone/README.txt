To compile zone.exe, you will need to edit the makefiles to link
to your local boost libraries.  Sample text files are given for
the flow direction grid and SDEM, though they are quite small.  
To run zone.exe, you can use the program options to dermine 
what is required, or just use this to get it to run:

-e -n test -x 3 -y 9 -v 10000

