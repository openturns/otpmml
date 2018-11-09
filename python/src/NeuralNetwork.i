// SWIG file NeuralNetwork.i

%{
#include "otpmml/NeuralNetwork.hxx"
%}

%include NeuralNetwork_doc.i

%include otpmml/NeuralNetwork.hxx
namespace OTPMML { %extend NeuralNetwork { NeuralNetwork(const NeuralNetwork & other) { return new OTPMML::NeuralNetwork(other); } } }
