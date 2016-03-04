#!/bin/sh -u
source /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/LBSCRIPTS_v8r4p3/InstallArea/scripts/LbLogin.sh --cmtconfig x86_64-slc6-gcc49-opt

export FCCEDM=/afs/cern.ch/exp/fcc/sw/0.6/fcc-edm/0.2/x86_64-slc6-gcc49-opt/
export PODIO=/afs/cern.ch/exp/fcc/sw/0.6/podio/0.1/x86_64-slc6-gcc49-opt
export DELPHES_DIR=/afs/cern.ch/exp/fcc/sw/0.6/Delphes-3.3.1/x86_64-slc6-gcc49-opt
export PYTHIA_DIR=/afs/cern.ch/sw/lcg/releases/LCG_80/MCGenerators/pythia8/212/x86_64-slc6-gcc49-opt/

export CMAKE_PREFIX_PATH=$FCCEDM:$PODIO:$DELPHES_DIR:$CMAKE_PREFIX_PATH:$PYTHIA_DIR

# set up Pythia8 Index.xml
export PYTHIA8_XML=/afs/cern.ch/sw/lcg/releases/LCG_80/MCGenerators/pythia8/212/x86_64-slc6-gcc49-opt/share/Pythia8/xmldoc

# add Geant4 data files
source /afs/cern.ch/sw/lcg/external/geant4/10.1/setup_g4datasets.sh

# add DD4hep
export inithere=$PWD
cd  /afs/cern.ch/exp/fcc/sw/0.6/DD4hep/20152311/x86_64-slc6-gcc49-opt
source bin/thisdd4hep.sh
cd $inithere

export CMTPROJECTPATH=/afs/cern.ch/exp/fcc/sw/0.6/
source /afs/cern.ch/sw/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.sh

# let ROOT 6 know about the location of headers
export ROOT_INCLUDE_PATH=$ROOT_INCLUDE_PATH:$PODIO/include/datamodel:$FCCEDM/include/datamodel
