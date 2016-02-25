FCCSW
=====

Common software for all FCC experiments.
This software is based on Gaudi.


Installation
------------

Log in to lxplus SLC6. Please note that these instructions might not work on another SLC6 machine. 

Set up your environment, as everytime you want to build or use this software: 

    source ./init.sh

Compile the FCC software:

    make -j 12

Test
----

    cp ~hegner/public/example_MyPythia.dat .
    ./run gaudirun.py Examples/options/example_options.py

You should see an HepMC printout.

Documentation
----

Please consult [FCCSW documentation](http://fccsw.web.cern.ch/fccsw/)
