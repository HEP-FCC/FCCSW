CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

project(FCCSW)

include(GNUInstallDirs)

include(CTest)

add_subdirectory(FWCore)
add_subdirectory(Examples)

install(DIRECTORY k4_workflow_blocks DESTINATION python/)
