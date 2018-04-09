//                                               -*- C++ -*-
/**
 *  @file  PMMLDoc.cxx
 *  @brief Internal class to manage a PMML document
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
#include "PMMLDoc.hxx"
#include "PMMLNeuralNetwork.hxx"
#include "PMMLRegressionModel.hxx"

#include <openturns/LinearLeastSquares.hxx>
#include <openturns/Exception.hxx>

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>  // errno

using namespace OT;


namespace OTPMML
{

/* Default constructor */
PMMLDoc::PMMLDoc()
  : document_(NULL)
  , rootNode_(NULL)
  , xpathContext_(NULL)
  , xpathNsPrefix_()
{
  // Create an empty document with a root PMML node
  reset();
}

/* Default constructor */
PMMLDoc::PMMLDoc(const FileName & pmmlFile)
  : document_(NULL)
  , rootNode_(NULL)
  , xpathContext_(NULL)
  , xpathNsPrefix_()
{
  std::ifstream inputFile(pmmlFile.c_str());
  if (!inputFile.is_open())
    throw FileNotFoundException(HERE) << "Could not open file " << pmmlFile << " for reading, reason: " << std::strerror(errno);
  inputFile.close();
  if (!read(pmmlFile)) reset();
}

/* Destructor */
PMMLDoc::~PMMLDoc()
{
  if (xpathContext_) xmlXPathFreeContext(xpathContext_);
  xpathContext_ = NULL;
  if (document_) xmlFreeDoc(document_);
  document_ = NULL;
}

void PMMLDoc::checkInitialized() const
{
  if (document_ == NULL || rootNode_ == NULL || xpathContext_ == NULL)
    throw InvalidArgumentException(HERE) << "Document is uninitialized";
}

/* Remove contents */
void PMMLDoc::reset()
{
  if (xpathContext_) xmlXPathFreeContext(xpathContext_);
  if (document_) xmlFreeDoc(document_);

  // Create an empty document with a root PMML node
  document_ = xmlNewDoc(BAD_CAST "1.0");
  rootNode_ = xmlNewNode(NULL, BAD_CAST "PMML");
  xmlNewProp(rootNode_, BAD_CAST "version", BAD_CAST "3.0");
  xmlDocSetRootElement(document_, rootNode_);

  xmlNsPtr ns = xmlNewNs(rootNode_, BAD_CAST "http://www.dmg.org/PMML-3_0", NULL);
  xmlSetNs(rootNode_, ns);

  // Create an XPath context
  xpathContext_ = xmlXPathNewContext(document_);
  xpathNsPrefix_ = String("p:");
  xmlXPathRegisterNs(xpathContext_,  BAD_CAST "p", BAD_CAST "http://www.dmg.org/PMML-3_0");
}

/* Read a file */
Bool PMMLDoc::read(const FileName & pmmlFile)
{
  if (xmlFirstElementChild(rootNode_) != NULL)
  {
    std::cerr << "WARNING: document is not empty, it is now destroyed before reading file " << pmmlFile << std::endl;
    xmlXPathFreeContext(xpathContext_);
    xpathContext_ = NULL;
    xmlFreeDoc(document_);
    document_ = NULL;
  }

  document_ = xmlParseFile(pmmlFile.c_str());
  if (document_ == NULL)
  {
      std::cerr << "Unable to parse XML file " << pmmlFile << std::endl;
      return false;
  }
  rootNode_ = xmlDocGetRootElement(document_);

  // Create an XPath context
  xpathContext_ = xmlXPathNewContext(document_);
  if (rootNode_->ns)
  {
    xpathNsPrefix_ = String("p:");
    xmlXPathRegisterNs(xpathContext_,  BAD_CAST "p", BAD_CAST "http://www.dmg.org/PMML-3_0");
  }
  return (rootNode_ != NULL);
}

/* Dump PMML into a file */
Bool PMMLDoc::write(const FileName & pmmlFile) const
{
  return xmlSaveFile(pmmlFile.c_str(), document_) != -1;
}

