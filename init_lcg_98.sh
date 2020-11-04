LCGPREFIX=/cvmfs/sft.cern.ch/lcg
export BINARY_TAG=x86_64-centos7-gcc8-opt
LCGPATH=$LCGPREFIX/views/LCG_98/$BINARY_TAG
source $LCGPATH/bin/thisdd4hep_only.sh
source $LCGPATH/setup.sh


# make sure Gaudi.xenv can be found

export Gaudi_DIR=$(dirname $(readlink -f "$(which gaudirun.py)"))/../
export CMAKE_PREFIX_PATH=$Gaudi_DIR:$CMAKE_PREFIX_PATH

# build FCC-dependencies on top of LCG

if [ ! -d "./fcc-edm" ] 
then
  git clone --depth=1 https://github.com/hep-fcc/fcc-edm
  cd fcc-edm; mkdir build install; cd build;
  cmake .. -DCMAKE_INSTALL_PREFIX=../install
  make -j `getconf _NPROCESSORS_ONLN` install
  cd ../
  export CMAKE_PREFIX_PATH=$PWD/install:$CMAKE_PREFIX_PATH
  export ROOT_INCLUDE_PATH=$PWD/install/include/datamodel:$PWD/install/:$ROOT_INCLUDE_PATH
  export LD_LIBRARY_PATH=$PWD/install/lib/:$PWD/install/lib64/$LD_LIBRARY_PATH
  touch .gaudi_project_ignore
  cd ../
fi


