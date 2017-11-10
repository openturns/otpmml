#include <iostream>
#include <cmath>

// OT includes
#include <openturns/OT.hxx>

#include "DAT.hxx"

using namespace OT;
using namespace OTPMML;

int main(int argc, char **argv)
{
  Description variables(4);
  Description formula(1);
  variables[0] = "E";
  variables[1] = "F";
  variables[2] = "L";
  variables[3] = "I";
  formula[0] = "F*L^3/(3*E*I)";
  SymbolicFunction poutre(variables, formula);

  Collection<Sample> samples(DAT::Import("input_output.dat"));
  std::cout << "Input:" << samples[0] << std::endl;
  std::cout << "Output:" << samples[1] << std::endl;
  const Sample result(poutre(samples[0]));
  std::cout << "Analytical output:" << result << std::endl;
  for (UnsignedInteger i = 0; i < result.getSize(); ++i)
  {
    if (std::abs(result[i][0] - samples[1][i][0]) > 1.e-10 * std::abs(result[i][0]))
    {
      std::cout << "Significant differences found at index " << i << std::endl;
      break;
    }
  }

  DAT::Export("export.dat", samples[0], samples[1]);

  return 0;
}


