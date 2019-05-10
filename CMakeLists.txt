cmake_minimum_required (VERSION 2.8.8)

option ( BUILD_PYTHON                 "Build the python module for the library"                               ON )
option ( BUILD_DOC                    "Build the documentation"                                               ON )
option ( BUILD_SHARED_LIBS            "Build shared libraries"                                                ON )

# By default, build in Release mode. Must appear before project() command
if ( NOT DEFINED CMAKE_BUILD_TYPE )
   set( CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif ()

project (OTPMML)

string ( TOLOWER ${CMAKE_PROJECT_NAME} PACKAGE_NAME )

# set module dir to find custom scripts
list ( APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )

find_package (OpenTURNS NO_MODULE REQUIRED)
message (STATUS "Found OpenTURNS: ${OPENTURNS_ROOT_DIR} (found version \"${OPENTURNS_VERSION_STRING}\")")
include ( ${OPENTURNS_USE_FILE} )
set ( OTPMML_DEFINITIONS ${OPENTURNS_DEFINITIONS} )
if (NOT BUILD_SHARED_LIBS)
  list ( APPEND OTPMML_DEFINITIONS "-DOTPMML_STATIC" )
endif ()
add_definitions ( ${OTPMML_DEFINITIONS} )

set ( OTPMML_LIBRARY_PATH lib${LIB_SUFFIX} )
set ( OTPMML_INCLUDE_PATH include)
set ( OTPMML_SWIG_INCLUDE_DIRS ${OTPMML_INCLUDE_PATH}/${PACKAGE_NAME}/swig )
set ( OTPMML_DATA_PATH share )
set ( OTPMML_EXAMPLE_PATH ${OTPMML_DATA_PATH}/${PACKAGE_NAME}/examples )
set ( OTPMML_CONFIG_CMAKE_PATH lib${LIB_SUFFIX}/cmake/otpmml )
set (OTPMML_DOC_PATH share/${PACKAGE_NAME}/doc)

if (BUILD_PYTHON)
  # workaround for cmake bug #0013449
  if (CMAKE_VERSION VERSION_LESS 3 AND DEFINED CMAKE_FIND_ROOT_PATH)
    find_program (SWIG_EXECUTABLE NAMES swig2.0 swig)
    if (SWIG_EXECUTABLE)
      set (SWIG_USE_FILE ${CMAKE_ROOT}/Modules/UseSWIG.cmake)
      set (SWIG_FOUND TRUE)
    endif ()
  else ()
    find_package (SWIG 2.0.9)
  endif ()

  find_package (PythonInterp)
  find_package (PythonLibs)

  if (PYTHONINTERP_FOUND)
    if (DEFINED PYTHON_SITE_PACKAGES)
      set (OTPMML_PYTHON_MODULE_PATH ${PYTHON_SITE_PACKAGES})

    else ()
      execute_process ( COMMAND ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_python_lib(plat_specific=True, prefix='${CMAKE_INSTALL_PREFIX}'))"
                        OUTPUT_VARIABLE _ABS_PYTHON_MODULE_PATH
                        RESULT_VARIABLE _PYTHON_pythonlib_result
                        OUTPUT_STRIP_TRAILING_WHITESPACE )

      if (_PYTHON_pythonlib_result)
        message (SEND_ERROR "Could not run ${PYTHON_EXECUTABLE}")
      endif ()

      get_filename_component (_ABS_PYTHON_MODULE_PATH ${_ABS_PYTHON_MODULE_PATH} ABSOLUTE)
      file (RELATIVE_PATH OTPMML_PYTHON_MODULE_PATH ${CMAKE_INSTALL_PREFIX} ${_ABS_PYTHON_MODULE_PATH})
    endif ()

    if (IS_ABSOLUTE ${OTPMML_PYTHON_MODULE_PATH})
      set (PYTHON_MODULE_PATH ${OTPMML_PYTHON_MODULE_PATH})
    else ()
      set (PYTHON_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/${OTPMML_PYTHON_MODULE_PATH})
    endif ()

    set (OTPMML_PYTHON${PYTHON_VERSION_MAJOR}_MODULE_PATH ${OTPMML_PYTHON_MODULE_PATH})
  endif ( PYTHONINTERP_FOUND )
endif ()


# Define variables without OTPMML_ prefix with absolute paths
foreach (_var PYTHON_MODULE_PATH LIBRARY_PATH INCLUDE_PATH CONFIG_CMAKE_PATH)
  if (IS_ABSOLUTE "${OTPMML_${_var}}")
    set (${_var} ${OTPMML_${_var}})
  else ()
    set (${_var} ${CMAKE_INSTALL_PREFIX}/${OTPMML_${_var}})
  endif ()
  get_filename_component (${_var} "${${_var}}" ABSOLUTE)
  file (TO_NATIVE_PATH "${${_var}}" NATIVE_${_var})
  string (REPLACE "\\" "\\\\" NATIVE_${_var} ${NATIVE_${_var}})
endforeach ()

# RPATH settings
set (CMAKE_INSTALL_RPATH ${LIBRARY_PATH})
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
if (POLICY CMP0042)
  # MACOSX_RPATH is enabled by default
  cmake_policy(SET CMP0042 NEW)
endif()
if (POLICY CMP0078)
  # UseSWIG generates standard target names
  cmake_policy(SET CMP0078 NEW)
endif()
if (POLICY CMP0086)
  # UseSWIG honors SWIG_MODULE_NAME via -module flag
  cmake_policy(SET CMP0086 NEW)
endif()

