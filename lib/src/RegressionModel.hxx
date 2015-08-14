//                                               -*- C++ -*-
/**
 *  @file  RegressionModel.hxx
 *  @brief The class RegressionModel allows reading and writing LinearLeastSquares instances in a PMML file
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
#ifndef OTPMML_REGRESSIONMODEL_HXX
#define OTPMML_REGRESSIONMODEL_HXX

#include <LinearLeastSquares.hxx>
#include <Pointer.hxx>

#include "OTPMMLprivate.hxx"

namespace OTPMML
{

/**
 * @class RegressionModel
 *
 * The class RegressionModel allows reading a regression model from a PMML file.
 * If the PMML file contains several regression models, the name of the desired regression model must
 * be provided.  Method getLinearLeastSquares can then be used to transform it into a LinearLeastSquares
 * instance.
 *
 * In order to export a LinearLeastSquares instance into a PMML file, a RegressionModel instance is
 * built from this LinearLeastSquares instance, and method exportToPMMLFile can then be called.
 */
class OTPMML_API RegressionModel
  : public OT::PersistentObject
{
public:
  /** Default constructor */
  RegressionModel(const OT::FileName & pmmlFile, const OT::String& modelName = "");

  /** Default constructor */
  RegressionModel(const OT::LinearLeastSquares & implementation);

  /** Virtual constructor method */
  RegressionModel * clone() const;

  /** LinearLeastSquares accessor */
  OT::LinearLeastSquares getLinearLeastSquares() const;

  /** Export model to a PMML file */
  void exportToPMMLFile(const OT::FileName & pmmlFile) const;

  /** String converter */
  OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(OT::Advocate & adv);

private:
  OT::Pointer<OT::LinearLeastSquares> metamodel_;
}; /* class RegressionModel */

} /* namespace OTPMML */

#endif /* OTPMML_REGRESSIONMODEL_HXX */
