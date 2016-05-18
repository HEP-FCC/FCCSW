# Special toolchain file that inherits the same heptools version as the
# used projects.
find_file(inherit_heptools_module InheritHEPTools.cmake)

# this check is needed because the toolchain seem to be called a second time
# without the proper cache
if(inherit_heptools_module)
  include(${inherit_heptools_module})
  inherit_heptools()
endif()
