//                                               -*- C++ -*-
/**
 *  @brief Internal class to manage a <RegressionModel> element within a PMML document
 *
 *  Copyright 2014-2018 Airbus-EDF-IMACS-Phimeca
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
#include "otpmml/PMMLRegressionModel.hxx"
#include "otpmml/PMMLDoc.hxx"

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
PMMLRegressionModel::PMMLRegressionModel(const PMMLDoc * pmml, const String & modelName, const xmlNodePtr & node)
  : pmml_(pmml)
  , modelName_(modelName)
  , node_(node)
  , validated_(false)
{
}

/** Get model name */
String PMMLRegressionModel::getModelName() const
{
  return modelName_;
}

/** Set XPath context to node_ */
void PMMLRegressionModel::setXPathContext() const
{
  pmml_->checkInitialized();
  pmml_->xpathContext_->node = node_;
}

/** Get intercept */
Scalar PMMLRegressionModel::getIntercept() const
{
  setXPathContext();
  checkValid();
  return pmml_->getXPathQueryScalar(String("number(./") + pmml_->xpathNsPrefix_ + String("RegressionTable/@intercept)"));
}

/** Get name of target variable */
String PMMLRegressionModel::getTargetVariableName() const
{
  setXPathContext();
  checkValid();
  return pmml_->getXPathQueryString("./@targetFieldName");
}

/** Get coefficients */
Sample PMMLRegressionModel::getCoefficients() const
{
  setXPathContext();
  checkValid();
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST (String("./") + pmml_->xpathNsPrefix_ + String("RegressionTable/") + pmml_->xpathNsPrefix_ + String("NumericPredictor")).c_str(), pmml_->xpathContext_);
  Sample result;
  if(!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
  {
    Collection<String> names;
    Collection<Scalar> coefficients;
    for (int cnt = 0; cnt < xpathObj->nodesetval->nodeNr; ++cnt)
    {
      for (xmlAttr *cur_attr = xpathObj->nodesetval->nodeTab[cnt]->properties;
           cur_attr != NULL; cur_attr = cur_attr->next)
      {
        if (cur_attr->type != XML_ATTRIBUTE_NODE) continue;
        if (0 == xmlStrcmp(cur_attr->name, BAD_CAST "name"))
        {
          names.add(String(reinterpret_cast<const char*>(cur_attr->children->content)));
        }
        else if (0 == xmlStrcmp(cur_attr->name, BAD_CAST "coefficient"))
        {
          coefficients.add(strtod(reinterpret_cast<const char*>(cur_attr->children->content), NULL));
        }
      }
    }
    result = Sample(1, Point(coefficients));
    result.setDescription(names);
  }
  xmlXPathFreeObject(xpathObj);
  return result;
}

/** Checks that this is a linearRegression model */
void PMMLRegressionModel::checkValid() const
{
  if (validated_) return;

  // Check regression type
  for (xmlAttr *cur_attr = node_->properties;
       cur_attr != NULL; cur_attr = cur_attr->next)
  {
    if (cur_attr->type != XML_ATTRIBUTE_NODE) continue;
    if (0 == xmlStrcmp(cur_attr->name, BAD_CAST "modelType"))
    {
      const char* modelType = reinterpret_cast<const char*>(cur_attr->children->content);
      if (0 != strcmp(modelType, "linearRegression"))
        throw InvalidArgumentException(HERE) << "RegressionModel found with modelType='" << modelType << "', but only 'linearRegression' is supported";
      break;
    }
    if (0 == xmlStrcmp(cur_attr->name, BAD_CAST "functionName"))
    {
      const char* functionName = reinterpret_cast<const char*>(cur_attr->children->content);
      if (0 != strcmp(functionName, "regression"))
        throw InvalidArgumentException(HERE) << "RegressionModel found with functionName='" << functionName << "', but only 'regression' is supported";
      break;
    }
    if (0 == xmlStrcmp(cur_attr->name, BAD_CAST "normalizationMethod"))
    {
      const char* normalizationMethod = reinterpret_cast<const char*>(cur_attr->children->content);
      if (0 != strcmp(normalizationMethod, "none"))
        throw InvalidArgumentException(HERE) << "RegressionModel found with normalizationMethod='" << normalizationMethod << "', but only 'none' is supported";
      break;
    }
  }

  setXPathContext();
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST (String("./") + pmml_->xpathNsPrefix_ + String("RegressionTable")).c_str(), pmml_->xpathContext_);
  if(xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
  {
    xmlXPathFreeObject(xpathObj);
    throw InvalidArgumentException(HERE) << "No <RegressionTable> element found";
  }
  if(xpathObj->nodesetval->nodeNr != 1)
  {
    const UnsignedInteger nr(xpathObj->nodesetval->nodeNr);
    xmlXPathFreeObject(xpathObj);
    throw InvalidArgumentException(HERE) << nr << "  <RegressionTable> elements found, but there must be only one";
  }
  for (xmlNode *cur_node = xpathObj->nodesetval->nodeTab[0]->children;
       cur_node != NULL; cur_node = cur_node->next)
  {
    if (cur_node->type != XML_ELEMENT_NODE) continue;
    if (0 != xmlStrcmp(cur_node->name, BAD_CAST "NumericPredictor"))
    {
      const String name(reinterpret_cast<const char*>(cur_node->name));
      xmlXPathFreeObject(xpathObj);
      throw InvalidArgumentException(HERE) << "Element <" << name << "> found, but only <NumericPredictor> is supported";
    }
  }
  xmlXPathFreeObject(xpathObj);

  setXPathContext();
  xpathObj = xmlXPathEvalExpression(BAD_CAST (String("./") + pmml_->xpathNsPrefix_ + String("RegressionTable/") + pmml_->xpathNsPrefix_ + String("NumericPredictor/@exponent")).c_str(), pmml_->xpathContext_);
  if(xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
  {
    xmlXPathFreeObject(xpathObj);
    throw InvalidArgumentException(HERE) << "Internal error, exponent attributes are not found";
  }

  for (int cnt = 0; cnt < xpathObj->nodesetval->nodeNr; ++cnt)
  {
    xmlNode *cur_node = xpathObj->nodesetval->nodeTab[cnt];
    if (cur_node->type != XML_ATTRIBUTE_NODE) continue;
    if (0 == xmlStrcmp(cur_node->name, BAD_CAST "exponent"))
    {
      UnsignedInteger exponent(strtol(reinterpret_cast<const char*>(cur_node->children->content), NULL, 10));
      if (1 != exponent)
      {
        xmlXPathFreeObject(xpathObj);
        throw InvalidArgumentException(HERE) << "Exponent " << exponent << " found, but only exponent=1 is supported";
      }
    }
  }
  xmlXPathFreeObject(xpathObj);

  validated_ = true;
}

} /* namespace OTPMML */
