#!/bin/sh -u
# set FCCSWBASEDIR to the directory containing this script
export FCCSWBASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# TEMPORARY and will be moved to init_fcc_stack.sh in release 0.9
# set up git distributed by lcg (slc6 / cc7 versions are too old for clang-format)
export PATH=/cvmfs/sft.cern.ch/lcg/contrib/git/bin:$PATH
export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/contrib/git/lib64:$LD_LIBRARY_PATH
# set up llvm including clang-format
source /cvmfs/sft.cern.ch/lcg/contrib/llvm/5.0.0/x86_64-slc6-gcc49-opt/setup.sh

# setup the FCC software stack
source /cvmfs/fcc.cern.ch/sw/0.8.1/init_fcc_stack.sh $1

# setup the pre-commit hook that checks with clang-format before committing
if [ ! -L $FCCSWBASEDIR/.git/hooks/pre-commit ]; then
  ln -s $FCCSWBASEDIR/FWCore/scripts/pre-commit.py $FCCSWBASEDIR/.git/hooks/pre-commit
fi

# TEMPORARY and will be moved to init_fcc_stack.sh in release 0.9
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:/cvmfs/fcc.cern.ch/sw/0.8.1/acts/0.4.0/x86_64-slc6-gcc62-opt/share/cmake/ACTS/

