# File automatically generated: DO NOT EDIT.

# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

add_library(Examples MODULE IMPORTED)
set(Examples_DEPENDENCIES GaudiAlg;DataObjects;FWCore;GaudiCoreSvc)
set(Examples_VERSION v1r0)
