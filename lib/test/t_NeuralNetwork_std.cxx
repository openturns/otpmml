#include <iostream>
// OT includes
#include <openturns/OT.hxx>

#include "DAT.hxx"
#include "PMMLDoc.hxx"
#include "PMMLNeuralNetwork.hxx"
#include "NeuralNetwork.hxx"

using namespace OT;
using namespace OTPMML;

int main(int argc, char **argv)
{
  Point reseau(4);
  reseau[0] = 3.048020426e+00;
  reseau[1] = 4.429896757e+04;
  reseau[2] = 2.550648980e+02;
  reseau[3] = 3.711170093e+02;

  Function annFunction = NeuralNetwork("uranie_ann_poutre.pmml");
  std::cout << annFunction(reseau) << " expected value=46.5277" << std::endl;

  return 0;
}

