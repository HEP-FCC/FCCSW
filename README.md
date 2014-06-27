fcc-experiments-sw
==================

Common software for all FCC experiments.
This software is based on Gaudi.

Installation
------------

Prepare a global software zone for FCC:

    mkdir FCC
    cd FCC
    export FCC=$PWD

Clone the Gaudi repository: 

    git clone -b dev/hive  http://cern.ch/gaudi/GaudiMC.git GAUDI/GAUDI_v25r2
    cd GAUDI/GAUDI_v25r2
    export GAUDI=$PWD
    
Set up your environment: **The FCC and GAUDI environment variables have to be set as above.**

    # set up CMake:
    export PATH=/afs/cern.ch/sw/lcg/contrib/CMake/2.8.12.2/Linux-i386/bin:$PATH
    export CMAKEFLAGS='-DCMAKE_USE_CCACHE=ON'
    export CMAKE_PREFIX_PATH=$GAUDI/cmake:$FCC:/afs/cern.ch/sw/lcg/releases
    export CMTCONFIG=x86_64-slc6-gcc48-opt


    # set up the compilers
    export PATH=/afs/cern.ch/lhcb/software/releases/LBSCRIPTS/LBSCRIPTS_v8r0/InstallArea/scripts:$PATH
    export LCG_hostos=x86_64-slc6
    export LCG_external_area=/afs/cern.ch/sw/lcg/external
    export PATH=/afs/cern.ch/sw/lcg/contrib/ninja/1.4.0/x86_64-slc6:$PATH

Compile Gaudi:

    cd $GAUDI
    make -j 12 
    make install

Clone this repository:

    cd $FCC
    git clone git@github.com:HEP-FCC/fcc-experiments-sw.git 
    export FCCSW=$FCC/fcc-experiments-sw/FCCSW

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
* create a Jets package
