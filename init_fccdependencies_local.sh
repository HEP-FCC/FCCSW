
mkdir -p external
cd external
touch .gaudi_project_ignore


echo "Building FCCSW dependencies locally ..."

if [ ! -d "./podio" ] 
then
  git clone --depth=1 https://github.com/aidasoft/podio
  cd podio; mkdir build install; cd build;
  cmake .. -DCMAKE_INSTALL_PREFIX=../install -DBUILD_TESTING=OFF
  make -j `getconf _NPROCESSORS_ONLN` install
  cd ../
  touch .gaudi_project_ignore
  cd ../
fi

  cd podio && \
  export CMAKE_PREFIX_PATH=$PWD/install:$CMAKE_PREFIX_PATH && \
  export ROOT_INCLUDE_PATH=$PWD/install/include/:$PWD/install/:$ROOT_INCLUDE_PATH && \
  export LD_LIBRARY_PATH=$PWD/install/lib/:$PWD/install/lib64/:$LD_LIBRARY_PATH && \
  touch .gaudi_project_ignore && \
  cd ../;

if [ ! -d "./fcc-edm" ] 
then
  git clone --depth=1 https://github.com/hep-fcc/fcc-edm
  cd fcc-edm; mkdir build install; cd build;
  cmake .. -DCMAKE_INSTALL_PREFIX=../install -DBUILD_TESTING=OFF
  make -j `getconf _NPROCESSORS_ONLN` install
  cd ../
  touch .gaudi_project_ignore
  cd ../
fi

  cd fcc-edm && \
  export CMAKE_PREFIX_PATH=$PWD/install:$CMAKE_PREFIX_PATH && \
  export ROOT_INCLUDE_PATH=$PWD/install/include/:$PWD/install/:$ROOT_INCLUDE_PATH && \
  export LD_LIBRARY_PATH=$PWD/install/lib/:$PWD/install/lib64/:$LD_LIBRARY_PATH && \
  touch .gaudi_project_ignore && \
  cd ../;

if [ ! -d "./edm4hep" ] 
then
  git clone --depth=1 https://github.com/key4hep/edm4hep
  cd edm4hep; mkdir build install; cd build;
  cmake .. -DCMAKE_INSTALL_PREFIX=../install -DBUILD_TESTING=OFF
  make -j `getconf _NPROCESSORS_ONLN` install
  cd ../
  touch .gaudi_project_ignore
  cd ../
fi

  cd edm4hep && \
  export CMAKE_PREFIX_PATH=$PWD/install:$CMAKE_PREFIX_PATH && \
  export ROOT_INCLUDE_PATH=$PWD/install/include/:$PWD/install/:$ROOT_INCLUDE_PATH && \
  export LD_LIBRARY_PATH=$PWD/install/lib/:$PWD/install/lib64/:$LD_LIBRARY_PATH && \
  touch .gaudi_project_ignore && \
  cd ../;

if [ ! -d "./k4fwcore" ] 
then
  git clone --depth=1 https://github.com/key4hep/k4fwcore
  cd k4fwcore; mkdir build install; cd build;
  cmake .. -DCMAKE_INSTALL_PREFIX=../install -DBUILD_TESTING=OFF
  make -j `getconf _NPROCESSORS_ONLN` install
  cd ../
  touch .gaudi_project_ignore
  cd ../
fi

  cd k4fwcore && \
  export CMAKE_PREFIX_PATH=$PWD/install:$CMAKE_PREFIX_PATH && \
  export ROOT_INCLUDE_PATH=$PWD/install/include/:$PWD/install/:$ROOT_INCLUDE_PATH && \
  export PYTHONPATH=$PWD/install:$PYTHONPATH && \
  export LD_LIBRARY_PATH=$PWD/install/lib/:$PWD/install/lib64/:$LD_LIBRARY_PATH && \
  touch .gaudi_project_ignore && \
  cd ../;

cd ..;
