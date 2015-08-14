// SWIG file

%{
#include "RegressionModel.hxx"
%}

%include RegressionModel_doc.i

%include RegressionModel.hxx
namespace OTPMML { %extend RegressionModel { RegressionModel(const RegressionModel & other) { return new OTPMML::RegressionModel(other); } } }
