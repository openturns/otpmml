//                                               -*- C++ -*-
/**
 *  @file  PMMLNeuralNetwork.hxx
 *  @brief Internal class to manage a <NeuralNetwork> element within a PMML document
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
#ifndef OTPMML_PMMLNEURALNETWORK_HXX
#define OTPMML_PMMLNEURALNETWORK_HXX

#include <Matrix.hxx>
#include <NumericalPoint.hxx>
#include <NumericalSample.hxx>
#include <Indices.hxx>
#include <NumericalMathFunction.hxx>

#include "OTPMMLprivate.hxx"

#include <libxml/tree.h>

namespace OTPMML
{

// Forward declaration
class PMMLDoc;

class OTPMML_API PMMLNeuralNetwork
{
public:
  /** Default constructor */
  PMMLNeuralNetwork(const PMMLDoc * pmml, const OT::String & modelName, const xmlNodePtr & node);

  /** Get model name */
  OT::String getModelName() const;

  /** Get number of inputs */
  OT::UnsignedInteger getNumberOfInputs() const;

  /** Get number of outputs */
  OT::UnsignedInteger getNumberOfOutputs() const;

  /** Get number of layers (= 1 + hidden) */
  OT::UnsignedInteger getNumberOfLayers() const;

  /** Get number of neurons in the given layer */
  OT::UnsignedInteger getLayerSize(OT::UnsignedInteger index) const;

  /** Get variable name associated to <NeuralInput> with given id attribute */
  OT::String getNeuralInputName(OT::UnsignedInteger id) const;

  /** Get bias of neurons in the given layer */
  OT::NumericalPoint getBiasAtLayer(OT::UnsignedInteger layerIndex) const;

  /** Get weights of neurons in the given layer; dimensions are size(layerIndex-1) x size(layerIndex) */
  OT::Matrix getWeightsAtLayer(OT::UnsignedInteger layerIndex) const;

  /** Get activation function name of the given layer */
  OT::String getActivationFunctionAtLayer(OT::UnsignedInteger layerIndex) const;

  /** Retrieve neuron ids of a given neural layer */
  OT::Indices getNeuronIdsAtLayer(OT::UnsignedInteger layerIndex) const;

  /** Get coefficients needed to normalize inputs */
  OT::NumericalSample getInputsNormalization() const;

  /** Get coefficients needed to normalize outputs */
  OT::NumericalSample getOutputsNormalization() const;

  /** Get input normalization as a NumericalMathFunction */
  OT::NumericalMathFunction getInputsNormalizationFunction() const;

  /** Get output normalization as a NumericalMathFunction */
  OT::NumericalMathFunction getOutputsNormalizationFunction() const;

  /** Get evaluation function of a given layer as a NumericalMathFunction */
  OT::NumericalMathFunction getEvaluationFunctionAtLayer(OT::UnsignedInteger layerIndex) const;

private:
  /** Set XPath context to node_ */
  void setXPathContext() const;

  /** Pointer to parent PMMLDoc instance */
  const PMMLDoc * pmml_;

  /** Model name */
  const OT::String modelName_;

  /** Pointer to the <NeuralNetwork> XML element */
  const xmlNodePtr node_;

}; /* class PMMLNeuralNetwork */

} /* namespace OTPMML */

#endif /* OTPMML_PMMLNEURALNETWORK_HXX */
