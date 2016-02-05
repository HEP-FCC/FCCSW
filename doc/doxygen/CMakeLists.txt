find_package(Doxygen)

if(DOXYGEN_FOUND)
  # directories used in the include search path during compilation
  set(DOXYGEN_INCLUDE_DIRS)
  foreach(package ${packages})
    set(DOXYGEN_INCLUDE_DIRS "${DOXYGEN_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/${package}")
  endforeach()

  # For release.note inclusion check Gaud/GaudiRelease/doc/doxygen/CMakeLists ll. 10-43
  # removed from this since we don't have any, yet..

  # run Doxygen to generate the documentation
  add_custom_target(run-doxygen
                    COMMAND ${env_cmd} --xml ${env_xml}
                            ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                    COMMENT "Running Doxygen...")

  # 'doc' target
  add_custom_target(doc DEPENDS run-doxygen)

  option(DOXYGEN_WITH_LOCAL_MATHJAX
         "Use a local copy of MathJax instead of taking it from cdn.mathjax.org"
         FALSE)

  if(DOXYGEN_WITH_LOCAL_MATHJAX)
    # install MathJax Javascript math renderer
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/doxygen/mathjax
                       COMMAND ${CMAKE_COMMAND}
                         -DTARFILE_DEST=${CMAKE_CURRENT_BINARY_DIR}
                         -DINSTALL_DIR=${CMAKE_BINARY_DIR}/doxygen
                         -P ${CMAKE_CURRENT_SOURCE_DIR}/get_mathjax.cmake
                       COMMENT "Installing MathJax...")
    # in principle this is not needed, but it allows to run Doxygen and install
    # MathJax at the same time
    add_custom_target(install-MathJax DEPENDS ${CMAKE_BINARY_DIR}/doxygen/mathjax)
    add_dependencies(doc install-MathJax)
    set(MATHJAX_RELPATH "../mathjax")
  else()
    set(MATHJAX_RELPATH "http://cdn.mathjax.org/mathjax/latest")
  endif(DOXYGEN_WITH_LOCAL_MATHJAX)

  # massage some version numbers
  string(REGEX REPLACE "_python.*" "" Boost_version "${Boost_config_version}")
  string(REGEX REPLACE "\\." "_" Boost_url_version "${Boost_version}")
  string(REGEX REPLACE "\\..*" "" XercesC_major_version "${XercesC_config_version}")

  # copy the template files
  foreach(template Doxyfile mainpage.md externaldocs.md)
    configure_file(${template}.in ${CMAKE_CURRENT_BINARY_DIR}/${template} @ONLY)
  endforeach()

  configure_file(doxy-boot.js.in ${CMAKE_BINARY_DIR}/doxygen/html/doxy-boot.js)

  # the cmake directory is not scanned
  foreach(doc_file)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/${doc_file}
                   ${CMAKE_CURRENT_BINARY_DIR}/cmake_doc/${doc_file} COPYONLY)
  endforeach()

endif()
