FCCSW
=====

Common software for all FCC experiments.
This software is based on Gaudi.

Installation
------------

Prepare a global software zone for FCC:

    mkdir FCC
    cd FCC
    export FCCBASE=$PWD

Clone the Gaudi repository: 

    git clone -b dev/hive  http://cern.ch/gaudi/GaudiMC.git GAUDI/GAUDI_v25r2
    export GAUDI=$FCCBASE/GAUDI/GAUDI_v25r2
    
Clone the FCCSW repository:

    git clone git@github.com:HEP-FCC/FCCSW.git FCCSW 
    export FCCSW=$FCCBASE/FCCSW

Set up your environment:

    cd $FCCSW
    source init.sh

Compile Gaudi:

    cd $GAUDI
    make -j 12 
    make install

Compile the FCC software:

    cd $FCCSW
    make -j 12


Test
----

    cp ~hegner/public/example_MyPythia.dat .
    ./run gaudirun.py example_options.py

You should see an HepMC printout. 

TODO 
----

* improve examples: add more, commit pythia input file, document them.
* move up what is in FCCSW
* move away what is in FCCSW_hh_dummy
* create a DataObjects package
