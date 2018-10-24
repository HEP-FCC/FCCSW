
#!/bin/sh -u
# set FCCSWBASEDIR to the directory containing this script
export FCCSWBASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# setup the pre-commit hook that checks with clang-format before committing
# slc6 git version is too old for use with clang-format
export PATH=/cvmfs/sft.cern.ch/lcg/contrib/git/bin:$PATH
export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/contrib/git/lib64:$LD_LIBRARY_PATH
if [ ! -L $FCCSWBASEDIR/.git/hooks/pre-commit ]; then
  mkdir -p $FCCSWBASEDIR/.git/hooks
  ln -s $FCCSWBASEDIR/git-hooks/pre-commit.py $FCCSWBASEDIR/.git/hooks/pre-commit
fi

weekday=`date +%a`

source /cvmfs/fcc.cern.ch/sw/views/releases/externals/93.0.0/x86_64-slc6-gcc62-opt/setup.sh
