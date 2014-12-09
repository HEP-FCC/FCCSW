#!/usr/bin/csh

if ! $?GAUDI then
    echo "Need to set GAUDI environmental variable: setenv GAUDI $FCCBASE/GAUDI/GAUDI_v25r2"
    exit 1
else 
    echo "Gaudi   :    $GAUDI"
endif

if ! $?FCCBASE then
    echo "Need to set the FCCBASE environment variable to root path of the software (contains both Gaudi and the FCC software)."
    exit 1
else 
    echo "FCC root:    $FCCBASE"
endif

# set up CMake:
setenv PATH /afs/cern.ch/sw/lcg/contrib/CMake/2.8.12.2/Linux-i386/bin:${PATH}
setenv CMAKE_PREFIX_PATH ${GAUDI}/cmake:${FCCBASE}:/afs/cern.ch/sw/lcg/releases
setenv CMTCONFIG x86_64-slc6-gcc48-opt

# set up the compilers
setenv PATH /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/LBSCRIPTS_v8r0/InstallArea/scripts:${PATH}

