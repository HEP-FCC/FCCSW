
#!/bin/sh -u
# set FCCSWBASEDIR to the directory containing this script
export FCCSWBASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# setup the pre-commit hook that checks with clang-format before committing
if [ ! -L $FCCSWBASEDIR/.git/hooks/pre-commit ]; then
  ln -s $FCCSWBASEDIR/FWCore/scripts/pre-commit.py $FCCSWBASEDIR/.git/hooks/pre-commit
fi

weekday=`date +%a`

source /cvmfs/fcc.cern.ch/sw/views/releases/externals/93.0.0/x86_64-slc6-gcc62-opt/setup.sh
