fcc-experiments-sw
==================

Common software for all FCC experiments.
This software is based on Gaudi.

Installation
------------

First install Gaudi: https://github.com/HEP-FCC/GaudiMC

Clone this repository:

    cd $FCC
    git clone git@github.com:HEP-FCC/fcc-experiments-sw.git 
    export FCCSW=$FCC/fcc-experiments-sw/FCCSW

Compile:

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
