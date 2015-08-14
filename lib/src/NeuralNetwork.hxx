//                                               -*- C++ -*-
/**
 *  @file  NeuralNetwork.hxx
 *  @brief The class NeuralNetwork creates a NumericalMathFunction from a neural network read in a PMML file
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
#ifndef OTPMML_NEURALNETWORK_HXX
#define OTPMML_NEURALNETWORK_HXX

#include <NumericalMathFunction.hxx>

#include "OTPMMLprivate.hxx"

namespace OTPMML
{

/**
 * @class NeuralNetwork
 *
 * The class NeuralNetwork creates a NumericalMathFunction from a neural network read in a PMML file.
 * If the PMML file contains several neural networks, the name of the desired neural network must
 * be provided.
 */
class OTPMML_API NeuralNetwork
  : public OT::NumericalMathFunction
{
  CLASSNAME;

public:
  /** Default constructor */
  NeuralNetwork(const OT::FileName & pmmlFile, const OT::String& modelName = "");

  /** String converter */
  virtual OT::String __repr__() const;

  /** String converter */
  virtual OT::String __str__(const OT::String & offset = "") const;

}; /* class PMMLNeuralNetwork */

} /* namespace OTPMML */

#endif /* OTPMML_NEURALNETWORK_HXX */
