

ot_add_current_dir_to_include_dirs ()

ot_add_source_file ( DAT.cxx )
ot_add_source_file ( RegressionModel.cxx )
ot_add_source_file ( NeuralNetwork.cxx )
ot_add_source_file ( PMMLDoc.cxx )
ot_add_source_file ( PMMLNeuralNetwork.cxx )
ot_add_source_file ( PMMLRegressionModel.cxx )

ot_install_header_file ( DAT.hxx )
ot_install_header_file ( RegressionModel.hxx )
ot_install_header_file ( NeuralNetwork.hxx )
ot_install_header_file ( PMMLDoc.hxx )
ot_install_header_file ( PMMLNeuralNetwork.hxx )
ot_install_header_file ( PMMLRegressionModel.hxx )


include_directories ( ${INTERNAL_INCLUDE_DIRS} )

add_library (otpmml ${SOURCEFILES})
set_target_properties (otpmml PROPERTIES POSITION_INDEPENDENT_CODE ON)
if (BUILD_SHARED_LIBS)
  set_target_properties (otpmml PROPERTIES COMPILE_DEFINITIONS "OTPMML_DLL_EXPORTS")
endif ()

if ( NOT DEFINED LIB_VERSION ) 
  set ( LIB_VERSION 0.0.0 )
endif ()
if ( NOT DEFINED LIB_SOVERSION ) 
  set ( LIB_SOVERSION 0 )
endif ()
set_target_properties ( otpmml PROPERTIES VERSION ${LIB_VERSION} )
set_target_properties ( otpmml PROPERTIES SOVERSION ${LIB_SOVERSION} )
target_link_libraries (otpmml ${OPENTURNS_LIBRARY})

# Add targets to the build-tree export set
export (TARGETS otpmml FILE ${PROJECT_BINARY_DIR}/OTTemplate-Targets.cmake)

# Install the export set for use with the install-tree
install(EXPORT OTTemplate-Targets
        DESTINATION "${OTPMML_CONFIG_CMAKE_PATH}"
        COMPONENT Development)

install(TARGETS otpmml
        EXPORT  OTTemplate-Targets 
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${OTPMML_LIBRARY_PATH}
        ARCHIVE DESTINATION ${OTPMML_LIBRARY_PATH}
)

