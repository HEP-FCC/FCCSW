# parse binary tag
include(BinaryTagUtils)
if(NOT BINARY_TAG_COMP)
  parse_binary_tag()
endif()
if(NOT HOST_BINARY_TAG_ARCH)
  get_host_binary_tag(HOST_BINARY_TAG)
  parse_binary_tag(HOST_BINARY_TAG)
endif()

check_compiler()


# Convert BINARY_TAG_TYPE to CMAKE_BUILD_TYPE
if(BINARY_TAG_TYPE STREQUAL "opt")
  set(_BT_CMAKE_BUILD_TYPE Release)
elseif(BINARY_TAG_TYPE MATCHES "^dbg|do0$")
  set(_BT_CMAKE_BUILD_TYPE Debug)
elseif(BINARY_TAG_TYPE STREQUAL "cov")
  set(_BT_CMAKE_BUILD_TYPE Coverage)
elseif(BINARY_TAG_TYPE STREQUAL "pro")
  set(_BT_CMAKE_BUILD_TYPE Profile)
#elseif(BINARY_TAG_TYPE STREQUAL "o2g")
#  set(CMAKE_BUILD_TYPE RelWithDebInfo)
#elseif(BINARY_TAG_TYPE STREQUAL "min")
#  set(CMAKE_BUILD_TYPE MinSizeRel)
else()
  message(FATAL_ERROR "BINARY_TAG build type ${BINARY_TAG_TYPE} not supported.")
endif()
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE ${_BT_CMAKE_BUILD_TYPE} CACHE STRING
      "Choose the type of build, options are: Release, Debug, Coverage, Profile, RelWithDebInfo, MinSizeRel."
      FORCE)
else()
  if(NOT _BT_CMAKE_BUILD_TYPE STREQUAL CMAKE_BUILD_TYPE)
    message(WARNING "CMAKE_BUILD_TYPE set to ${CMAKE_BUILD_TYPE}, but BINARY_TAG build type ${BINARY_TAG_TYPE} implies ${_BT_CMAKE_BUILD_TYPE}")
  endif()
endif()


# Report the platform ids.
message(STATUS "Target system:    ${BINARY_TAG}")
message(STATUS "CMake build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Host system:      ${HOST_BINARY_TAG}")
message(STATUS "LCG system:       ${LCG_SYSTEM}")


# define a minimun default version
set(GAUDI_CXX_STANDARD_DEFAULT "c++14")
# overriddend depending on the compiler
if (BINARY_TAG_COMP_NAME STREQUAL "clang" AND BINARY_TAG_COMP_VERSION VERSION_GREATER "3.6")
  set(GAUDI_CXX_STANDARD_DEFAULT "c++14")
elseif(BINARY_TAG_COMP_NAME STREQUAL "gcc")
  # Special defaults
  if (BINARY_TAG_COMP_VERSION VERSION_LESS "4.7")
    set(GAUDI_CXX_STANDARD_DEFAULT "c++98")
  elseif(BINARY_TAG_COMP_VERSION VERSION_LESS "4.9")
    # C++11 is enable by default on 4.7 <= gcc < 4.9
    set(GAUDI_CXX_STANDARD_DEFAULT "c++11")
  elseif(BINARY_TAG_COMP_VERSION VERSION_LESS "5.1")
    # C++1y (C++14 preview) is enable by default on 4.9 <= gcc < 5.1
    set(GAUDI_CXX_STANDARD_DEFAULT "c++1y")
  else()
    # C++14 is enable by default on gcc >= 5.1
    set(GAUDI_CXX_STANDARD_DEFAULT "c++14")
    option(GAUDI_GCC_OLD_ABI "use old gcc ABI for c++11 and above (gcc >= 5.1)"
           OFF)
  endif()
endif()
# special for GaudiHive
set(GAUDI_CPP11_DEFAULT ON)

#--- Gaudi Build Options -------------------------------------------------------
# Build options that map to compile time features
#
option(GAUDI_V21
       "disable backward compatibility hacks (implies all G21_* options)"
       OFF)
