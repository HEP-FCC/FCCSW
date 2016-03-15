#!/bin/sh -u
if [ -z "$BUILDTYPE" ] || [ "$BUILDTYPE" == "Release" ]; then
    export BINARY_TAG=x86_64-slc6-gcc49-opt
    export BUILDTYPE="Release"
else
    export BINARY_TAG=x86_64-slc6-gcc49-dbg
    export BUILDTYPE="Debug"
fi

source /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/LBSCRIPTS_v8r5p7/InstallArea/scripts/LbLogin.sh --cmtconfig $BINARY_TAG
# The LbLogin sets VERBOSE to 1 which increases the compilation output. If you want details et this to 1 by hand.
export VERBOSE=

export FCCEDM=/afs/cern.ch/exp/fcc/sw/0.7/fcc-edm/0.3/$BINARY_TAG/
export PODIO=/afs/cern.ch/exp/fcc/sw/0.7/podio/0.3/$BINARY_TAG
export DELPHES_DIR=/afs/cern.ch/exp/fcc/sw/0.7/Delphes/3.3.2/$BINARY_TAG
export PYTHIA_DIR=/afs/cern.ch/sw/lcg/releases/LCG_80/MCGenerators/pythia8/212/$BINARY_TAG/

export CMAKE_PREFIX_PATH=$FCCEDM:$PODIO:$DELPHES_DIR:/afs/cern.ch/sw/lcg/releases/LCG_83:$CMAKE_PREFIX_PATH:$PYTHIA_DIR

# set up Pythia8 Index.xml
export PYTHIA8_XML=/afs/cern.ch/sw/lcg/releases/LCG_80/MCGenerators/pythia8/212/$BINARY_TAG/share/Pythia8/xmldoc

# add DD4hep
export inithere=$PWD
cd /afs/cern.ch/exp/fcc/sw/0.7/DD4hep/20152311/$BINARY_TAG
source bin/thisdd4hep.sh
cd $inithere


# add Geant4 data files
source /afs/cern.ch/sw/lcg/external/geant4/10.2/setup_g4datasets.sh
export CMTPROJECTPATH=/afs/cern.ch/exp/fcc/sw/0.7/
source /afs/cern.ch/sw/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.sh

# let ROOT 6 know about the location of headers
export ROOT_INCLUDE_PATH=$ROOT_INCLUDE_PATH:$PODIO/include/datamodel:$FCCEDM/include/datamodel
