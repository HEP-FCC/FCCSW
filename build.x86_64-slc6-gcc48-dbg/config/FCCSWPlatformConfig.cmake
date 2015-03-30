# File automatically generated: DO NOT EDIT.

# Get the exported informations about the targets
get_filename_component(_dir "${CMAKE_CURRENT_LIST_FILE}" PATH)
#include(${_dir}/FCCSWExports.cmake)

# Set useful properties
get_filename_component(_dir "${_dir}" PATH)
set(FCCSW_INCLUDE_DIRS ${_dir}/include)
set(FCCSW_LIBRARY_DIRS ${_dir}/lib)

set(FCCSW_BINARY_PATH ${_dir}/bin ${_dir}/scripts)
set(FCCSW_PYTHON_PATH ${_dir}/python)

set(FCCSW_COMPONENT_LIBRARIES DetDesServices;Detectors;Geant4GeoConverter;RecoGeoConverter;FWCorePlugins;Examples;Generation;Reconstruction;Simulation;TestAlgorithms)
set(FCCSW_LINKER_LIBRARIES albers;DataObjects;TrkGeometryUtils;TrkParametersBase;RecoGeometry;FWCore;TrkChargedParameters;TrkNeutralParameters)

set(FCCSW_ENVIRONMENT PREPEND;PYTHONPATH;\${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib;PREPEND;PATH;/afs/cern.ch/sw/lcg/external/gcc/4.8.1/x86_64-slc6-gcc48-opt/bin;PREPEND;PATH;\${LCG_releases}/Python/2.7.6/x86_64-slc6-gcc48-opt/bin;PREPEND;PATH;\${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/bin;PREPEND;LD_LIBRARY_PATH;\${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib;PREPEND;LD_LIBRARY_PATH;\${LCG_releases}/HepMC/2.06.08/x86_64-slc6-gcc48-opt/lib;PREPEND;LD_LIBRARY_PATH;\${LCG_releases}/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/lib;PREPEND;LD_LIBRARY_PATH;\${LCG_releases}/tbb/42_20131118/x86_64-slc6-gcc48-opt/lib;PREPEND;LD_LIBRARY_PATH;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/lib;PREPEND;LD_LIBRARY_PATH;\${LCG_releases}/MCGenerators/pythia8/186/x86_64-slc6-gcc48-opt/lib;PREPEND;LD_LIBRARY_PATH;\${LCG_releases}/fastjet/3.0.6/x86_64-slc6-gcc48-opt/lib;PREPEND;LD_LIBRARY_PATH;/afs/cern.ch/sw/lcg/external/gcc/4.8.1/x86_64-slc6-gcc48-opt/lib64;PREPEND;LD_LIBRARY_PATH;\${LCG_releases}/XercesC/3.1.1p1/x86_64-slc6-gcc48-opt/lib;SET;ROOTSYS;\${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt;PREPEND;ROOT_INCLUDE_PATH;/afs/cern.ch/user/j/jhrdinka/FCC/GAUDI/GAUDI_v25r2/InstallArea/x86_64-slc6-gcc48-opt/include;PREPEND;ROOT_INCLUDE_PATH;\${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/include;PREPEND;ROOT_INCLUDE_PATH;\${LCG_releases}/Boost/1.55.0_python2.7/x86_64-slc6-gcc48-opt/include/boost-1_55;PREPEND;ROOT_INCLUDE_PATH;\${LCG_releases}/tbb/42_20131118/x86_64-slc6-gcc48-opt/include;PREPEND;ROOT_INCLUDE_PATH;\${LCG_releases}/AIDA/3.2.1/x86_64-slc6-gcc48-opt/src/cpp;PREPEND;ROOT_INCLUDE_PATH;\${LCG_releases}/HepMC/2.06.08/x86_64-slc6-gcc48-opt/include;PREPEND;ROOT_INCLUDE_PATH;/afs/cern.ch/exp/fcc/sw/0.2/DD4hep/v00-09/x86_64-slc6-gcc48-dbg/include;PREPEND;ROOT_INCLUDE_PATH;/afs/cern.ch/exp/fcc/sw/0.2/Geant4/9.6.p03/x86_64-slc6-gcc48-dbg/include/Geant4;PREPEND;ROOT_INCLUDE_PATH;/afs/cern.ch/exp/fcc/sw/0.2/clhep/2.1.4.1/x86_64-slc6-gcc48-dbg/include;PREPEND;ROOT_INCLUDE_PATH;/afs/cern.ch/exp/fcc/sw/0.2/XercesC/3.1.1p1/x86_64-slc6-gcc48-dbg/include;PREPEND;ROOT_INCLUDE_PATH;\${LCG_releases}/MCGenerators/pythia8/186/x86_64-slc6-gcc48-opt/include;PREPEND;ROOT_INCLUDE_PATH;\${LCG_releases}/fastjet/3.0.6/x86_64-slc6-gcc48-opt/include;PREPEND;PATH;\${.}/scripts;PREPEND;PATH;\${.}/bin;PREPEND;LD_LIBRARY_PATH;\${.}/lib;PREPEND;ROOT_INCLUDE_PATH;\${.}/include;PREPEND;PYTHONPATH;\${.}/python;PREPEND;PYTHONPATH;\${.}/python/lib-dynload;SET;FCCSW_PROJECT_ROOT;\${.}/../../;SET;ALBERSROOT;\${FCCSW_PROJECT_ROOT}/albers;SET;ALGEBRAROOT;\${FCCSW_PROJECT_ROOT}/Algebra;SET;DATAOBJECTSROOT;\${FCCSW_PROJECT_ROOT}/DataObjects;SET;DETDESINTERFACESROOT;\${FCCSW_PROJECT_ROOT}/DetectorDescription/DetDesInterfaces;SET;ICONVERTERROOT;\${FCCSW_PROJECT_ROOT}/DetectorDescription/DetConverterTools/IConverter;SET;TRKGEOMETRYUTILSROOT;\${FCCSW_PROJECT_ROOT}/Tracking/TrkGeometry/TrkGeometryUtils;SET;TRKPARAMETERSBASEROOT;\${FCCSW_PROJECT_ROOT}/Tracking/TrkEvent/TrkParametersBase;SET;DETEXTENSIONSROOT;\${FCCSW_PROJECT_ROOT}/DetectorDescription/DetExtensions;SET;RECOGEOMETRYROOT;\${FCCSW_PROJECT_ROOT}/RecoGeometry;SET;DETDESSERVICESROOT;\${FCCSW_PROJECT_ROOT}/DetectorDescription/DetDesServices;SET;DETECTORSROOT;\${FCCSW_PROJECT_ROOT}/DetectorDescription/Detectors;SET;GEANT4GEOCONVERTERROOT;\${FCCSW_PROJECT_ROOT}/DetectorDescription/DetConverterTools/Geant4GeoConverter;SET;RECOGEOCONVERTERROOT;\${FCCSW_PROJECT_ROOT}/DetectorDescription/DetConverterTools/RecoGeoConverter;SET;FWCOREROOT;\${FCCSW_PROJECT_ROOT}/FWCore;SET;EXAMPLESROOT;\${FCCSW_PROJECT_ROOT}/Examples;SET;GENERATIONROOT;\${FCCSW_PROJECT_ROOT}/Generation;SET;RECONSTRUCTIONROOT;\${FCCSW_PROJECT_ROOT}/Reconstruction;SET;SIMULATIONROOT;\${FCCSW_PROJECT_ROOT}/Simulation;SET;TESTALGORITHMSROOT;\${FCCSW_PROJECT_ROOT}/Test/TestAlgorithms;SET;TRKCHARGEDPARAMETERSROOT;\${FCCSW_PROJECT_ROOT}/Tracking/TrkEvent/TrkChargedParameters;SET;TRKNEUTRALPARAMETERSROOT;\${FCCSW_PROJECT_ROOT}/Tracking/TrkEvent/TrkNeutralParameters)

set(FCCSW_EXPORTED_SUBDIRS)
foreach(p albers;Algebra;DataObjects;DetectorDescription/DetDesInterfaces;DetectorDescription/DetConverterTools/IConverter;Tracking/TrkGeometry/TrkGeometryUtils;Tracking/TrkEvent/TrkParametersBase;DetectorDescription/DetExtensions;RecoGeometry;DetectorDescription/DetDesServices;DetectorDescription/Detectors;DetectorDescription/DetConverterTools/Geant4GeoConverter;DetectorDescription/DetConverterTools/RecoGeoConverter;FWCore;Examples;Generation;Reconstruction;Simulation;Test/TestAlgorithms;Tracking/TrkEvent/TrkChargedParameters;Tracking/TrkEvent/TrkNeutralParameters)
  get_filename_component(pn ${p} NAME)
  if(EXISTS ${_dir}/cmake/${pn}Export.cmake)
    set(FCCSW_EXPORTED_SUBDIRS ${FCCSW_EXPORTED_SUBDIRS} ${p})
  endif()
endforeach()

set(FCCSW_OVERRIDDEN_SUBDIRS )
