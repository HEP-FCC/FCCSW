
#!/bin/sh -u
# set FCCSWBASEDIR to the directory containing this script
export FCCSWBASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
weekday=`date +%a`

source /cvmfs/fcc.cern.ch/sw/views/releases/externals/93.0.0/x86_64-slc6-gcc62-opt/setup.sh
