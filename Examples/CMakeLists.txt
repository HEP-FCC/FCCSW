

install(DIRECTORY options 
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/Examples
        PATTERN k4_workflow_blocks EXCLUDE
        )


file(GLOB fccsw_examples options/*.py)
list(FILTER fccsw_examples EXCLUDE REGEX "__init__.py")
foreach(opt ${fccsw_examples})
  get_filename_component(e ${opt} NAME)
  add_test(NAME ${e}
           WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/options
           COMMAND k4run ${e}
           )
   # make sure the workflow blocks are found
   set_property(TEST ${e} APPEND PROPERTY ENVIRONMENT
    PYTHONPATH=${PROJECT_SOURCE_DIR}:$ENV{PYTHONPATH}

    )

endforeach()

