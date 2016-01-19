set(searchpath $ENV{PODIO}  )

find_library(PODIO_LIBRARY
              NAMES podio
              HINTS ${searchpath}
              PATH_SUFFIXES lib)

find_path(PODIO_INCLUDE_DIR
           NAMES podio/CollectionBase.h
           HINTS ${searchpath}
           PATH_SUFFIXES include)

unset(searchpath)

set(PODIO_INCLUDE_DIRS ${PODIO_INCLUDE_DIR})
set(PODIO_LIBRARIES ${PODIO_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FCCEDM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(PODIO DEFAULT_MSG PODIO_LIBRARY PODIO_INCLUDE_DIR)

mark_as_advanced(PODIO_FOUND)
