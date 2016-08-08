//                                               -*- C++ -*-
/**
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
#include "PMMLNeuralNetwork.hxx"
#include "PMMLDoc.hxx"

#include <openturns/Log.hxx>
#include <openturns/Exception.hxx>

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace OT;

namespace OTPMML
{

/* Default constructor */
PMMLNeuralNetwork::PMMLNeuralNetwork(const PMMLDoc * pmml, const String & modelName, const xmlNodePtr & node)
  : pmml_(pmml)
  , modelName_(modelName)
  , node_(node)
{
}

/** Get model name */
String PMMLNeuralNetwork::getModelName() const
{
  return modelName_;
}

/** Set XPath context to node_ */
void PMMLNeuralNetwork::setXPathContext() const
{
  pmml_->checkInitialized();
  pmml_->xpathContext_->node = node_;
}

/** Get number of inputs */
UnsignedInteger PMMLNeuralNetwork::getNumberOfInputs() const
{
  setXPathContext();
  return pmml_->getXPathQueryScalar(String("number(./")+pmml_->xpathNsPrefix_+String("NeuralInputs/@numberOfInputs)"));
}

/** Get number of outputs */
UnsignedInteger PMMLNeuralNetwork::getNumberOfOutputs() const
{
  setXPathContext();
  return pmml_->getXPathQueryScalar(String("number(./")+pmml_->xpathNsPrefix_+String("NeuralOutputs/@numberOfOutputs)"));
}

/** Get number of layers (= 1 + hidden) */
UnsignedInteger PMMLNeuralNetwork::getNumberOfLayers() const
{
  setXPathContext();
  return pmml_->getXPathQueryScalar("number(./@numberOfLayers)");
}

/** Get number of neurons in the given layer */
UnsignedInteger PMMLNeuralNetwork::getLayerSize(UnsignedInteger index) const
{
  setXPathContext();
  String query(OSS() << "number(./" << pmml_->xpathNsPrefix_ << "NeuralLayer[" << (1 + index) << "]/@numberOfNeurons)");
  return pmml_->getXPathQueryScalar(query);
}

/** Get variable name associated to <NeuralInput> with given id attribute */
String PMMLNeuralNetwork::getNeuralInputName(UnsignedInteger id) const
{
  setXPathContext();
  String query(OSS() << "./" << pmml_->xpathNsPrefix_ << "NeuralInputs/" << "NeuralInput[@id=" << id << "]/*/*/@field");
  return pmml_->getXPathQueryString(query);
}

/** Get bias of neurons in the given layer */
NumericalPoint PMMLNeuralNetwork::getBiasAtLayer(UnsignedInteger layerIndex) const
{
  NumericalPoint result(getLayerSize(layerIndex));
  setXPathContext();
  String query(OSS() << "./" << pmml_->xpathNsPrefix_ << "NeuralLayer[" << (1 + layerIndex) << "]/" << pmml_->xpathNsPrefix_ << "Neuron/@bias");
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST query.c_str(), pmml_->xpathContext_);
  if(!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval) && static_cast<UnsignedInteger>(xpathObj->nodesetval->nodeNr) == result.getDimension())
  {
    for (int cnt = 0; cnt < xpathObj->nodesetval->nodeNr; ++cnt)
    {
      result[cnt] = strtod(reinterpret_cast<const char*>(xpathObj->nodesetval->nodeTab[cnt]->children->content), NULL);
    }
  }
  xmlXPathFreeObject(xpathObj);
  return result;
}

