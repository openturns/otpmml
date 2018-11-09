
find_package ( LATEX )
set ( IMAGEMAGICK_CONVERT convert )
set ( LATEX2HTML_CONVERTER latex2html )
include ( UseLATEX )

# latex part
if ( PDFLATEX_COMPILER AND BIBTEX_COMPILER )

  if ( ${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR} )
    # in-source build
    set ( LATEX_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build )
#     execute_process (
#       COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/src/logoOpenTURNS.jpg ${CMAKE_CURRENT_SOURCE_DIR}/logoOpenTURNS.jpg
#     )
  else ()
    # out-of-source build
    set ( LATEX_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
  endif ()
  add_latex_document ( OTPMML_Documentation.tex
    INPUTS logoOTPMML.png GenericInformation.tex.in Math_Notations.sty OTPMML_Documentation.sty
           OTPMML_summary.tex OTPMML_title.tex OTPMML_ArchitectureGuide.tex OTPMML_UserManual.tex OTPMML_ReferenceGuide.tex OTPMML_UseCasesGuide.tex OTPMML_Validation.tex
           DAT.png NeuralNetwork.png RegressionModel.png
           PMMLDoc.png PMMLNeuralNetwork.png PMMLRegressionModel.png absolute_relative_errors.png comparaison_outputs.png
           perceptron.png single_layer.png
    DEFAULT_PDF
    MANGLE_TARGET_NAMES
  )

  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/GenericInformation.tex.in
                  ${LATEX_OUTPUT_PATH}/GenericInformation.tex
                  @ONLY
  )

   execute_process (
     COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/validation/input_output.dat ${LATEX_OUTPUT_PATH}/input_output.dat
     COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/validation/compare_otpmml.py ${LATEX_OUTPUT_PATH}/compare_otpmml.py
     COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/validation/compare_otpmml.out ${LATEX_OUTPUT_PATH}/compare_otpmml.out
   )
  install ( FILES ${LATEX_OUTPUT_PATH}/OTPMML_Documentation.pdf DESTINATION ${OTPMML_DATA_PATH}/doc/otpmml/pdf )

endif ()
