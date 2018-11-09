#include <iostream>
// OT includes
#include <openturns/OT.hxx>

#include "otpmml/PMMLDoc.hxx"
#include "otpmml/PMMLRegressionModel.hxx"
#include "otpmml/RegressionModel.hxx"

using namespace OT;
using namespace OTPMML;

int main(int argc, char **argv)
{
  PMMLDoc doc("linear_regression.pmml");
  std::cout << "Number of regression models = " << doc.getNumberOfRegressionModels() << std::endl;
  std::cout << "Number of neural networks = " << doc.getNumberOfNeuralNetworks() << std::endl;

  PMMLRegressionModel rm(doc.getRegressionModel());
  std::cout << "Coefficients = " << rm.getCoefficients() << std::endl;

  LinearLeastSquares leastSquares(RegressionModel("linear_regression.pmml").getLinearLeastSquares());
  std::cout << "Linear regression:" << std::endl;
  std::cout << "   constant = " << leastSquares.getConstant() << std::endl;
  std::cout << "   linear = " << leastSquares.getLinear() << std::endl;

  return 0;
}