option(G21_HIDE_SYMBOLS
       "enable explicit symbol visibility on gcc-4"
       OFF)
option(G21_NEW_INTERFACES
       "disable backward-compatibility hacks in IInterface and InterfaceID"
       OFF)
option(G21_NO_DEPRECATED
       "remove deprecated methods and functions"
       OFF)
option(G22_NEW_SVCLOCATOR
       "use (only) the new interface of the ServiceLocator"
       OFF)
option(GAUDI_V22
       "enable some API extensions"
       OFF)

if (BINARY_TAG_COMP_NAME STREQUAL "gcc" AND BINARY_TAG_COMP_VERSION VERSION_GREATER "5.0")
  option(GAUDI_SUGGEST_OVERRIDE "enable warnings for missing override keyword" ON)
endif()


string(COMPARE EQUAL "${BINARY_TAG_TYPE}" "do0" GAUDI_SLOW_DEBUG_DEFAULT)
option(GAUDI_SLOW_DEBUG
       "turn off all optimizations in debug builds"
       ${GAUDI_SLOW_DEBUG_DEFAULT})

# set optimization flags
string(TOUPPER "${CMAKE_BUILD_TYPE}" _up_bt)
foreach(_subtype ${BINARY_TAG_SUBTYPE})
  if(_subtype MATCHES "^[oO]([0-3sg]|fast)$")
    set(_opt_level_${_up_bt} "${_opt_level_${_up_bt}} -O${CMAKE_MATCH_1}")
    #message(STATUS "setting _opt_level_${_up_bt} -> ${_opt_level_${_up_bt}}")
  elseif(_subtype STREQUAL "g")
    set(_opt_level_${_up_bt} "${_opt_level_${_up_bt}} -g")
  endif()
endforeach()

if(_opt_level_RELWITHDEBINFO)
  # RelWithDebInfo shared the flags with Release
  set(_opt_level_RELEASE "${_opt_level_RELWITHDEBINFO}")
endif()

# default optimization levels
if(NOT _opt_level_RELEASE)
  set(_opt_level_RELEASE "-O3")
  # RelWithDebInfo shared the flags with Release
  set(_opt_level_RELWITHDEBINFO "${_opt_level_RELEASE}")
endif()
if(NOT _opt_level_DEBUG)
  if(NOT GAUDI_SLOW_DEBUG AND
     (BINARY_TAG_COMP_NAME STREQUAL "gcc" AND NOT BINARY_TAG_COMP_VERSION VERSION_LESS "4.8"))
    # Use -Og with Debug builds in gcc >= 4.8 (if not disabled)
    set(_opt_level_DEBUG "-Og")
  else()
    set(_opt_level_DEBUG "-O0")
  endif()
endif()

if(_opt_level_${_up_bt})
  message(STATUS "Optimization:     ${_opt_level_${_up_bt}}")
endif()

# special architecture flags
set(GAUDI_ARCH_DEFAULT)
if(BINARY_TAG_MICROARCH)
  set(GAUDI_ARCH_DEFAULT ${BINARY_TAG_MICROARCH})
elseif(BINARY_TAG_COMP_NAME STREQUAL "gcc" AND BINARY_TAG_COMP_VERSION VERSION_GREATER "5.0" AND
   BINARY_TAG_ARCH STREQUAL "x86_64")
  set(GAUDI_ARCH_DEFAULT "sse4.2")
else()
  if (NOT HOST_BINARY_TAG_ARCH STREQUAL BINARY_TAG_ARCH)
    if (HOST_BINARY_TAG_ARCH STREQUAL "x86_64" AND BINARY_TAG_ARCH STREQUAL "i686")
      set(GAUDI_ARCH_DEFAULT "32")
    else()
      message(FATAL_ERROR "Cannot build for ${BINARY_TAG_ARCH} on ${HOST_BINARY_TAG_ARCH}.")
    endif()
  endif()
endif()
set(GAUDI_ARCH "${GAUDI_ARCH_DEFAULT}"
    CACHE STRING "Which architecture-specific optimizations to use")