UnsignedInteger PMMLDoc::getNumberOfNeuralNetworks() const
{
  checkInitialized();
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST (String("count(")+xpathNsPrefix_+String("PMML/")+xpathNsPrefix_+String("NeuralNetwork)")).c_str(), xpathContext_);
  const UnsignedInteger result = static_cast<UnsignedInteger>(xpathObj->floatval);
  xmlXPathFreeObject(xpathObj);
  return result;
}

PMMLDoc::StringCollection PMMLDoc::getNeuralNetworkModelNames() const
{
  return getModelNames(String("NeuralNetwork"));
}

PMMLNeuralNetwork PMMLDoc::getNeuralNetwork(const OT::String & modelName) const
{
  checkInitialized();
  String query(OSS() << "/" << xpathNsPrefix_ << "PMML/" << xpathNsPrefix_ << "NeuralNetwork[@modelName='" << modelName << "']/@modelName");
  if (modelName.empty())
  {
    Collection<String> modelNames = getNeuralNetworkModelNames();
    query = OSS() << "/" << xpathNsPrefix_ << "PMML/" << xpathNsPrefix_ << "NeuralNetwork[@modelName='" << modelNames[0] << "']/@modelName";
  }
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST query.c_str(), xpathContext_);
  xmlNodePtr node = NULL;
  String name;
  if(!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
  {
    node = xpathObj->nodesetval->nodeTab[0]->parent;
    xmlChar * text = xmlNodeListGetString(document_, xpathObj->nodesetval->nodeTab[0]->xmlChildrenNode, 1);
    name = String(reinterpret_cast<char*>(text));
    xmlFree(text);
  }
  xmlXPathFreeObject(xpathObj);
  if (name.empty())
  {
    throw InvalidArgumentException(HERE) << "Unable to find NeuralNetwork named '" << modelName << "', models found are: " << getNeuralNetworkModelNames();
  }
  return PMMLNeuralNetwork(this, name, node);
}

PMMLDoc::StringCollection PMMLDoc::getModelNames(const String & category) const
{
  checkInitialized();
  String query(String("/")+xpathNsPrefix_+String("PMML/")+xpathNsPrefix_+category);
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST query.c_str(), xpathContext_);
  StringCollection result;
  if(!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
  {
    for (int cnt = 0; cnt < xpathObj->nodesetval->nodeNr; ++cnt)
    {
      String name;
      for (xmlAttr *cur_attr = xpathObj->nodesetval->nodeTab[cnt]->properties;
           cur_attr != NULL; cur_attr = cur_attr->next)
      {
        if (cur_attr->type == XML_ATTRIBUTE_NODE && 0 == xmlStrcmp(cur_attr->name, BAD_CAST "modelName"))
        {
            name = reinterpret_cast<const char*>(cur_attr->children->content);
            break;
        }
      }
      result.add(name);
    }
  }
  xmlXPathFreeObject(xpathObj);
  return result;
}

Scalar PMMLDoc::getXPathQueryScalar(const String & xpathQuery) const
{
  checkInitialized();
  Scalar result = 0.0;
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST xpathQuery.c_str(), xpathContext_);
  if (!xmlXPathIsNaN(xpathObj->floatval)) result = static_cast<Scalar>(xpathObj->floatval);
  xmlXPathFreeObject(xpathObj);
  return result;
}

String PMMLDoc::getXPathQueryString(const String & xpathQuery) const
{
  checkInitialized();
  String result;
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST xpathQuery.c_str(), xpathContext_);
  if(!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
  {
    xmlChar * text = xmlNodeListGetString(document_, xpathObj->nodesetval->nodeTab[0]->xmlChildrenNode, 1);
    result = String(reinterpret_cast<char*>(text));
    xmlFree(text);
  }
  xmlXPathFreeObject(xpathObj);
  return result;
}

UnsignedInteger PMMLDoc::getNumberOfRegressionModels() const
{
  checkInitialized();
  UnsignedInteger count = 0;
  for (xmlNode *cur_node = rootNode_->children; cur_node != NULL; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE && 0 == xmlStrcmp(cur_node->name, BAD_CAST "RegressionModel"))
      ++count;
  }
  return count;
}

