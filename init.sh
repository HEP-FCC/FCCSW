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

# set up CMake:
export PATH=/afs/cern.ch/sw/lcg/contrib/CMake/2.8.12.2/Linux-i386/bin:$PATH
#export CMAKEFLAGS='-DCMAKE_USE_CCACHE=ON'
export CMAKE_PREFIX_PATH=$GAUDI/cmake:$FCCBASE:/afs/cern.ch/sw/lcg/releases
export CMTCONFIG=x86_64-slc6-gcc48-opt

# set up the compilers
export PATH=/afs/cern.ch/lhcb/software/releases/LBSCRIPTS/LBSCRIPTS_v8r0/InstallArea/scripts:$PATH
#export LCG_hostos=x86_64-slc6
#export LCG_external_area=/afs/cern.ch/sw/lcg/external
#export PATH=/afs/cern.ch/sw/lcg/contrib/ninja/1.4.0/x86_64-slc6:$PATH

# set up Pythia8 Index.xml
export PYTHIA8_XML=/afs/cern.ch/sw/lcg/releases/LCG_68/MCGenerators/pythia8/186/x86_64-slc6-gcc48-opt/xmldoc
