//                                               -*- C++ -*-
/**
 *  @file  RegressionModel.cxx
 *  @brief RegressionModel
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
#include "otpmml/RegressionModel.hxx"
#include "otpmml/PMMLDoc.hxx"
#include "otpmml/PMMLRegressionModel.hxx"

#include <libxml/parser.h>

using namespace OT;

namespace OTPMML
{

/* Default constructor */
RegressionModel::RegressionModel(const FileName & pmmlFile, const OT::String& modelName)
  : PersistentObject()
{
  xmlInitParser();
  PMMLDoc doc(pmmlFile);
  PMMLRegressionModel model(doc.getRegressionModel(modelName));
  const Sample coefficients(model.getCoefficients());
  const Scalar intercept(model.getIntercept());
  // There is no LinearLeastSquares constructor with explicit
  // coefficients, thus build a problem which will give the
  // expected solution.
  const UnsignedInteger dimension(coefficients.getDimension());
  Sample inputSample(dimension + 1, dimension);
  Sample outputSample(dimension + 1, 1);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    inputSample(i, i) = 1.0;
    outputSample(i, 0) = intercept + coefficients(0, i);
  }
  outputSample(dimension, 0) = intercept;
  inputSample.setDescription(coefficients.getDescription());
  outputSample.setDescription(Description(1, model.getTargetVariableName()));
  metamodel_ = new LinearLeastSquares(inputSample, outputSample);
  metamodel_->setName(model.getModelName());
  metamodel_->run();
  xmlCleanupParser();
}

RegressionModel::RegressionModel(const LinearLeastSquares & implementation)
  : PersistentObject()
  , metamodel_(new LinearLeastSquares(implementation))
{
  // Nothing to do
}

/* Virtual constructor method */
RegressionModel * RegressionModel::clone() const
{
  return new RegressionModel(*this);
}

/** String converter */
String RegressionModel::__repr__() const
{
  OSS oss;
  oss << " name=" << getName()
      << " model=" << *metamodel_;
  return oss;

}

/** LinearLeastSquares accessor */
LinearLeastSquares RegressionModel::getLinearLeastSquares() const
{
  return *metamodel_;
}

/** Export linear regression into a .pmml file */
void RegressionModel::exportToPMMLFile(const FileName & pmmlFile) const
{
  xmlInitParser();
  PMMLDoc doc;
  String modelName(metamodel_->getName());
  if (modelName.empty())
    modelName = "Unnamed";
  doc.addRegressionModel(modelName, *metamodel_);
  doc.write(pmmlFile);
  xmlCleanupParser();
}

/* Method save() stores the object through the StorageManager */
void RegressionModel::save(Advocate & adv) const
{
  PersistentObject::save( adv );
  adv.saveAttribute( "metamodel_", *metamodel_ );
}

/* Method load() reloads the object from the StorageManager */
void RegressionModel::load(Advocate & adv)
{
  PersistentObject::load( adv );
  adv.loadAttribute( "metamodel_", *metamodel_ );
}

} /* namespace OTPMML */
