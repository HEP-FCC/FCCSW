
#!/bin/sh -u
# set FCCSWBASEDIR to the directory containing this script
export FCCSWBASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source /cvmfs/fcc.cern.ch/sw/views/releases/0.9.0/x86_64-slc6-gcc62-opt/setup.sh
export CMAKE_PREFIX_PATH=/afs/cern.ch/work/v/vavolkl/public/fcc.cern.ch/sw/0.9.0/tricktrack/install:$CMAKE_PREFIX_PATH
export CMAKE_PREFIX_PATH=/afs/cern.ch/work/v/vavolkl/public/fcc.cern.ch/sw/0.9.0/fcc-edm/0.5.1/x86_64-slc6-gcc62-opt:$CMAKE_PREFIX_PATH


