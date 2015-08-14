// SWIG file

%module(package="otpmml", docstring="otpmml module") otpmml
//%feature("autodoc","1");

%{
#include "OT.hxx"
#include "PythonWrappingFunctions.hxx"
%}

// Prerequisites needed
%include typemaps.i
%include exception.i

// Fix where to add this
// Because of OT::, the ignore defined in Types.i is not taken into account
%ignore *::load(OT::Advocate & adv);
%ignore *::save(OT::Advocate & adv) const;

%import base_module.i
%import uncertainty_module.i

// The new classes
%include OTPMMLprivate.hxx
%include DAT.i
%include RegressionModel.i
%include NeuralNetwork.i


