# File automatically generated: DO NOT EDIT.

# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

add_library(TestTools MODULE IMPORTED)
set(Test/TestTools_DEPENDENCIES GaudiAlg;GaudiKernel;DataObjects;Test/TestInterfaces;GaudiCoreSvc)
set(Test/TestTools_VERSION v1r0)
