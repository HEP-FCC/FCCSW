set(searchpath ${ALBERS_DIR}  )

find_library(ALBERS_LIBRARY
              NAMES albers
              HINTS ${searchpath}
              PATH_SUFFIXES lib)

find_path(ALBERS_INCLUDE_DIR
           NAMES albers/CollectionBase.h
           HINTS ${searchpath}
           PATH_SUFFIXES include)

unset(searchpath)

set(ALBERS_INCLUDE_DIRS ${ALBERS_INCLUDE_DIR})
set(ALBERS_LIBRARIES ${ALBERS_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FCCEDM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(ALBERS DEFAULT_MSG ALBERS_LIBRARY ALBERS_INCLUDE_DIR)

mark_as_advanced(ALBERS_FOUND)