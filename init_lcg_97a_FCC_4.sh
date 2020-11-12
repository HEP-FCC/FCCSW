LCGPREFIX=/cvmfs/sft.cern.ch/lcg
export BINARY_TAG=x86_64-centos7-gcc8-opt
LCGPATH=$LCGPREFIX/views/LCG_97a_FCC_4/$BINARY_TAG
source $LCGPATH/bin/thisdd4hep_only.sh
source $LCGPATH/setup.sh


export Gaudi_DIR=$(dirname $(readlink -f "$(which gaudirun.py)"))/../
export CMAKE_PREFIX_PATH=$Gaudi_DIR:$CMAKE_PREFIX_PATH

export CMAKE_PREFIX_PATH=/cvmfs/sft.cern.ch/lcg/releases/k4FWCore/00-01-01-8d426/x86_64-centos7-gcc8-opt/:$CMAKE_PREFIX_PATH

#source init_fccdependencies_local.sh