/** Get weights of neurons in the given layer; dimensions are size(layerIndex-1) x size(layerIndex) */
Matrix PMMLNeuralNetwork::getWeightsAtLayer(UnsignedInteger layerIndex) const
{
  UnsignedInteger from, to;
  Indices fromIndices, toIndices;
  if (layerIndex == 0)
  {
    from = getNumberOfInputs();
    fromIndices = Indices(from);
    for (UnsignedInteger i = 0; i < from; ++i)
      fromIndices[i] = i;
    to = getLayerSize(0);
    toIndices = getNeuronIdsAtLayer(0);
  } else if (layerIndex < getNumberOfLayers()) {
    from = getLayerSize(layerIndex - 1);
    fromIndices = getNeuronIdsAtLayer(layerIndex - 1);
    to = getLayerSize(layerIndex);
    toIndices = getNeuronIdsAtLayer(layerIndex);
  } else {
    return Matrix();
  }
  if (fromIndices.isEmpty() || toIndices.isEmpty())
    return Matrix();
  // Build inverse transformation on indices
  UnsignedInteger maxFrom(1 + *std::max_element(fromIndices.begin(), fromIndices.end()));
  Indices invFromIndices(maxFrom);
  for (UnsignedInteger i = 0; i < from; ++i)
    invFromIndices[fromIndices[i]] = i;
  UnsignedInteger maxTo(1 + *std::max_element(toIndices.begin(), toIndices.end()));
  Indices invToIndices(maxTo);
  for (UnsignedInteger i = 0; i < to; ++i)
    invToIndices[toIndices[i]] = i;

  Matrix result(from, to);
  setXPathContext();
  String query(OSS() << "./" << pmml_->xpathNsPrefix_ << "NeuralLayer[" << (1 + layerIndex) << "]/" << pmml_->xpathNsPrefix_ << "Neuron");
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST query.c_str(), pmml_->xpathContext_);
  if(!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval) && static_cast<UnsignedInteger>(xpathObj->nodesetval->nodeNr) == to)
  {
    for (int cnt = 0; cnt < xpathObj->nodesetval->nodeNr; ++cnt)
    {
      // Get neuron id
      UnsignedInteger toId(0);
      for (xmlAttr *cur_attr = xpathObj->nodesetval->nodeTab[cnt]->properties;
           cur_attr != NULL; cur_attr = cur_attr->next)
      {
        if (cur_attr->type == XML_ATTRIBUTE_NODE && 0 == xmlStrcmp(cur_attr->name, BAD_CAST "id"))
        {
            toId = strtol(reinterpret_cast<const char*>(cur_attr->children->content), NULL, 10);
            break;
        }
      }
      toId = invToIndices[toId];
      // Get connections
      for (xmlNode *cur_node = xpathObj->nodesetval->nodeTab[cnt]->children; cur_node != NULL; cur_node = cur_node->next)
      {
        if (cur_node->type == XML_ELEMENT_NODE && 0 == xmlStrcmp(cur_node->name, BAD_CAST "Con"))
        {
          UnsignedInteger countAttributes(0);
          UnsignedInteger fromId(0);
          NumericalScalar weight(0.0);
          for (xmlAttr *cur_attr = cur_node->properties;
               cur_attr != NULL; cur_attr = cur_attr->next)
          {
            if (cur_attr->type != XML_ATTRIBUTE_NODE) continue;
            if (0 == xmlStrcmp(cur_attr->name, BAD_CAST "from"))
            {
                fromId = strtol(reinterpret_cast<const char*>(cur_attr->children->content), NULL, 10);
                ++ countAttributes;
            }
            else if (0 == xmlStrcmp(cur_attr->name, BAD_CAST "weight"))
            {
                weight = strtod(reinterpret_cast<const char*>(cur_attr->children->content), NULL);
                ++ countAttributes;
            }
          }
          if (countAttributes != 2) continue;
          fromId = invFromIndices[fromId];
          result(fromId, toId) = weight;
        }
      }
    }
  }
  xmlXPathFreeObject(xpathObj);
  return result;
}

/** Get activation function name of the given layer */
String PMMLNeuralNetwork::getActivationFunctionAtLayer(UnsignedInteger layerIndex) const
{
  setXPathContext();
  String query(OSS() << "./" << pmml_->xpathNsPrefix_ << "NeuralLayer[" << (1 + layerIndex) << "]/@activationFunction");
  return pmml_->getXPathQueryString(query);
}

/** Retrieve neuron ids of a given neural layer */
Indices PMMLNeuralNetwork::getNeuronIdsAtLayer(UnsignedInteger layerIndex) const
{
  Indices result;
  UnsignedInteger numberOfNeurons(getLayerSize(layerIndex));
  std::vector<UnsignedInteger> ids(numberOfNeurons);
  setXPathContext();
  String query(OSS() << "./" << pmml_->xpathNsPrefix_ << "NeuralLayer[" << (1 + layerIndex) << "]/" << pmml_->xpathNsPrefix_ << "Neuron/@id");
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST query.c_str(), pmml_->xpathContext_);
  if(!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval) && static_cast<UnsignedInteger>(xpathObj->nodesetval->nodeNr) == numberOfNeurons)
  {
    for (int cnt = 0; cnt < xpathObj->nodesetval->nodeNr; ++cnt)
    {
      UnsignedInteger id = strtol(reinterpret_cast<const char*>(xpathObj->nodesetval->nodeTab[cnt]->children->content), NULL, 10);
      result.add(id);
    }
  }
  xmlXPathFreeObject(xpathObj);
  return result;
}

