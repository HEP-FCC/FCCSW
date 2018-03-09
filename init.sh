
#!/bin/sh -u
# set FCCSWBASEDIR to the directory containing this script
export FCCSWBASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
weekday=`date +%a`
source /cvmfs/fcc.cern.ch/testing/sw/views/stable/x86_64-slc6-gcc62-opt/setup.sh


#export EOS_MGM_URL="root://eosuser.cern.ch"
#source /afs/cern.ch/project/eos/installation/client/etc/setup.sh