if(DEFINED GAUDI_CPP11)
  message(WARNING "GAUDI_CPP11 is an obsolete option, use GAUDI_CXX_STANDARD=c++11 instead")
endif()

set(GAUDI_CXX_STANDARD "${GAUDI_CXX_STANDARD_DEFAULT}"
    CACHE STRING "Version of the C++ standard to be used.")

# If modern c++ and gcc >= 5.1 and requested, use old ABI compatibility
if((NOT GAUDI_CXX_STANDARD STREQUAL "c++98") AND
   (BINARY_TAG_COMP_NAME STREQUAL "gcc" AND NOT BINARY_TAG_COMP_VERSION VERSION_LESS "5.1") AND
   GAUDI_GCC_OLD_ABI)
  add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
endif()

#--- Compilation Flags ---------------------------------------------------------
if(NOT GAUDI_FLAGS_SET)
  #message(STATUS "Setting cached build flags")

  if(MSVC90)

    set(CMAKE_CXX_FLAGS_DEBUG "/D_NDEBUG /MD /Zi /Ob0 /Od /RTC1"
        CACHE STRING "Flags used by the compiler during debug builds."
        FORCE)
    set(CMAKE_C_FLAGS_DEBUG "/D_NDEBUG /MD /Zi /Ob0 /Od /RTC1"
        CACHE STRING "Flags used by the compiler during debug builds."
        FORCE)

  else()
    # special architecture flags
    set(arch_opts)
    foreach(_arch_opt ${GAUDI_ARCH})
      if(_arch_opt STREQUAL "native")
        set(_arch_opt "arch=native")
      endif()
      set(arch_opts "${arch_opts} -m${_arch_opt}")
    endforeach()
    # Common compilation flags
    set(CMAKE_CXX_FLAGS
        "${arch_opts} -fmessage-length=0 -pipe -Wall -Wextra -Werror=return-type -pthread -pedantic -Wwrite-strings -Wpointer-arith -Woverloaded-virtual -Wno-long-long -Wno-shadow"
        CACHE STRING "Flags used by the compiler during all build types."
        FORCE)
    set(CMAKE_C_FLAGS
        "${arch_opts} -fmessage-length=0 -pipe -Wall -Wextra -Werror=return-type -pthread -pedantic -Wwrite-strings -Wpointer-arith -Wno-long-long"
        CACHE STRING "Flags used by the compiler during all build types."
        FORCE)
    set(CMAKE_Fortran_FLAGS
        "${arch_opts} -fmessage-length=0 -pipe -Wall -Wextra -Werror=return-type -pthread -pedantic -fsecond-underscore"
        CACHE STRING "Flags used by the compiler during all build types."
        FORCE)

    if (BINARY_TAG_COMP_NAME STREQUAL "gcc" AND BINARY_TAG_COMP_VERSION VERSION_GREATER "5.0" AND GAUDI_SUGGEST_OVERRIDE)
        set(CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS} -Wsuggest-override"
            CACHE STRING "Flags used by the compiler during all build types."
            FORCE)
    endif()

    # Build type compilation flags
    set(CMAKE_CXX_FLAGS_RELEASE "${_opt_level_RELEASE} -DNDEBUG"
        CACHE STRING "Flags used by the compiler during release builds."
        FORCE)
    set(CMAKE_C_FLAGS_RELEASE "${_opt_level_RELEASE} -DNDEBUG"
        CACHE STRING "Flags used by the compiler during release builds."
        FORCE)
    set(CMAKE_Fortran_FLAGS_RELEASE "${_opt_level_RELEASE} -DNDEBUG"
        CACHE STRING "Flags used by the compiler during release builds."
        FORCE)

    set(CMAKE_CXX_FLAGS_DEBUG "${_opt_level_DEBUG} -g"
        CACHE STRING "Flags used by the compiler during Debug builds."
        FORCE)
    set(CMAKE_C_FLAGS_DEBUG "${_opt_level_DEBUG} -g"
        CACHE STRING "Flags used by the compiler during Debug builds."
        FORCE)
    set(CMAKE_Fortran_FLAGS_DEBUG "${_opt_level_DEBUG} -g"
        CACHE STRING "Flags used by the compiler during Debug builds."
        FORCE)

    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} -g"
        CACHE STRING "Flags used by the compiler during Release with Debug Info builds."
        FORCE)
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELEASE} -g"
        CACHE STRING "Flags used by the compiler during Release with Debug Info builds."
        FORCE)
    set(CMAKE_Fortran_FLAGS_RELWITHDEBINFO "${CMAKE_Fortran_FLAGS_RELEASE} -g"
        CACHE STRING "Flags used by the compiler during Release with Debug Info builds."
        FORCE)

    set(CMAKE_CXX_FLAGS_COVERAGE "--coverage"
        CACHE STRING "Flags used by the compiler during coverage builds."
        FORCE)
    set(CMAKE_C_FLAGS_COVERAGE "--coverage"
        CACHE STRING "Flags used by the compiler during coverage builds."
        FORCE)
    set(CMAKE_Fortran_FLAGS_COVERAGE "--coverage"
        CACHE STRING "Flags used by the compiler during coverage builds."
        FORCE)

    set(CMAKE_CXX_FLAGS_PROFILE "-pg"
        CACHE STRING "Flags used by the compiler during profile builds."
        FORCE)
    set(CMAKE_C_FLAGS_PROFILE "-pg"
        CACHE STRING "Flags used by the compiler during profile builds."
        FORCE)
    set(CMAKE_Fortran_FLAGS_PROFILE "-pg"
        CACHE STRING "Flags used by the compiler during profile builds."
        FORCE)


    # The others are already marked as 'advanced' by CMake, these are custom.
    mark_as_advanced(CMAKE_C_FLAGS_COVERAGE CMAKE_CXX_FLAGS_COVERAGE CMAKE_Fortran_FLAGS_COVERAGE
                     CMAKE_C_FLAGS_PROFILE CMAKE_CXX_FLAGS_PROFILE CMAKE_Fortran_FLAGS_PROFILE)
    mark_as_advanced(CMAKE_Fortran_COMPILER CMAKE_Fortran_FLAGS
                     CMAKE_Fortran_FLAGS_RELEASE CMAKE_Fortran_FLAGS_RELWITHDEBINFO)

  endif()

  #--- Link shared flags -------------------------------------------------------
  if (CMAKE_SYSTEM_NAME MATCHES Linux)
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed -Wl,--no-undefined  -Wl,-z,max-page-size=0x1000"
        CACHE STRING "Flags used by the linker during the creation of dll's."
        FORCE)
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--as-needed -Wl,--no-undefined  -Wl,-z,max-page-size=0x1000"
        CACHE STRING "Flags used by the linker during the creation of modules."
        FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--no-undefined  -Wl,-z,max-page-size=0x1000"
        CACHE STRING "Flags used by the linker during the creation of executables."
        FORCE)
  endif()

  if(APPLE)
    # special link options for MacOSX
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup"
        CACHE STRING "Flags used by the linker during the creation of dll's."
        FORCE)
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -undefined dynamic_lookup"
        CACHE STRING "Flags used by the linker during the creation of modules."
        FORCE)
  endif()

  # prevent resetting of the flags
  set(GAUDI_FLAGS_SET ON
      CACHE INTERNAL "flag to check if the compilation flags have already been set")
