# Special toolchain file that inherits the same heptools version as the
# used projects.
find_file(use_heptools_module NAME "UseHEPTools.cmake" HINTS "/afs/cern.ch/exp/fcc/sw/0.8pre2/Gaudi/v28r0/x86_64-slc6-gcc49-opt/cmake")

set(LCG_releases_base "$ENV{LCGPREFIX}/releases")
set(BINARY_TAG $ENV{BINARY_TAG})
set(LCG_SYSTEM x86_64-slc6-gcc49)

# this check is needed because the toolchain seem to be called a second time
# without the proper cache
if(inherit_heptools_module)
  include(${inherit_heptools_module})
  inherit_heptools()
endif()
