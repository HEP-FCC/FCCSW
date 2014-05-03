localdir=$PWD
location=/afs/cern.ch/user/h/hegner/work/FCC

export CMTCONFIG=x86_64-slc6-gcc48-opt
source /afs/cern.ch/sw/lcg/external/gcc/4.8/$CMTCONFIG/setup.sh /afs/cern.ch/sw/lcg/external
export PATH=/afs/cern.ch/sw/lcg/external/CMake/2.8.6/x86_64-slc6-gcc46-opt/bin:${PATH}

ROOTlibs=${location}/ROOT/v5-34-00-patches/x86_64-slc6-gcc48-opt/lib
XERCESClibs=${location}/XercesC/3.1.1p1/x86_64-slc6-gcc48-opt/lib
DD4hepLibs=${location}/DD4hep/v00-06/x86_64-slc6-gcc48-opt/lib/
CLHEPlibs=${location}/clhep/2.1.4.1/x86_64-slc6-gcc48-opt/lib
Geant4libs=${location}/Geant4/9.6.p03/x86_64-slc6-gcc48-opt/lib64

export LD_LIBRARY_PATH=${Geant4libs}:${CLHEPlibs}:${DD4hepLibs}:${ROOTlibs}:${XERCESClibs}:${LD_LIBRARY_PATH}

cd ${ROOTlibs}/..
. bin/thisroot.sh

cd ${localdir}
export PATH=/afs/cern.ch/user/h/hegner/work/FCC/DD4hep/v00-06/x86_64-slc6-gcc48-opt/bin:${PATH}
