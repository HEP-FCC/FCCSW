LCGPREFIX=/cvmfs/sft.cern.ch/lcg
export BINARY_TAG=x86_64-centos7-gcc8-opt
LCGPATH=$LCGPREFIX/views/LCG_98/$BINARY_TAG
source $LCGPATH/bin/thisdd4hep_only.sh
source $LCGPATH/setup.sh


# make sure Gaudi.xenv can be found

export Gaudi_DIR=$(dirname $(readlink -f "$(which gaudirun.py)"))/../
export CMAKE_PREFIX_PATH=$Gaudi_DIR:$CMAKE_PREFIX_PATH

# build FCC-dependencies on top of LCG
source ./init_fccdependencies_local.sh
