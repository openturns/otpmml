#include <iostream>
// OT includes
#include <openturns/OT.hxx>

#include "PMMLDoc.hxx"
#include "PMMLNeuralNetwork.hxx"

using namespace OT;
using namespace OTPMML;

int main(int argc, char **argv)
{
  PMMLDoc doc("uranie_ann_poutre.pmml");
  doc.write("out.pmml");
  std::cout << "Number of regression models = " << doc.getNumberOfRegressionModels() << std::endl;
  std::cout << "Number of neural networks = " << doc.getNumberOfNeuralNetworks() << std::endl;

  PMMLNeuralNetwork nnet(doc.getNeuralNetwork("ANNPoutre"));
  std::cout << "Number of neural inputs = " << nnet.getNumberOfInputs() << std::endl;
  std::cout << "Number of neural outputs = " << nnet.getNumberOfOutputs() << std::endl;
  std::cout << "Number of layers = " << nnet.getNumberOfLayers() << std::endl;
  std::cout << "Size of layer 0 = " << nnet.getLayerSize(0) << std::endl;
  std::cout << "Size of layer 1 = " << nnet.getLayerSize(1) << std::endl;
  std::cout << "Size of layer 4 = " << nnet.getLayerSize(4) << std::endl;
  std::cout << "Name of neural input 2 = " << nnet.getNeuralInputName(2) << std::endl;
  std::cout << "Neuron ids of layer 0 = " << nnet.getNeuronIdsAtLayer(0) << std::endl;
  std::cout << "Neuron ids of layer 1 = " << nnet.getNeuronIdsAtLayer(1) << std::endl;
  std::cout << "Neuron ids of layer 2 = " << nnet.getNeuronIdsAtLayer(2) << std::endl;
  std::cout << "Bias of layer 0 = " << nnet.getBiasAtLayer(0) << std::endl;
  std::cout << "Bias of layer 1 = " << nnet.getBiasAtLayer(1) << std::endl;
  std::cout << "Bias of layer 2 = " << nnet.getBiasAtLayer(2) << std::endl;
  std::cout << "Weights of layer 0 = " << nnet.getWeightsAtLayer(0) << std::endl;
  std::cout << "Weights of layer 1 = " << nnet.getWeightsAtLayer(1) << std::endl;
  std::cout << "Weights of layer 2 = " << nnet.getWeightsAtLayer(2) << std::endl;
  std::cout << "Inputs normalization = " << nnet.getInputsNormalization() << std::endl;
  std::cout << "Outputs normalization = " << nnet.getOutputsNormalization() << std::endl;
  std::cout << "Evaluation function at layer 2 = " << nnet.getEvaluationFunctionAtLayer(2).__repr__() << std::endl;

  Point reseau(4);
  reseau[0] = 3.048020426e+00;
  reseau[1] = 4.429896757e+04;
  reseau[2] = 2.550648980e+02;
  reseau[3] = 3.711170093e+02;

  std::cout << "reseau=" << reseau << std::endl;
  Point normInput(nnet.getInputsNormalizationFunction()(reseau));
  std::cout << "normInput=" << normInput << std::endl;
  Point x0(nnet.getEvaluationFunctionAtLayer(0)(normInput));
  std::cout << "x0=" << x0 << std::endl;
  Point x1(nnet.getEvaluationFunctionAtLayer(1)(x0));
  std::cout << "x1=" << x1 << std::endl;
  Point y(nnet.getOutputsNormalizationFunction()(x1));
  std::cout << "y=" << y << std::endl;

  doc.reset();
  doc.write("clear.pmml");

  return 0;
}

