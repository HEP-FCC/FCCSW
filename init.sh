#!/bin/sh -u
# set FCCSWBASEDIR to the directory containing this script
export FCCSWBASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
#source /cvmfs/fcc.cern.ch/sw/0.8.1/init_fcc_stack.sh $1
weekday=`date +%a`
source /cvmfs/fcc.cern.ch/testing/sw/views/${weekday}/x86_64-slc6-gcc62-opt/setup.sh

#export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:/cvmfs/fcc.cern.ch/sw/0.8.1/acts/0.4.0/x86_64-slc6-gcc62-opt/share/cmake/ACTS/
