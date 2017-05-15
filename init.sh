#!/bin/sh -u
# set FCCSWBASEDIR to the directory containing this script
export FCCSWBASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source /cvmfs/fcc.cern.ch/sw/0.8.1/init_fcc_stack.sh $1

# TODO: temporary, until new ACTS tag available & tests work with gcc6.2
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:/cvmfs/fcc.cern.ch/sw/0.8pre/acts/0.3.0/x86_64-slc6-gcc49-opt/share/cmake/ACTS/