/** Get coefficients needed to normalize inputs */
NumericalSample PMMLNeuralNetwork::getInputsNormalization() const
{
  const UnsignedInteger nrInputs(getNumberOfInputs());
  NumericalSample result(nrInputs, 4);
  Description description(4);
  description[0] = "orig0";
  description[1] = "orig1";
  description[2] = "norm0";
  description[3] = "norm1";
  result.setDescription(description);

  setXPathContext();
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST (String("./")+pmml_->xpathNsPrefix_+String("NeuralInputs/")+pmml_->xpathNsPrefix_+String("NeuralInput/")+pmml_->xpathNsPrefix_+String("DerivedField/")+pmml_->xpathNsPrefix_+String("NormContinuous/")+pmml_->xpathNsPrefix_+String("LinearNorm/@orig")).c_str(), pmml_->xpathContext_);
  if(!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval) && static_cast<UnsignedInteger>(xpathObj->nodesetval->nodeNr) == 2*nrInputs)
  {
    for (int cnt = 0; cnt < xpathObj->nodesetval->nodeNr; ++cnt)
    {
      result(cnt / 2, cnt & 1) = strtod(reinterpret_cast<const char*>(xpathObj->nodesetval->nodeTab[cnt]->children->content), NULL);
    }
  }
  xmlXPathFreeObject(xpathObj);
  setXPathContext();
  xpathObj = xmlXPathEvalExpression(BAD_CAST (String("./")+pmml_->xpathNsPrefix_+String("NeuralInputs/")+pmml_->xpathNsPrefix_+String("NeuralInput/")+pmml_->xpathNsPrefix_+String("DerivedField/")+pmml_->xpathNsPrefix_+String("NormContinuous/")+pmml_->xpathNsPrefix_+String("LinearNorm/@norm")).c_str(), pmml_->xpathContext_);
  if(!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval) && static_cast<UnsignedInteger>(xpathObj->nodesetval->nodeNr) == 2*nrInputs)
  {
    for (int cnt = 0; cnt < xpathObj->nodesetval->nodeNr; ++cnt)
    {
      result(cnt / 2, 2 + (cnt & 1)) = strtod(reinterpret_cast<const char*>(xpathObj->nodesetval->nodeTab[cnt]->children->content), NULL);
    }
  }
  xmlXPathFreeObject(xpathObj);

  // Check that all normalizations are consistent
  UnsignedInteger normalizationType = 0;
  for (UnsignedInteger i = 0; i < result.getSize(); ++i)
  {
    UnsignedInteger currentNormType(0);
    if (result(i, 0) == 0.0 && result(i, 3) == 0.0)
    {
      // Case 1:
      //   orig0 = 0;
      //   orig1 = dmin;
      //   norm0 = -dmax/dmin;
      //   norm1 = 0;
      currentNormType = 1;
    }
    else if (result(i, 2) == -1.0 && result(i, 3) == 1.0)
    {
      // Case 2:
      //   orig0 = dmin;
      //   orig1 = dmax;
      //   norm0 = -1;
      //   norm1 = 1;
      currentNormType = 2;
    }
    else
    {
      LOGWARN("Unknown method for inputs normalization, results may be wrong");
      break;
    }
    if (normalizationType != 0 && normalizationType != currentNormType)
    {
      LOGWARN("Unknown method for inputs normalization, results may be wrong");
      break;
    }
    normalizationType = currentNormType;
  }

  return result;
}

