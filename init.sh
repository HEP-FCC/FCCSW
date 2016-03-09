#!/bin/sh -u
source /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/LBSCRIPTS_v8r5p7/InstallArea/scripts/LbLogin.sh --cmtconfig x86_64-slc6-gcc49-opt
# The LbLogin sets VERBOSE to 1 which increases the compilation output. If you want details et this to 1 by hand.
export VERBOSE=

# source /afs/cern.ch/sw/lcg/views/LCG_83/x86_64-slc6-gcc49-opt/setup.sh

export FCCEDM=/afs/cern.ch/exp/fcc/sw/0.7/fcc-edm/0.3/x86_64-slc6-gcc49-opt/
export PODIO=/afs/cern.ch/exp/fcc/sw/0.7/podio/0.3/x86_64-slc6-gcc49-opt
export DELPHES_DIR=/afs/cern.ch/exp/fcc/sw/0.7/Delphes/3.3.2/x86_64-slc6-gcc49-opt
export PYTHIA_DIR=/afs/cern.ch/sw/lcg/releases/LCG_80/MCGenerators/pythia8/212/x86_64-slc6-gcc49-opt/

export CMAKE_PREFIX_PATH=$FCCEDM:$PODIO:$DELPHES_DIR:/afs/cern.ch/sw/lcg/releases/LCG_83:$CMAKE_PREFIX_PATH:$PYTHIA_DIR

# set up Pythia8 Index.xml
export PYTHIA8_XML=/afs/cern.ch/sw/lcg/releases/LCG_80/MCGenerators/pythia8/212/x86_64-slc6-gcc49-opt/share/Pythia8/xmldoc



# add DD4hep
export inithere=$PWD
cd /afs/cern.ch/exp/fcc/sw/0.7/DD4hep/20152311/x86_64-slc6-gcc49-opt
source bin/thisdd4hep.sh
cd $inithere


# add Geant4 data files
source /afs/cern.ch/sw/lcg/external/geant4/10.2/setup_g4datasets.sh
export CMTPROJECTPATH=/afs/cern.ch/exp/fcc/sw/0.7/
source /afs/cern.ch/sw/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.sh

# let ROOT know about the location of headers
export ROOT_INCLUDE_PATH=$ROOT_INCLUDE_PATH:$FCCEDM/include/datamodel