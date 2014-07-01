FCCSW
=====

Common software for all FCC experiments.
This software is based on Gaudi.


Working with GitHub
-------------------

Our software is organized in the following way on github. 

* The FCC github organization is responsible for software integration. It owns the "main" copies of the FCC software repositories:
** FCCSW : core software shared by hh, ee, and he
** FCCSW-hh : software specific to the hh configuration
** FCCSW-ee : to be created
** FCCSW-eh : to be created

* Individual github users do not directly push their code to the FCC repositories. Instead, they fork their own copy of the FCC repositories. Then, they: 
** develop and commit on their local machine
** push their code to their own copy of the FCC repositories on github
** do pull requests to the FCC organization. The FCC organization administrators are then free to pull the code from the user repositories -- or not :-).


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

Compile Gaudi (it will take a bit of time, but you won't do that often):

    cd $GAUDI
    make -j 12 
    make install  # _do not forget to install_

Compile the FCC software:

    cd $FCCSW
    make -j 12


Test
----

    cp ~hegner/public/example_MyPythia.dat .
    ./run gaudirun.py example_options.py

You should see an HepMC printout. 

