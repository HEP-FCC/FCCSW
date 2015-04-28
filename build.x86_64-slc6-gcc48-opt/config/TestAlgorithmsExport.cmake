# File automatically generated: DO NOT EDIT.

# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

add_library(TestAlgorithms MODULE IMPORTED)
set(Test/TestAlgorithms_DEPENDENCIES GaudiAlg;GaudiKernel;DetectorDescription/DetDesInterfaces;Test/TestInterfaces;RecoGeometry;DataObjects;GaudiCoreSvc)
set(Test/TestAlgorithms_VERSION v1r0)
