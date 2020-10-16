LCGPREFIX=/cvmfs/sft.cern.ch/lcg
export BINARY_TAG=x86_64-centos7-gcc8-opt
LCGPATH=$LCGPREFIX/views/LCG_97a_FCC_2/$BINARY_TAG
source $LCGPATH/bin/thisdd4hep_only.sh
source $LCGPATH/setup.sh


export Gaudi_DIR=$(dirname $(readlink -f "$(which gaudirun.py)"))/../
export CMAKE_PREFIX_PATH=$Gaudi_DIR:$CMAKE_PREFIX_PATH

if [ ! -d "./edm4hep" ] 
then
  git clone --depth=1 https://github.com/key4hep/edm4hep
  cd edm4hep; mkdir build install; cd build;
  cmake .. -DCMAKE_INSTALL_PREFIX=../install -DBUILD_TESTING=OFF
  make -j `getconf _NPROCESSORS_ONLN` install
  cd ../
  export CMAKE_PREFIX_PATH=$PWD/install:$CMAKE_PREFIX_PATH
  export ROOT_INCLUDE_PATH=$PWD/install/include/edm4hep:$PWD/install/include/:$ROOT_INCLUDE_PATH
  export LD_LIBRARY_PATH=$PWD/install/lib/:$PWD/install/lib64/$LD_LIBRARY_PATH
  touch .gaudi_project_ignore
  cd ../
fi


if [ ! -d "./k4fwcore" ] 
then
  git clone --depth=1 https://github.com/key4hep/k4fwcore
  cd edm4hep; mkdir build install; cd build;
  cmake .. -DCMAKE_INSTALL_PREFIX=../install -DBUILD_TESTING=OFF
  make -j `getconf _NPROCESSORS_ONLN` install
  cd ../
  export CMAKE_PREFIX_PATH=$PWD/install:$CMAKE_PREFIX_PATH
  export PYTHONPATH=$PWD/install:$PYTHONPATH
  export LD_LIBRARY_PATH=$PWD/install/lib/:$PWD/install/lib64/$LD_LIBRARY_PATH
  touch .gaudi_project_ignore
  cd ../
fi

