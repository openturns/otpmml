
include ( ${SWIG_USE_FILE} )

include_directories ( ${OPENTURNS_SWIG_INCLUDE_DIRS} )
add_definitions( ${OPENTURNS_SWIG_DEFINITIONS} )
include_directories ( ${INTERNAL_INCLUDE_DIRS} )
include_directories ( ${PYTHON_INCLUDE_DIRS} )

add_custom_target( generate_docstrings )

macro ( ot_add_python_module MODULENAME SOURCEFILE )
  set_source_files_properties ( ${SOURCEFILE} PROPERTIES CPLUSPLUS ON )
  set_source_files_properties ( ${SOURCEFILE} PROPERTIES SWIG_MODULE_NAME ${MODULENAME} )
  ot_install_swig_file ( ${SOURCEFILE} )
  if ( ${ARGC} GREATER 2 )
    set ( SWIG_MODULE_${MODULENAME}_EXTRA_DEPS ${ARGN} )
  endif ()

  set ( swig_other_sources )
  foreach ( it ${ARGN} )
    if ( ${it} MATCHES ".*\\.i$" )
      ot_install_swig_file ( ${it} )
    elseif(${it} MATCHES ".*_doc\\.i\\.in$")
      # handle docstring generation
      set (DOCSTRING_IN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${it})
      get_filename_component ( DOCSTRING_FILE_BASENAME ${DOCSTRING_IN_FILE} NAME_WE )
      add_custom_target ( generate_${DOCSTRING_FILE_BASENAME}
                          COMMAND ${CMAKE_COMMAND} -DDOCSTRING_IN_FILE=${DOCSTRING_IN_FILE} -DDOCSTRING_FILE=${CMAKE_CURRENT_BINARY_DIR}/${DOCSTRING_FILE_BASENAME}.i -P ${CMAKE_SOURCE_DIR}/cmake/escape_backslash_x4.cmake
                          DEPENDS ${DOCSTRING_IN_FILE} )
      add_dependencies ( generate_docstrings generate_${DOCSTRING_FILE_BASENAME} )
      list ( APPEND SWIGFILES ${CMAKE_CURRENT_BINARY_DIR}/${DOCSTRING_FILE_BASENAME}.i )
    else ()
      set ( swig_other_sources ${swig_other_sources} "${it}" )
    endif ()
  endforeach ()

  # swig_add_module is deprecated
  if (CMAKE_VERSION VERSION_LESS 3.8)
    swig_add_module (${MODULENAME} python ${SOURCEFILE} ${swig_other_sources} )
  else ()
    swig_add_library (${MODULENAME} LANGUAGE python SOURCES ${SOURCEFILE} ${swig_other_sources})
  endif ()

  add_dependencies ( ${SWIG_MODULE_${MODULENAME}_REAL_NAME} generate_docstrings )
  swig_link_libraries ( ${MODULENAME} otpmml )
  if ( LINK_PYTHON_LIBRARY )
    swig_link_libraries ( ${MODULENAME} ${PYTHON_LIBRARIES} )
  endif ()

  set_target_properties ( ${SWIG_MODULE_${MODULENAME}_REAL_NAME} PROPERTIES NO_SONAME ON )

  list ( APPEND PYTHON_MODULES ${MODULENAME} )
endmacro ( ot_add_python_module )



ot_add_python_module( ${PACKAGE_NAME} ${PACKAGE_NAME}_module.i 
                      DAT.i DAT_doc.i.in
                      RegressionModel.i RegressionModel_doc.i.in
                      NeuralNetwork.i NeuralNetwork_doc.i.in
                    )


foreach ( module ${PYTHON_MODULES} )
  install ( FILES ${CMAKE_CURRENT_BINARY_DIR}/${module}.py
            DESTINATION ${OTPMML_PYTHON_MODULE_PATH}/${PACKAGE_NAME}
            )
  install ( TARGETS _${module}
            LIBRARY DESTINATION ${OTPMML_PYTHON_MODULE_PATH}/${PACKAGE_NAME}
          )
endforeach ( module )

install ( FILES __init__.py
          DESTINATION ${OTPMML_PYTHON_MODULE_PATH}/${PACKAGE_NAME}
        )

install ( FILES ${SWIGFILES}
          DESTINATION ${OTPMML_SWIG_INCLUDE_PATH}
        )


