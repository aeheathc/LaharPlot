To compile zone.exe, you will need to edit the makefiles to link
to your local boost libraries.  Sample text files are given for
the flow direction grid and SDEM.  To run zone.exe, you can use
the program options to dermine what is required, or just use this
to get it to run:

-e -n test -x 0 -y 10 -v 10000

