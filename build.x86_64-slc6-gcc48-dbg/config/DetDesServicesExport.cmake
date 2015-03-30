# File automatically generated: DO NOT EDIT.

# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

add_library(DetDesServices MODULE IMPORTED)
set(DetectorDescription/DetDesServices_DEPENDENCIES GaudiKernel;DetectorDescription/DetDesInterfaces;DetectorDescription/DetConverterTools/IConverter;RecoGeometry;GaudiCoreSvc)
set(DetectorDescription/DetDesServices_VERSION v1r0)
