# Download MathJax

message(STATUS "Downloading MathJax...")
set(MathJax_url https://github.com/mathjax/MathJax/archive/master.tar.gz)
set(MathJax_dst ${TARFILE_DEST}/MathJax.tar.gz)
if(CMAKE_VERSION VERSION_LESS 2.8.12)
  find_program(CURL_CMD curl)
  if(CURL_CMD)
    execute_process(COMMAND curl --location --output ${MathJax_dst} ${MathJax_url})
  else()
    find_program(WGET_CMD wget)
    if(WGET_CMD wget)
      execute_process(COMMAND wget -O ${MathJax_dst} ${MathJax_url})
    else()
      message(FATAL_ERROR "Cannot find a way of downloading MathJax archive.")
    endif()
  endif()
else()
  file(DOWNLOAD ${MathJax_url} ${MathJax_dst})
endif()

message(STATUS "Unpacking MathJax...")
file(MAKE_DIRECTORY ${INSTALL_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf ${TARFILE_DEST}/MathJax.tar.gz
                WORKING_DIRECTORY ${INSTALL_DIR})
file(RENAME ${INSTALL_DIR}/MathJax-master ${INSTALL_DIR}/mathjax)

message(STATUS "MathJax installed.")
