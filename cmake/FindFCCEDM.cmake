set(searchpath ${FCCEDM_DIR} ${ALBERS_DIR}  )

# first find the FCC EDM
find_library(FCCEDM_LIBRARY
              NAMES datamodel
              HINTS ${searchpath}
              PATH_SUFFIXES lib)

find_path(FCCEDM_INCLUDE_DIR
           NAMES datamodel/Point.h
           HINTS ${searchpath} $ENV{FCCEDM_DIR}/include
           PATH_SUFFIXES include)

unset(searchpath)

set(FCCEDM_INCLUDE_DIRS ${FCC_INCLUDE_DIR})
set(FCCEDM_LIBRARIES ${FCCEDM_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FCCEDM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(FCCEDM DEFAULT_MSG FCCEDM_INCLUDE_DIR FCCEDM_LIBRARY)

mark_as_advanced(FCCEDM_FOUND)