#!/bin/sh -u
# set FCCSWBASEDIR to the directory containing this script
export FCCSWBASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
weekday=`date +%a`
source /cvmfs/fcc.cern.ch/testing/sw/views/${weekday}/x86_64-slc6-gcc62-opt/setup.sh
#source /cvmfs/fcc.cern.ch/sw/0.8.2/init_fcc_stack.sh $1

