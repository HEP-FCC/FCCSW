#!/bin/sh -u
# set FCCSWBASEDIR to the directory containing this script
export FCCSWBASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source /cvmfs/fcc.cern.ch/sw/0.8/init_fcc_stack.sh $1


#export EOS_MGM_URL="root://eosuser.cern.ch"
#source /afs/cern.ch/project/eos/installation/client/etc/setup.sh
