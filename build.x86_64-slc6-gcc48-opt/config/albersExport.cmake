# File automatically generated: DO NOT EDIT.

# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)

add_library(albers SHARED IMPORTED)
set_target_properties(albers PROPERTIES
  REQUIRED_INCLUDE_DIRS "ROOT"
  REQUIRED_LIBRARIES "${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libCore.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libRIO.so;${LCG_releases}/ROOT/5.34.18/x86_64-slc6-gcc48-opt/lib/libTree.so"
  IMPORTED_SONAME "libalbers.so"
  IMPORTED_LOCATION "${_IMPORT_PREFIX}/lib/libalbers.so"
  )
set(albers_DEPENDENCIES GaudiAlg)
set(albers_VERSION v1r0)
