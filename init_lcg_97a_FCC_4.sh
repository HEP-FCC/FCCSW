LCGPREFIX=/cvmfs/sft.cern.ch/lcg
export BINARY_TAG=x86_64-centos7-gcc8-opt
LCGPATH=$LCGPREFIX/views/LCG_97a_FCC_4/$BINARY_TAG
source $LCGPATH/bin/thisdd4hep_only.sh
source $LCGPATH/setup.sh

export PATH=/cvmfs/sft.cern.ch/lcg/releases/CMake/3.17.3-75516/x86_64-centos7-gcc8-opt/bin:$PATH
export PATH=/cvmfs/sft.cern.ch/lcg/releases/ninja/1.10.0-3d8f6/x86_64-centos7-gcc8-opt/bin:$PATH

export Gaudi_DIR=$(dirname $(readlink -f "$(which gaudirun.py)"))/../
export CMAKE_PREFIX_PATH=$Gaudi_DIR:$CMAKE_PREFIX_PATH