# Some useful macros to ease CMakeLists.txt file writing
set ( SOURCEFILES "" CACHE INTERNAL "List of source files to compile" )
macro ( ot_add_source_file FILENAME )
  set ( sf ${SOURCEFILES} ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME} )
  set ( SOURCEFILES ${sf} CACHE INTERNAL "List of source files to compile" )
endmacro ( ot_add_source_file )

set ( HEADERFILES "" CACHE INTERNAL "List of header files to install" )
macro ( ot_install_header_file FILENAME )
  set ( hf ${HEADERFILES} ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}/${FILENAME} )
  set ( HEADERFILES ${hf} CACHE INTERNAL "List of header files to install" )
endmacro ( ot_install_header_file )

set ( SWIGFILES "" CACHE INTERNAL "List of SWIG files to install" )
macro ( ot_install_swig_file FILENAME )
  set ( hf ${SWIGFILES} ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME} )
  set ( SWIGFILES ${hf} CACHE INTERNAL "List of SWIG files to install" )
endmacro ( ot_install_swig_file )

set ( INTERNAL_INCLUDE_DIRS "" CACHE INTERNAL "List of directories with header files needed for build" )
macro ( ot_add_current_dir_to_include_dirs )
  set ( inc_dirs ${INTERNAL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} )
  set ( INTERNAL_INCLUDE_DIRS ${inc_dirs} CACHE INTERNAL "List of directories with header files needed for build" )
endmacro ( ot_add_current_dir_to_include_dirs )


set ( CPACK_PACKAGE_NAME          ${PACKAGE_NAME} )
set ( CPACK_PACKAGE_VERSION_MAJOR 1              )
set ( CPACK_PACKAGE_VERSION_MINOR 7              )
set ( CPACK_PACKAGE_VERSION_PATCH                )
set ( CPACK_SOURCE_GENERATOR      "TGZ;TBZ2"     )
set (CPACK_BINARY_STGZ    OFF CACHE BOOL "STGZ")
set (CPACK_BINARY_TBZ2    ON  CACHE BOOL "TBZ2")
set (CPACK_BINARY_TGZ     ON  CACHE BOOL "TGZ" )
set (CPACK_BINARY_TZ      OFF CACHE BOOL "TZ"  )
set ( CPACK_SOURCE_IGNORE_FILES "/.git;/build;.*~;${CPACK_SOURCE_IGNORE_FILES}" )
set ( PACKAGE_NAME           ${CPACK_PACKAGE_NAME} )
set ( PACKAGE_VERSION        ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR} )
if ( CPACK_PACKAGE_VERSION_PATCH )
  set ( PACKAGE_VERSION       ${PACKAGE_VERSION}.${CPACK_PACKAGE_VERSION_PATCH} )
endif ()
set ( CPACK_SOURCE_PACKAGE_FILE_NAME ${PACKAGE_NAME}-${PACKAGE_VERSION} )


# The tests can't be run if this function is absent
enable_testing ()
add_custom_target (tests COMMENT "Build tests")
add_custom_target (check COMMENT "Run pre-installation tests")
add_custom_target (installcheck COMMENT "Run post-installation tests")

add_subdirectory (lib)
add_dependencies (check cppcheck)

if (PYTHONINTERP_FOUND AND PYTHONLIBS_FOUND AND SWIG_FOUND)
  add_subdirectory (python)
  add_dependencies (installcheck pyinstallcheck)
endif ()

if (BUILD_DOC)
  add_subdirectory (doc)
endif ()

# uninstall target
configure_file (
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  IMMEDIATE @ONLY
)

add_custom_target ( uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

set ( OTPMML_LIBRARY                 otpmml )
set ( OTPMML_LIBRARIES ${OPENTURNS_LIBRARIES} )
list ( APPEND OTPMML_LIBRARIES ${OTPMML_LIBRARY} )
set ( OTPMML_LIBRARY_DIRS ${OPENTURNS_LIBRARY_DIRS} )
list ( APPEND OTPMML_LIBRARY_DIRS ${LIBRARY_PATH} )
set ( OTPMML_INCLUDE_DIR ${INCLUDE_PATH} )
set ( OTPMML_INCLUDE_DIRS ${OTPMML_INCLUDE_DIR} )
set ( OTPMML_ROOT_DIR       ${CMAKE_INSTALL_PREFIX} )
list ( APPEND OTPMML_INCLUDE_DIRS ${OPENTURNS_INCLUDE_DIRS} )
set ( OTPMML_VERSION_STRING ${PACKAGE_VERSION} )
set ( OTPMML_VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR} )
set ( OTPMML_VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR} )
set ( OTPMML_VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH} )
set ( OTPMML_USE_FILE      ${CONFIG_CMAKE_PATH}/UseOTPMML.cmake )

include(CMakePackageConfigHelpers)
configure_package_config_file (cmake/OTPMMLConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/OTPMMLConfig.cmake
  INSTALL_DESTINATION ${OTPMML_CONFIG_CMAKE_PATH}
  PATH_VARS
    OTPMML_USE_FILE
    OTPMML_ROOT_DIR
    OTPMML_INCLUDE_DIR
    OTPMML_INCLUDE_DIRS
    OTPMML_LIBRARY_DIRS
    PYTHON_MODULE_PATH
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file (OTPMMLConfigVersion.cmake VERSION ${OTPMML_VERSION_STRING} COMPATIBILITY AnyNewerVersion)

install (FILES cmake/UseOTPMML.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/OTPMMLConfig.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/OTPMMLConfigVersion.cmake
         DESTINATION ${OTPMML_CONFIG_CMAKE_PATH})

include (CPack)
