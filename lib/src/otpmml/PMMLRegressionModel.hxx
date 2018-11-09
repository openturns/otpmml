//                                               -*- C++ -*-
/**
 *  @file  PMMLRegressionModel.hxx
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
#ifndef OTPMML_PMMLREGRESSIONMODEL_HXX
#define OTPMML_PMMLREGRESSIONMODEL_HXX

#include <openturns/Sample.hxx>

#include "otpmml/OTPMMLprivate.hxx"

#include <libxml/tree.h>

namespace OTPMML
{

// Forward declaration
class PMMLDoc;

class OTPMML_API PMMLRegressionModel
{
public:
  typedef OT::Collection<OT::String> StringCollection;

  /** Default constructor */
  PMMLRegressionModel(const PMMLDoc * pmml, const OT::String & modelName, const xmlNodePtr & node);

  /** Get model name */
  OT::String getModelName() const;

  /** Get intercept */
  OT::Scalar getIntercept() const;

  /** Get name of target variable */
  OT::String getTargetVariableName() const;

  /** Get coefficients */
  OT::Sample getCoefficients() const;

private:
  /** Set XPath context to node_ */
  void setXPathContext() const;

  /** Checks that this is a linearRegression model */
  void checkValid() const;

  /** Pointer to parent PMMLDoc instance */
  const PMMLDoc * pmml_;

  /** Model name */
  const OT::String modelName_;

  /** Pointer to the <RegressionModel> XML element */
  const xmlNodePtr node_;

  /** Flag to validate model only once */
  mutable OT::Bool validated_;

}; /* class PMMLRegressionModel */

} /* namespace OTPMML */

#endif /* OTPMML_PMMLREGRESSIONMODEL_HXX */
