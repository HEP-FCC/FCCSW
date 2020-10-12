LCGPREFIX=/cvmfs/sft.cern.ch/lcg
LCGPATH=/cvmfs/sft.cern.ch/lcg/views/LCG_97a_FCC_2/x86_64-centos7-gcc8-opt
export BINARY_TAG=x86_64-centos7-gcc8-opt
source $LCGPATH/bin/thisdd4hep_only.sh

export Gaudi_DIR=/cvmfs/sft.cern.ch/lcg/releases/Gaudi/v33r1-2775b/x86_64-centos7-gcc8-opt
export CMAKE_PREFIX_PATH=$Gaudi_DIR:$CMAKE_PREFIX_PATH
source /cvmfs/sft.cern.ch/lcg/views/LCG_97a_FCC_2/x86_64-centos7-gcc8-opt/setup.sh

export FCCSWBASEDIR=/cvmfs/sft.cern.ch/lcg/releases/fccsw/0.13-5b877/x86_64-centos7-gcc8-opt/
export FCCSWSHAREDIR=$FCCSWBASEDIR/share/FCCSW
export FCC_DETECTORS=$FCCSWSHAREDIR
export FCC_PYTHIACARDS=$FCCSWSHAREDIR
export FCCSW=/cvmfs/sft.cern.ch/lcg/releases/fccsw/0.13-5b877/x86_64-centos7-gcc8-opt/share/FCCSW

