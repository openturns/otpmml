//                                               -*- C++ -*-
/**
 *  @file  NeuralNetwork.cxx
 *  @brief The class NeuralNetwork creates a Function from a neural network read in a PMML file
 *
 *  Copyright (C) 2014 EDF
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */
#include "NeuralNetwork.hxx"
#include "PMMLDoc.hxx"
#include "PMMLNeuralNetwork.hxx"

#include <libxml/parser.h>

using namespace OT;

namespace OTPMML
{

CLASSNAMEINIT(NeuralNetwork);

/* Default constructor */
NeuralNetwork::NeuralNetwork(const FileName & pmmlFile, const String& modelName)
  : Function()
{
  xmlInitParser();
  PMMLDoc doc(pmmlFile);
  PMMLNeuralNetwork nnet(doc.getNeuralNetwork(modelName));
  Function composedFunction(nnet.getInputsNormalizationFunction());
  for (UnsignedInteger layer = 0; layer < nnet.getNumberOfLayers(); ++layer)
    composedFunction = Function(nnet.getEvaluationFunctionAtLayer(layer), composedFunction);
  composedFunction = Function(nnet.getOutputsNormalizationFunction(), composedFunction);
  xmlCleanupParser();
  setName(nnet.getModelName());
  getImplementation().swap(composedFunction.getImplementation());
}

/* String converter */
String NeuralNetwork::__repr__() const
{
  return OSS(true) << "class=" << NeuralNetwork::GetClassName()
         << " name=" << getName()
         << " implementation=" << getImplementation()->__repr__();
}

/* String converter */
String NeuralNetwork::__str__(const String & offset) const
{
  return OSS(false) << offset << getClassName() << " :\n" << getImplementation()->__str__( offset + "  ");
}


} /* namespace OTPMML */
