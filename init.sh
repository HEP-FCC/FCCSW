#!/bin/sh -u
# set FCCSWBASEDIR to the directory containing this script

#TEMPORARY, pending installation
# Add the passed value only to path if it's not already in there. 
function add_to_path { 
    if [ -z "$1" ] || [[ "$1" == "/lib" ]]; then 
        return 
    fi 
    path_name=${1} 
    eval path_value=\$$path_name 
    path_prefix=${2} 
    case ":$path_value:" in 
      *":$path_prefix:"*) :;;        # already there 
      *) path_value=${path_prefix}:${path_value};; # or prepend path 
    esac 
    eval export ${path_name}=${path_value} 
} 

export FCCSWBASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source /cvmfs/sft.cern.ch/lcg/views/LCG_92/x86_64-slc6-gcc62-opt/setup.sh
add_to_path CMAKE_PREFIX_PATH /afs/cern.ch/work/v/vavolkl/public/fcc.cern.ch/sw/0.9.0/podio/0.8/x86_64-slc6-gcc62-opt
export PODIO=/afs/cern.ch/work/v/vavolkl/public/fcc.cern.ch/sw/0.9.0/podio/0.8/x86_64-slc6-gcc62-opt
add_to_path CMAKE_PREFIX_PATH /afs/cern.ch/work/v/vavolkl/public/fcc.cern.ch/sw/0.9.0/fcc-edm/0.5.1/x86_64-slc6-gcc62-opt/
add_to_path CMAKE_PREFIX_PATH /afs/cern.ch/work/v/vavolkl/public/fcc.cern.ch/sw/0.9.0/dag/v0.1/x86_64-slc6-gcc62-opt/
add_to_path CMAKE_PREFIX_PATH /afs/cern.ch/work/v/vavolkl/public/fcc.cern.ch/sw/0.9.0/papas/1.2.0/x86_64-slc6-gcc62-opt/
add_to_path CMAKE_PREFIX_PATH /afs/cern.ch/work/v/vavolkl/public/fcc.cern.ch/sw/0.9.0/acts/0.5.3/x86_64-slc6-gcc62-opt
export LCG_releases_base=/cvmfs/sft.cern.ch/lcg/releases
export BINARY_TAG=x86_64-slc6-gcc62-opt
source /cvmfs/sft.cern.ch/lcg/views/LCG_92/x86_64-slc6-gcc62-opt/bin/thisdd4hep_only.sh
add_to_path CMAKE_PREFIX_PATH /cvmfs/sft.cern.ch/lcg/releases/LCG_92/HepMC/2.06.09/x86_64-slc6-gcc62-opt
add_to_path CMAKE_PREFIX_PATH /cvmfs/lhcb.cern.ch/lib/lhcb/GAUDI/GAUDI_v29r2/InstallArea/x86_64-slc6-gcc62-opt/
add_to_path CMAKE_PREFIX_PATH /afs/cern.ch/work/v/vavolkl/public/fcc.cern.ch/sw/0.9.0/delphes/install/

#source /cvmfs/fcc.cern.ch/sw/0.8.2/init_fcc_stack.sh $1

#export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:/cvmfs/fcc.cern.ch/sw/0.8.2/acts/0.4.0/x86_64-slc6-gcc62-opt/share/cmake/ACTS/

