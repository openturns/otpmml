
if (MINGW AND CMAKE_SIZEOF_VOID_P EQUAL 4)
  add_library (CRT_fp8 STATIC CRT_fp8.c)
endif ()

set (CHECK_ENVIRONMENT)

set ( CHECK_TO_BE_RUN )
macro ( ot_check_test TESTNAME )
  set ( TESTNAME_TGT t_${TESTNAME} )
  set ( TESTNAME_SRC ${TESTNAME_TGT}.cxx )
  set ( TESTNAME_SRCS ${TESTNAME_SRC} )
  add_executable ( ${TESTNAME_TGT} EXCLUDE_FROM_ALL ${TESTNAME_SRCS} )
  add_dependencies ( tests ${TESTNAME_TGT} )
  target_link_libraries ( ${TESTNAME_TGT} otpmml )
  if (MINGW AND CMAKE_SIZEOF_VOID_P EQUAL 4)
    target_link_libraries (${TESTNAME_TGT} CRT_fp8)
  endif ()
  set_target_properties ( ${TESTNAME_TGT} PROPERTIES
                                          UNITY_BUILD OFF
                                          INSTALL_RPATH "${CMAKE_BINARY_DIR}/lib/src;${CMAKE_INSTALL_RPATH}" )
  set ( TESTNAME_LOC ${CMAKE_CURRENT_BINARY_DIR}/${TESTNAME_TGT}${CMAKE_EXECUTABLE_SUFFIX} ) 
  set ( _CMD )
  set ( _PRE )
  set ( _POST )
  set ( _PARAMS )
  set ( _IGNOREOUT OFF )
  foreach ( _ARG ${ARGN} )
    string ( TOUPPER ${_ARG} ARG )
    if ( ${ARG} MATCHES PRE )
      set ( _CMD PRE )
    elseif ( ${ARG} MATCHES POST )
      set ( _CMD POST )
    elseif ( ${ARG} MATCHES PARAMS )
      set ( _CMD PARAMS )
    elseif ( ${ARG} MATCHES IGNOREOUT )
      set ( _IGNOREOUT ON )
    else ( ${ARG} MATCHES PRE )
      if ( ${_CMD} MATCHES PRE )
        set ( _PRE "${_PRE} ${_ARG} && " )
      elseif ( ${_CMD} MATCHES POST )
        set ( _POST "${_POST} && ${_ARG}" )
      elseif ( ${_CMD} MATCHES PARAMS )
        set ( _PARAMS "${_PARAMS} ${_ARG}" )
      endif  ( ${_CMD} MATCHES PRE )
      set ( _CMD )
    endif ( ${ARG} MATCHES PRE )
  endforeach ( _ARG )

  if (_IGNOREOUT)
    set ( COMMAND "( ${CMAKE_CROSSCOMPILING_EMULATOR} ${TESTNAME_LOC} ${_PARAMS} > /dev/null )" )
  else (_IGNOREOUT)
    set ( TESTNAME_OUT ${TESTNAME_TGT}.expout )
    set ( OUTFILE_LOC ${CMAKE_CURRENT_SOURCE_DIR}/${TESTNAME_OUT} ) 
    set ( COMMAND "( ${CMAKE_CROSSCOMPILING_EMULATOR} ${TESTNAME_LOC} ${_PARAMS} > ${CMAKE_CURRENT_BINARY_DIR}/${TESTNAME_TGT}.out ) && diff -u --strip-trailing-cr ${OUTFILE_LOC} ${CMAKE_CURRENT_BINARY_DIR}/${TESTNAME_TGT}.out" )
  endif (_IGNOREOUT)

  if (_PRE OR _POST)
    set ( COMMAND "${_PRE}${COMMAND}${_POST}" )
  endif (_PRE OR _POST)

  add_test ( cppcheck_${TESTNAME} "sh" "-c" "${COMMAND}" )
  set_tests_properties ( cppcheck_${TESTNAME} PROPERTIES ENVIRONMENT "${CHECK_ENVIRONMENT}" )
  list ( APPEND CHECK_TO_BE_RUN ${TESTNAME_TGT} )
endmacro ()

include_directories ( ${INTERNAL_INCLUDE_DIRS} )



ot_check_test ( PMMLDoc_std )
ot_check_test ( PMMLRegressionModel )
ot_check_test ( NeuralNetwork_std )
ot_check_test ( DAT_std )

add_custom_target ( cppcheck COMMAND ${CMAKE_CTEST_COMMAND} -R "^cppcheck_"
                    DEPENDS ${CHECK_TO_BE_RUN}
                    COMMENT "Run C++ pre-installation tests" )

execute_process (
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/validation/input_output.dat ${CMAKE_CURRENT_BINARY_DIR}/input_output.dat
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/validation/uranie_ann_poutre.pmml ${CMAKE_CURRENT_BINARY_DIR}/uranie_ann_poutre.pmml
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/validation/linear_regression.pmml ${CMAKE_CURRENT_BINARY_DIR}/linear_regression.pmml
)