PMMLDoc::StringCollection PMMLDoc::getRegressionModelNames() const
{
  return getModelNames(String("RegressionModel"));
}

PMMLRegressionModel PMMLDoc::getRegressionModel(const OT::String & modelName) const
{
  checkInitialized();
  String query(OSS() << "/" << xpathNsPrefix_ << "PMML/" << xpathNsPrefix_ << "RegressionModel[@modelName='" << modelName << "']/@modelName");
  if (modelName.empty())
  {
    Collection<String> modelNames = getRegressionModelNames();
    query = OSS() << "/" << xpathNsPrefix_ << "PMML/" << xpathNsPrefix_ << "RegressionModel[@modelName='" << modelNames[0] << "']/@modelName";
  }
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST query.c_str(), xpathContext_);
  xmlNodePtr node = NULL;
  String name;
  if(!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
  {
    node = xpathObj->nodesetval->nodeTab[0]->parent;
    xmlChar * text = xmlNodeListGetString(document_, xpathObj->nodesetval->nodeTab[0]->xmlChildrenNode, 1);
    name = String(reinterpret_cast<char*>(text));
    xmlFree(text);
  }
  xmlXPathFreeObject(xpathObj);
  if (name.empty())
  {
    throw InvalidArgumentException(HERE) << "Unable to find RegressionModel named '" << modelName << "', models found are: " << getRegressionModelNames();
  }
  return PMMLRegressionModel(this, name, node);
}

void PMMLDoc::addHeader()
{
  checkInitialized();

  xmlNodePtr headerNode = NULL;
  for (xmlNodePtr cur_node = rootNode_->children; cur_node != NULL; cur_node = cur_node->next)
  {
    if (cur_node->type != XML_ELEMENT_NODE) continue;
    if (0 == xmlStrcmp(cur_node->name, BAD_CAST "Header"))
      return;
    // Document is not empty, add header before the first element
    while(cur_node->prev != NULL && cur_node->prev->type == XML_TEXT_NODE) { cur_node = cur_node->prev; }

    headerNode = xmlNewNode(rootNode_->ns, BAD_CAST "Header");
    xmlAddPrevSibling(cur_node, headerNode);
    xmlAddPrevSibling(headerNode, xmlNewText(BAD_CAST "\n  "));
    break;
  }
  if (headerNode == NULL)
  {
    // Document is empty.  Take care of indentation
    xmlNodeAddContent(rootNode_, BAD_CAST "\n  ");
    headerNode = xmlNewChild(rootNode_, 0, BAD_CAST "Header", 0);
    xmlNodeAddContent(rootNode_, BAD_CAST "\n");
  }

  xmlNewProp(headerNode, BAD_CAST "copyright", BAD_CAST "copyright text");
  xmlNewProp(headerNode, BAD_CAST "description", BAD_CAST "description text");
  xmlNodeAddContent(headerNode, BAD_CAST "\n    ");
  xmlNodePtr newApplicationNode = xmlNewChild(headerNode, 0, BAD_CAST "Application", 0);
  xmlNewProp(newApplicationNode, BAD_CAST "name", BAD_CAST "otpmml");
  xmlNewProp(newApplicationNode, BAD_CAST "version", BAD_CAST "1.0");
  xmlNodeAddContent(headerNode, BAD_CAST "\n  ");
}

