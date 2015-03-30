# File automatically generated: DO NOT EDIT.

# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

add_library(RecoGeoConverter MODULE IMPORTED)
set(DetectorDescription/DetConverterTools/RecoGeoConverter_DEPENDENCIES GaudiAlg;GaudiKernel;GaudiSvc;DetectorDescription/DetConverterTools/IConverter;DetectorDescription/DetDesServices;RecoGeometry;DetectorDescription/DetExtensions;GaudiCoreSvc)
set(DetectorDescription/DetConverterTools/RecoGeoConverter_VERSION v1r0)
