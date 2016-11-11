# Special toolchain file that inherits the same heptools version as the
# used projects.
find_file(use_heptools_module NAME "UseHEPTools.cmake" "/afs/cern.ch/exp/fcc/sw/0.8pre1/Gaudi/v28r0pre1/x86_64-slc6-gcc49-opt")

set(LCG_releases_base "$ENV{LCGPREFIX}/releases")
set(BINARY_TAG $ENV{BINARY_TAG})

# this check is needed because the toolchain seem to be called a second time
# without the proper cache
if(use_heptools_module)
  include(${use_heptools_module})

  use_heptools(86)
endif()
