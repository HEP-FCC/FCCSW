#!/bin/sh -u

# Set up the environment to build and test FCCSW
# This script relies on CVMFS and the FCC Externals
#
# Usage:
#   source init.sh         # Uses default value for the FCC Externals 94.2.0
#   source init.sh 94.1.0  # Sets a the 94.1.0 version of the FCC Externals


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

# Version
versiontag="97_FCC_1.0.0"
if ! test "x$1" = "x" ; then
   versiontag="$1"
fi

# Guess OS
ostag="x86_64-slc6"
if test -f "/etc/redhat-release"; then
   six=`grep "release 7" /etc/redhat-release || echo ""`
   if test ! "x$six" = "x" ; then
      ostag="x86_64-centos7"
   fi
fi

# Compiler
gcctag="gcc8"

# Platform
platform="$ostag-$gcctag-opt"

# Setup script
setuppath="/cvmfs/fcc.cern.ch/sw/views/releases/externals/$versiontag/$platform/setup.sh"

weekday=`date +%a`
if test -f $setuppath ; then
   echo "Setting up FCC externals from $setuppath"
   source $setuppath
else
   echo "Setup script not found: $setuppath! "
   echo "Platforms available for this version:"
   ls -1 /cvmfs/fcc.cern.ch/sw/views/releases/externals/$versiontag
   echo "Versions available for this platform:"
   ls -1 /cvmfs/fcc.cern.ch/sw/views/releases/externals/*/$platform/setup.sh
fi

# TEMPORARY: fix to lcg releases
#export Gaudi_DIR=/cvmfs/sft.cern.ch/lcg/releases/LCG_96c_LS/Gaudi/v32r2/$platform/

