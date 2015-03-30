# File automatically generated: DO NOT EDIT.

# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

add_library(Geant4GeoConverter MODULE IMPORTED)
set(DetectorDescription/DetConverterTools/Geant4GeoConverter_DEPENDENCIES GaudiAlg;GaudiKernel;DetectorDescription/DetConverterTools/IConverter;DetectorDescription/DetDesServices;GaudiCoreSvc)
set(DetectorDescription/DetConverterTools/Geant4GeoConverter_VERSION v1r0)