/** Get coefficients needed to normalize outputs */
NumericalSample PMMLNeuralNetwork::getOutputsNormalization() const
{
  const UnsignedInteger nrOutputs(getNumberOfOutputs());
  NumericalSample result(nrOutputs, 4);
  Description description(4);
  description[0] = "orig0";
  description[1] = "orig1";
  description[2] = "norm0";
  description[3] = "norm1";
  result.setDescription(description);

  setXPathContext();
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST (String("./")+pmml_->xpathNsPrefix_+String("NeuralOutputs/")+pmml_->xpathNsPrefix_+String("NeuralOutput/")+pmml_->xpathNsPrefix_+String("DerivedField/")+pmml_->xpathNsPrefix_+String("NormContinuous/")+pmml_->xpathNsPrefix_+String("LinearNorm/@orig")).c_str(), pmml_->xpathContext_);
  if(!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval) && static_cast<UnsignedInteger>(xpathObj->nodesetval->nodeNr) == 2*nrOutputs)
  {
    for (int cnt = 0; cnt < xpathObj->nodesetval->nodeNr; ++cnt)
    {
      result(cnt / 2, cnt & 1) = strtod(reinterpret_cast<const char*>(xpathObj->nodesetval->nodeTab[cnt]->children->content), NULL);
    }
  }
  xmlXPathFreeObject(xpathObj);
  setXPathContext();
  xpathObj = xmlXPathEvalExpression(BAD_CAST (String("./")+pmml_->xpathNsPrefix_+String("NeuralOutputs/")+pmml_->xpathNsPrefix_+String("NeuralOutput/")+pmml_->xpathNsPrefix_+String("DerivedField/")+pmml_->xpathNsPrefix_+String("NormContinuous/")+pmml_->xpathNsPrefix_+String("LinearNorm/@norm")).c_str(), pmml_->xpathContext_);
  if(!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval) && static_cast<UnsignedInteger>(xpathObj->nodesetval->nodeNr) == 2*nrOutputs)
  {
    for (int cnt = 0; cnt < xpathObj->nodesetval->nodeNr; ++cnt)
    {
      result(cnt / 2, 2 + (cnt & 1)) = strtod(reinterpret_cast<const char*>(xpathObj->nodesetval->nodeTab[cnt]->children->content), NULL);
    }
  }
  xmlXPathFreeObject(xpathObj);

  // Check that all normalizations are consistent
  UnsignedInteger normalizationType = 0;
  for (UnsignedInteger i = 0; i < result.getSize(); ++i)
  {
    UnsignedInteger currentNormType(0);
    if (result(i, 0) == 0.0 && result(i, 3) == 0.0)
    {
      // Case 1:
      //   orig0 = 0;
      //   orig1 = dmin;
      //   norm0 = -dmax/dmin;
      //   norm1 = 0;
      currentNormType = 1;
    }
    else if (result(i, 2) == -1.0 && result(i, 3) == 1.0)
    {
      // Case 2:
      //   orig0 = dmin;
      //   orig1 = dmax;
      //   norm0 = -1;
      //   norm1 = 1;
      currentNormType = 2;
    }
    else
    {
      LOGWARN("Unknown method for outputs normalization, results may be wrong");
      break;
    }
    if (normalizationType != 0 && normalizationType != currentNormType)
    {
      LOGWARN("Unknown method for outputs normalization, results may be wrong");
      break;
    }
    normalizationType = currentNormType;
  }


  return result;
}

/** Get input normalization as a NumericalMathFunction */
NumericalMathFunction PMMLNeuralNetwork::getInputsNormalizationFunction() const
{
  // See http://www.dmg.org/v3-0/Transformations.html
  const NumericalSample input(getInputsNormalization());
  const UnsignedInteger dimension(input.getSize());

  Description inputVariablesNames(dimension);
  Description formulas(dimension);
  if (dimension == 0)
    return NumericalMathFunction(inputVariablesNames, formulas);
  if (input[0][3] == 0.0)
  {
    // Case 1:
    //   orig0 = 0;
    //   orig1 = dmin;
    //   norm0 = -dmax/dmin;
    //   norm1 = 0;
    // y = (x - dmin) / dmax = - (x - orig1) / (orig1 * norm0)
    for (UnsignedInteger d = 0; d < dimension; ++d)
    {
      inputVariablesNames[d] = (OSS() << "x" << d);
      Bool minus = (- input[d][1] * input[d][2] < 0.0);
      formulas[d] = (OSS().setPrecision(20) << (minus ? "- " : "" ) << "(" << inputVariablesNames[d] << (input[d][1] < 0.0 ? " + " : " - ") << std::abs(input[d][1]) << ") / " << std::abs(input[d][1] * input[d][2]));
    }
  } else {
    // Case 2:
    //   orig0 = dmin;
    //   orig1 = dmax;
    //   norm0 = -1;
    //   norm1 = 1;
    // y = 2.0 * (x - dmin) / (dmax - dmin) - 1
    for (UnsignedInteger d = 0; d < dimension; ++d)
    {
      inputVariablesNames[d] = (OSS() << "x" << d);
      Bool minus = (input[d][1] < input[d][0]);
      formulas[d] = (OSS().setPrecision(20) << (minus ? "- " : "" ) << " (" << inputVariablesNames[d] << (input[d][0] < 0.0 ? " + " : " - ") << std::abs(input[d][0]) << ") / " << (0.5 * std::abs(input[d][1] - input[d][0])) << " - 1.0");
    }
  }
  return NumericalMathFunction(inputVariablesNames, formulas);
}

