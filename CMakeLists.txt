CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

option(SAS "Whether to run static code analysis with SAS." OFF)
if (SAS)
    find_package(sas)
    enable_sas(
        FORMAT
        COMPARISON_REPORT_DIR "${CMAKE_BINARY_DIR}/sas_report"
        IGNORE_DIRS ${CMAKE_SOURCE_DIR}/build ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_SOURCE_DIR}/doc
        )
endif(SAS)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

configure_file(cmake/fccrun.in ${CMAKE_BINARY_DIR}/fccrun)
install(FILES ${CMAKE_BINARY_DIR}/fccrun
        DESTINATION .
        PERMISSIONS WORLD_EXECUTE WORLD_READ
        RENAME run)

gaudi_project(FCCSW HEAD
              USE Gaudi v28r2 )