void PMMLDoc::addRegressionModel(const String & modelName, LinearLeastSquares regression)
{
  // NOTE: Up to OpenTURNS 1.5, LinearLeastSquares.getDataOut() was not declared as const
  //       and thus regression cannot be passed by a const reference.
  checkInitialized();
  addHeader();

  const Sample dataIn(regression.getDataIn());
  Sample dataOut(regression.getDataOut());
  Description descriptionIn(dataIn.getDescription());
  for (UnsignedInteger i = 0; i < dataIn.getDimension(); ++i)
    if (descriptionIn[i].empty())
      descriptionIn[i] = (OSS() << "x" << i);
  Description descriptionOut(dataOut.getDescription());
  if (descriptionOut[0].empty())
    descriptionOut[0] = "output";
  const Point intercept(regression.getConstant());
  if (intercept.getDimension() != 1)
    throw InvalidArgumentException(HERE) << "Intercept of LinearLeastSquares must be a scalar, but has dimension " << intercept.getDimension();
  const Matrix linear(regression.getLinear());

  // Update dictionary
  String query(OSS() << "/" << xpathNsPrefix_ << "PMML/" << xpathNsPrefix_ << "DataDictionary");
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST query.c_str(), xpathContext_);
  xmlNodePtr dictionnaryNode = NULL;
  Bool addNewline(true);
  if(xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
  {
    // Find a node to anchor DataDictionary
    xmlNodePtr anchor = NULL;
    Bool append(true);
    for (xmlNodePtr cur_node = rootNode_->children; cur_node != NULL; cur_node = cur_node->next)
    {
      if (cur_node->type != XML_ELEMENT_NODE) continue;
      if (NULL == anchor && 0 == xmlStrcmp(cur_node->name, BAD_CAST "Header"))
      {
        anchor = cur_node;
        while(anchor->next != NULL && anchor->next->type == XML_TEXT_NODE) { anchor = anchor->next; }
      }
      else if (0 == xmlStrcmp(cur_node->name, BAD_CAST "MiningBuildTask"))
      {
        anchor = cur_node;
        while(anchor->next != NULL && anchor->next->type == XML_TEXT_NODE) { anchor = anchor->next; }
        break;
      }
      else
      {
        if (anchor == NULL)
        {
          append = false;
          anchor = cur_node;
          while(anchor->prev != NULL && anchor->prev->type == XML_TEXT_NODE) { anchor = anchor->prev; }
        }
        break;
      }
    }

    if (anchor == NULL)
    {
      xmlNodeAddContent(rootNode_, BAD_CAST "\n  ");
      dictionnaryNode = xmlNewChild(rootNode_, 0, BAD_CAST "DataDictionary", 0);
    }
    else
    {
      dictionnaryNode = xmlNewNode(rootNode_->ns, BAD_CAST "DataDictionary");
      if (append)
      {
        xmlNodeAddContent(anchor, BAD_CAST "  ");
        xmlAddNextSibling(anchor, dictionnaryNode);
        xmlNodeAddContent(anchor, BAD_CAST "\n  ");
      }
      else
      {
        xmlAddPrevSibling(anchor, dictionnaryNode);
        xmlAddPrevSibling(anchor, xmlNewText(BAD_CAST "\n  "));
      }
    }
    xmlNewProp(dictionnaryNode, BAD_CAST "numberOfFields", BAD_CAST String(OSS() << (1 + linear.getNbRows())).c_str());
  }
  else
  {
    addNewline = false;
    dictionnaryNode = xpathObj->nodesetval->nodeTab[0];
    for (xmlAttr *cur_attr = dictionnaryNode->properties;
         cur_attr != NULL; cur_attr = cur_attr->next)
    {
      if (cur_attr->type == XML_ATTRIBUTE_NODE && 0 == xmlStrcmp(cur_attr->name, BAD_CAST "numberOfFields"))
      {
          UnsignedInteger numberOfFields = strtol(reinterpret_cast<const char*>(cur_attr->children->content), NULL, 10);
          numberOfFields += 1 + linear.getNbRows();
          xmlSetProp(dictionnaryNode, BAD_CAST "numberOfFields", BAD_CAST String(OSS() << numberOfFields).c_str());
          break;
      }
    }
  }
  xmlXPathFreeObject(xpathObj);

  if (addNewline)
    xmlNodeAddContent(rootNode_, BAD_CAST "\n  ");
  else
    xmlNodeAddContent(rootNode_, BAD_CAST "  ");
  xmlNodePtr newRegressionNode = xmlNewChild(rootNode_, 0, BAD_CAST "RegressionModel", 0);
  xmlNewProp(newRegressionNode, BAD_CAST "modelName", BAD_CAST modelName.c_str());
  xmlNewProp(newRegressionNode, BAD_CAST "functionName", BAD_CAST "regression");
  xmlNewProp(newRegressionNode, BAD_CAST "algorithmName", BAD_CAST "linearRegression");
  xmlNewProp(newRegressionNode, BAD_CAST "targetFieldName", BAD_CAST descriptionOut[0].c_str());

  xmlNodeAddContent(newRegressionNode, BAD_CAST "\n    ");
  xmlNodePtr newMiningSchemaNode = xmlNewChild(newRegressionNode, 0, BAD_CAST "MiningSchema", 0);
  for (UnsignedInteger i = 0; i < dataIn.getDimension(); ++i)
  {
    xmlNodeAddContent(newMiningSchemaNode, BAD_CAST "\n      ");
    xmlNodePtr newMiningFieldNode = xmlNewChild(newMiningSchemaNode, 0, BAD_CAST "MiningField", 0);
    xmlNewProp(newMiningFieldNode, BAD_CAST "name", BAD_CAST descriptionIn[i].c_str());
  }
  xmlNodeAddContent(newMiningSchemaNode, BAD_CAST "\n      ");
  xmlNodePtr newMiningFieldNode = xmlNewChild(newMiningSchemaNode, 0, BAD_CAST "MiningField", 0);
  xmlNewProp(newMiningFieldNode, BAD_CAST "name", BAD_CAST descriptionOut[0].c_str());
  xmlNewProp(newMiningFieldNode, BAD_CAST "usageType", BAD_CAST "predicted");
  xmlNodeAddContent(newMiningSchemaNode, BAD_CAST "\n    ");

  xmlNodeAddContent(newRegressionNode, BAD_CAST "\n    ");
  xmlNodePtr newRegressionTableNode = xmlNewChild(newRegressionNode, 0, BAD_CAST "RegressionTable", 0);
  xmlNewProp(newRegressionTableNode, BAD_CAST "intercept", BAD_CAST String(OSS().setPrecision(20) << intercept[0]).c_str());

  for (UnsignedInteger i = 0; i < linear.getNbRows(); ++i)
  {
    xmlNodeAddContent(newRegressionTableNode, BAD_CAST "\n      ");
    xmlNodePtr newNumericPredictorNode = xmlNewChild(newRegressionTableNode, 0, BAD_CAST "NumericPredictor", 0);
    xmlNewProp(newNumericPredictorNode, BAD_CAST "name", BAD_CAST descriptionIn[i].c_str());
    xmlNewProp(newNumericPredictorNode, BAD_CAST "exponent", BAD_CAST "1");
    xmlNewProp(newNumericPredictorNode, BAD_CAST "coefficient", BAD_CAST String(OSS().setPrecision(20) << linear(i,0)).c_str());
  }
  xmlNodeAddContent(newRegressionTableNode, BAD_CAST "\n    ");
  xmlNodeAddContent(newRegressionNode, BAD_CAST "\n  ");


  for (UnsignedInteger i = 0; i < linear.getNbRows(); ++i)
  {
    if (i != 0 || addNewline)
      xmlNodeAddContent(dictionnaryNode, BAD_CAST "\n    ");
    else
      xmlNodeAddContent(dictionnaryNode, BAD_CAST "  ");
    xmlNodePtr newDataFieldNode = xmlNewChild(dictionnaryNode, 0, BAD_CAST "DataField", 0);
    xmlNewProp(newDataFieldNode, BAD_CAST "name", BAD_CAST descriptionIn[i].c_str());
    xmlNewProp(newDataFieldNode, BAD_CAST "optype", BAD_CAST "continuous");
  }
  xmlNodeAddContent(dictionnaryNode, BAD_CAST "\n    ");
  xmlNodePtr newDataFieldNode = xmlNewChild(dictionnaryNode, 0, BAD_CAST "DataField", 0);
  xmlNewProp(newDataFieldNode, BAD_CAST "name", BAD_CAST descriptionOut[0].c_str());
  xmlNewProp(newDataFieldNode, BAD_CAST "optype", BAD_CAST "continuous");
  xmlNodeAddContent(dictionnaryNode, BAD_CAST "\n  ");
  xmlNodeAddContent(rootNode_, BAD_CAST "\n");

}

} /* namespace OTPMML */