endif()


if(UNIX)
  add_definitions(-D_GNU_SOURCE -Dunix -Df2cFortran)

  if (CMAKE_SYSTEM_NAME MATCHES Linux)
    add_definitions(-Dlinux)
  endif()
endif()

if(MSVC90)
  add_definitions(/wd4275 /wd4251 /wd4351)
  add_definitions(-DBOOST_ALL_DYN_LINK -DBOOST_ALL_NO_LIB)
  add_definitions(/nologo)
endif()

if(APPLE)
  # by default, CMake uses the option -bundle for modules, but we need -dynamiclib for them too
  string(REPLACE "-bundle" "-dynamiclib" CMAKE_SHARED_MODULE_CREATE_C_FLAGS "${CMAKE_SHARED_MODULE_CREATE_C_FLAGS}")
  string(REPLACE "-bundle" "-dynamiclib" CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS "${CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS}")
endif()

#--- Special build flags -------------------------------------------------------
if ((GAUDI_V21 OR G21_HIDE_SYMBOLS) AND (BINARY_TAG_COMP_NAME STREQUAL "gcc" AND
                                         NOT BINARY_TAG_COMP_VERSION VERSION_LESS "4.0"))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
endif()

# handle options to choose the  version of the C++ standard
string(TOLOWER "${GAUDI_CXX_STANDARD}" GAUDI_CXX_STANDARD)
if(GAUDI_CXX_STANDARD STREQUAL "ansi")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=${GAUDI_CXX_STANDARD}")
  if(NOT GAUDI_CXX_STANDARD STREQUAL "c++98")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
  else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")
  endif()
