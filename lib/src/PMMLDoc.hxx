//                                               -*- C++ -*-
/**
 *  @file  PMMLDoc.hxx
 *  @brief Internal class to manage a PMML document
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
#ifndef OTPMML_PMMLDOC_HXX
#define OTPMML_PMMLDOC_HXX

#include "OTPMMLprivate.hxx"
#include "PMMLNeuralNetwork.hxx"
#include "PMMLRegressionModel.hxx"

#include <LinearLeastSquares.hxx>
#include <Collection.hxx>

#include <libxml/tree.h>
#include <libxml/xpathInternals.h>

namespace OTPMML
{

class OTPMML_API PMMLDoc
{
  friend class PMMLNeuralNetwork;
  friend class PMMLRegressionModel;

public:
  typedef OT::Collection<OT::String> StringCollection;

  /** Default constructor */
  PMMLDoc();

  /** Default constructor */
  PMMLDoc(const OT::FileName & pmmlFile);

  /** Default destructor */
  ~PMMLDoc();

  /** Delete contents */
  void reset();

  /** Read a PMML from a file */
  OT::Bool read(const OT::FileName & pmmlFile);

  /** Dump PMML into a file */
  OT::Bool write(const OT::FileName & pmmlFile) const;

  /** Get the number of neural networks */
  OT::UnsignedInteger getNumberOfNeuralNetworks() const;

  /** Get model names of neural networks */
  StringCollection getNeuralNetworkModelNames() const;

  /** Get the specified neural network */
  OTPMML::PMMLNeuralNetwork getNeuralNetwork(const OT::String & modelName = "") const;

  /** Get the number of regression models */
  OT::UnsignedInteger getNumberOfRegressionModels() const;

  /** Get model names of regression models */
  StringCollection getRegressionModelNames() const;

  /** Get the specified regression model */
  OTPMML::PMMLRegressionModel getRegressionModel(const OT::String & modelName = "") const;

  // NOTE: Up to OpenTURNS 1.5, LinearLeastSquares.getDataOut() was not declared as const
  //       and thus regression cannot be passed by a const reference.

  /** Add a regression model to current document */
  void addRegressionModel(const OT::String & modelName, OT::LinearLeastSquares regression);

  /** Add a header element to current document if there is none */
  void addHeader();

private:
  /** Check whether document is initialized */
  void checkInitialized() const;

  /** Get modelName attributes of a given category (NeuralNetwork or RegressionModel) */
  StringCollection getModelNames(const OT::String & category) const;

  /** Get result of an XPath query as a NumericalScalar */
  OT::NumericalScalar getXPathQueryScalar(const OT::String & xpathQuery) const;

  /** Get result of an XPath query as a String */
  OT::String getXPathQueryString(const OT::String & xpathQuery) const;

private:
  /** Pointer to the document node */
  xmlDocPtr document_;

  /** Pointer to the root node of the document */
  xmlNodePtr rootNode_;

  /** Pointer to the context used by XPath queries */
  xmlXPathContextPtr xpathContext_;

  /** Prefix used by XPath queries; this is needed if a namespace is declared in XML document */
  OT::String xpathNsPrefix_;

}; /* class PMMLDoc */

} /* namespace OTPMML */

#endif /* OTPMML_PMMLDOC_HXX */
