#!/usr/bin/csh
source /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/LBSCRIPTS_v8r4p3/InstallArea/scripts/LbLogin.csh --cmtconfig x86_64-slc6-gcc49-opt

setenv FCCEDM /afs/cern.ch/exp/fcc/sw/0.6/fcc-edm/0.1/x86_64-slc6-gcc49-opt/
setenv PODIO /afs/cern.ch/exp/fcc/sw/0.6/podio/0.1/x86_64-slc6-gcc49-opt
setenv DELPHES_DIR /afs/cern.ch/exp/fcc/sw/0.6/Delphes-3.3.1/x86_64-slc6-gcc49-opt
setenv PYTHIA_DIR /afs/cern.ch/sw/lcg/releases/LCG_80/MCGenerators/pythia8/212/x86_64-slc6-gcc49-opt/

setenv CMAKE_PREFIX_PATH ${FCCEDM}:${PODIO}:${DELPHES_DIR}:${CMAKE_PREFIX_PATH}:${PYTHIA_DIR}

# set up Pythia8 Index.xml
setenv PYTHIA8_XML /afs/cern.ch/sw/lcg/releases/LCG_80/MCGenerators/pythia8/212/x86_64-slc6-gcc49-opt/share/Pythia8/xmldoc

# add Geant4 data files
source /afs/cern.ch/sw/lcg/external/geant4/10.1/setup_g4datasets.csh

# add DD4hep
setenv inithere ${PWD}
cd /afs/cern.ch/exp/fcc/sw/0.6/DD4hep/20152311/x86_64-slc6-gcc49-opt
source bin/thisdd4hep.csh
cd $inithere

setenv CMTPROJECTPATH /afs/cern.ch/exp/fcc/sw/0.6/
source /afs/cern.ch/sw/lcg/contrib/gcc/4.9.3/x86_64-slc6/setup.csh