endif()

if(BINARY_TAG_COMP_NAME STREQUAL "clang" AND BINARY_TAG_COMP_VERSION VERSION_EQUAL "3.7")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --gcc-toolchain=${lcg_system_compiler_path}")
endif()

if(NOT GAUDI_V21)
  if(GAUDI_V22)
    add_definitions(-DGAUDI_V22_API)
  else()
    add_definitions(-DGAUDI_V20_COMPAT)
  endif()
  # special case
  if(G21_HIDE_SYMBOLS AND (BINARY_TAG_COMP_NAME STREQUAL "gcc" AND
                           NOT BINARY_TAG_COMP_VERSION VERSION_LESS "4.0"))
    add_definitions(-DG21_HIDE_SYMBOLS)
  endif()
  #
  foreach (feature G21_NEW_INTERFACES G21_NO_DEPRECATED G22_NEW_SVCLOCATOR)
    if (${feature})
      add_definitions(-D${feature})
    endif()
  endforeach()
endif()

#--- Tuning of warnings --------------------------------------------------------
if(GAUDI_HIDE_WARNINGS)
  if(BINARY_TAG_COMP_NAME STREQUAL "clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -Wno-deprecated -Wno-overloaded-virtual -Wno-char-subscripts -Wno-unused-parameter -Wno-unused-local-typedefs -Wno-missing-braces")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-empty-body")
    if(BINARY_TAG_COMP_VERSION VERSION_GREATER "4.7")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
    endif()
  endif()
else()
  if(BINARY_TAG_COMP_NAME STREQUAL "gcc" AND NOT BINARY_TAG_COMP_VERSION VERSION_LESS "5.0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsuggest-override")
  endif()
endif()

#--- Special flags -------------------------------------------------------------
# FIXME: enforce the use of Boost Filesystem V3 to be compatible between 1.44 and 1.48
add_definitions(-DBOOST_FILESYSTEM_VERSION=3)
# FIXME: enforce the use of Boost Phoenix V3 (V2 does not work with recent compilers)
#        see http://stackoverflow.com/q/20721486
#        and http://stackoverflow.com/a/20440238/504346
add_definitions(-DBOOST_SPIRIT_USE_PHOENIX_V3)

if(BINARY_TAG_COMP_NAME STREQUAL "gcc" AND BINARY_TAG_COMP_VERSION VERSION_EQUAL "4.3")
  # The -pedantic flag gives problems on GCC 4.3.
  string(REPLACE "-pedantic" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(REPLACE "-pedantic" "" CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}")
endif()

if(GAUDI_ATLAS)
  # FIXME: this macro is used in ATLAS to simplify the migration to Gaudi v25,
  #        unfortunately it's not possible to detect the version of Gaudi at this point
  #        so we assume that any CMake-based build in ATLAS uses Gaudi >= v25
  add_definitions(-DHAVE_GAUDI_PLUGINSVC)

  add_definitions(-DATLAS_GAUDI_V21)
  add_definitions(-DATLAS)
  include(AthenaBuildFlags OPTIONAL)
else()
  # FIXME: these macros are LHCb specific, but we do not have yet a way to set
  # compile flags in a project, such that they are inherited by other projects.
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DGOD_NOALLOC)
  endif()
endif()
