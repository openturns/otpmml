// SWIG file

%{
#include "NeuralNetwork.hxx"
%}

%include NeuralNetwork_doc.i

%include NeuralNetwork.hxx
namespace OTPMML { %extend NeuralNetwork { NeuralNetwork(const NeuralNetwork & other) { return new OTPMML::NeuralNetwork(other); } } }
