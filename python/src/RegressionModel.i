// SWIG file RegressionModel.i

%{
#include "otpmml/RegressionModel.hxx"
%}

%include RegressionModel_doc.i

%include otpmml/RegressionModel.hxx
namespace OTPMML { %extend RegressionModel { RegressionModel(const RegressionModel & other) { return new OTPMML::RegressionModel(other); } } }
