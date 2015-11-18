#!/bin/sh -u

if [[ "x$GAUDI" == "x" ]]; then 
    echo "Need to set the GAUDI environment variable to the path of the Gaudi software directory (contains GaudiKernel/)."
    return 1
else 
    echo "Gaudi   :    $GAUDI"
fi
if [[ "x$FCCBASE" == "x" ]]; then 
    echo "Need to set the FCCBASE environment variable to root path of the software (contains both Gaudi and the FCC software)."
    return 1
else 
    echo "FCC root:    $FCCBASE"
fi

if [[ "x$FCCEDM" == "x" ]]; then
    echo "Need to set the FCCEDM environment variable to the path of the FCC EDM software."
    return 1
else
    echo "FCCEDM  :    $FCCEDM"
fi

if [[ "x$ALBERS" == "x" ]]; then
    echo "Need to set the ALBERS environment variable to the path of the ALBERS core library."
    return 1
else
    echo "ALBERS  :    $ALBERS"
fi

# set up CMake:
export PATH=/afs/cern.ch/sw/lcg/contrib/CMake/2.8.12.2/Linux-i386/bin:$PATH
#export CMAKEFLAGS='-DCMAKE_USE_CCACHE=ON'

export DELPHES_DIR=/afs/cern.ch/exp/fcc/sw/0.3/Delphes/3.2.0/

export CMAKE_PREFIX_PATH=$GAUDI/cmake:$FCCBASE:$FCCEDM:$ALBERS:/afs/cern.ch/sw/lcg/releases:$DELPHES_DIR:/afs/cern.ch/sw/lcg/external/geant4/10.0.p04/x86_64-slc6-gcc47-opt/

export CMTCONFIG=x86_64-slc6-gcc48-opt

# # set up the compilers
# export PATH=/afs/cern.ch/lhcb/software/releases/LBSCRIPTS/LBSCRIPTS_v8r0/InstallArea/scripts:$PATH
#export LCG_hostos=x86_64-slc6
#export LCG_external_area=/afs/cern.ch/sw/lcg/external
#export PATH=/afs/cern.ch/sw/lcg/contrib/ninja/1.4.0/x86_64-slc6:$PATH

# set up Pythia8 Index.xml
export PYTHIA8_XML=/afs/cern.ch/sw/lcg/releases/LCG_68/MCGenerators/pythia8/186/x86_64-slc6-gcc48-opt/xmldoc

# add Geant4 data files
source /afs/cern.ch/sw/lcg/external/geant4/10.1/setup_g4datasets.sh

# add DD4hep
export inithere=$PWD
cd /afs/cern.ch/exp/fcc/sw/0.3/DD4hep/v00-09/
source bin/thisdd4hep.sh
cd $inithere
source /afs/cern.ch/sw/lcg/contrib/gcc/4.8.1/x86_64-slc6/setup.sh
