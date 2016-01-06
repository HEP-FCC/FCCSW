#!/bin/sh -u
source /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/LBSCRIPTS_v8r4p3/InstallArea/scripts/LbLogin.sh --cmtconfig x86_64-slc6-gcc49-opt

export FCCEDM=/afs/cern.ch/exp/fcc/sw/0.5/fcc-edm/0.1/x86_64-slc6-gcc49-opt/
export ALBERS=/afs/cern.ch/exp/fcc/sw/0.5/albers-core/0.1/x86_64-slc6-gcc49-opt
export DELPHES_DIR=/afs/cern.ch/exp/fcc/sw/0.5/Delphes-3.3.1/x86_64-slc6-gcc49-opt
export PYTHIA_DIR=/afs/cern.ch/sw/lcg/releases/LCG_80/MCGenerators/pythia8/212/x86_64-slc6-gcc49-opt/

export CMAKE_PREFIX_PATH=$FCCEDM:$ALBERS:$DELPHES_DIR:$CMAKE_PREFIX_PATH:$PYTHIA_DIR

# set up Pythia8 Index.xml
export PYTHIA8_XML=/afs/cern.ch/sw/lcg/releases/LCG_80/MCGenerators/pythia8/212/x86_64-slc6-gcc49-opt/xmldoc

# add Geant4 data files
source /afs/cern.ch/sw/lcg/external/geant4/10.1/setup_g4datasets.sh

# add DD4hep
export inithere=$PWD
cd  /afs/cern.ch/exp/fcc/sw/0.5/DD4hep/20152311/x86_64-slc6-gcc49-opt
source bin/thisdd4hep.sh
cd $inithere

export CMTPROJECTPATH=/afs/cern.ch/exp/fcc/sw/0.5/
source /afs/cern.ch/sw/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.sh