/** Get output normalization as a NumericalMathFunction */
NumericalMathFunction PMMLNeuralNetwork::getOutputsNormalizationFunction() const
{
  const NumericalSample output(getOutputsNormalization());
  const UnsignedInteger dimension(output.getSize());

  Description outputVariablesNames(dimension);
  Description formulas(dimension);
  if (dimension == 0)
    return NumericalMathFunction(outputVariablesNames, formulas);
  if (output[0][3] == 0.0)
  {
    // Case 1:
    //   orig0 = 0;
    //   orig1 = dmin;
    //   norm0 = -dmax/dmin;
    //   norm1 = 0;
    // y = (x - dmin) / dmax = - (x - orig1) / (orig1 * norm0)
    // x = - y * (orig1 * norm0) + orig1
    for (UnsignedInteger d = 0; d < dimension; ++d)
    {
      outputVariablesNames[d] = (OSS() << "x" << d);
      Bool minus = (- output[d][1] * output[d][2] < 0.0);
      formulas[d] = (OSS().setPrecision(20) << (minus ? "- " : "" ) << outputVariablesNames[d] << " * " << std::abs(output[d][2] * output[d][1]) << (output[d][1] < 0.0 ? " - " : " + ") << std::abs(output[d][1]));
    }
  } else {
    // Case 2:
    //   orig0 = dmin;
    //   orig1 = dmax;
    //   norm0 = -1;
    //   norm1 = 1;
    // y = 2.0 * (x - dmin) / (dmax - dmin) - 1
    // x = dmin + 0.5 * (dmax - dmin) * (y + 1)
    for (UnsignedInteger d = 0; d < dimension; ++d)
    {
      outputVariablesNames[d] = (OSS() << "x" << d);
      Bool minus = (output[d][1] < output[d][0]);
      formulas[d] =(OSS().setPrecision(20) << output[d][0] << (minus ? " - " : " + " ) << (0.5 * std::abs(output[d][1] - output[d][0])) << " * (" << outputVariablesNames[d] << " + 1.0)");
    }
  }
  return NumericalMathFunction(outputVariablesNames, formulas);
}

/** Get evaluation function of a given layer as a NumericalMathFunction */
NumericalMathFunction PMMLNeuralNetwork::getEvaluationFunctionAtLayer(UnsignedInteger layerIndex) const
{
  // See http://www.dmg.org/v3-0/Transformations.html
  const NumericalSample input(getInputsNormalization());
  const UnsignedInteger size(getLayerSize(layerIndex));
  if (size == 0) return NumericalMathFunction();

  String activation(getActivationFunctionAtLayer(layerIndex));

  const NumericalPoint bias(getBiasAtLayer(layerIndex));
  const Matrix weights(getWeightsAtLayer(layerIndex));

  UnsignedInteger prevSize;
  if (layerIndex == 0)
    prevSize = getNumberOfInputs();
  else
    prevSize = getLayerSize(layerIndex - 1);

  Description inputVariablesNames(prevSize);
  for (UnsignedInteger k = 0; k < prevSize; ++k)
    inputVariablesNames[k] = (OSS() << "x" << k);

  Description formulas(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    OSS stream;
    stream.setPrecision(20);
    for (UnsignedInteger k = 0; k < prevSize; ++k)
    {
      stream << (weights(k, i) < 0.0 ? "-" : (k == 0 ? "" : "+")) << std::abs(weights(k, i)) << "*x" << k;
    }
    stream << (bias[i] < 0.0 ? "" : "+" ) << bias[i];
    if(activation == "tanh")
      formulas[i] = String("tanh(")+String(stream)+String(")");
    else if(activation == "logistic")
      formulas[i] = String("1/(1+exp(-(")+String(stream)+String(")))");
    else if(activation == "exponential")
      formulas[i] = String("exp(")+String(stream)+String(")");
    else if(activation == "reciprocal")
      formulas[i] = String("1/(")+String(stream)+String(")");
    else if(activation == "square")
      formulas[i] = String("(")+String(stream)+String(")*(")+String(stream)+String(")");
    else if(activation == "Gauss")
      formulas[i] = String("exp(-(")+String(stream)+String(")*(")+String(stream)+String("))");
    else if(activation == "sine")
      formulas[i] = String("sin(")+String(stream)+String(")");
    else if(activation == "cosine")
      formulas[i] = String("cos(")+String(stream)+String(")");
    else if(activation == "Elliott")
      formulas[i] = String("(")+String(stream)+String(")/(1+abs(")+String(stream)+String("))");
    else if(activation == "arctan")
      formulas[i] =String("0.5*atan(")+String(stream)+String(")/atan(1)");
    else if(activation == "identity")
      formulas[i] = String(stream);
    else
    {
      LOGERROR(OSS() << "Unknown activation function: " << activation);
      formulas[i] = String(stream);
    }
  }
  return NumericalMathFunction(inputVariablesNames, formulas);
}

} /* namespace OTPMML */
