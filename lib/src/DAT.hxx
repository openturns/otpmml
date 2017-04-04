//                                               -*- C++ -*-
/**
 *  @brief Utility class to import/export Uranie .dat files
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
#ifndef OTPMML_DAT_HXX
#define OTPMML_DAT_HXX

#include "OTPMMLprivate.hxx"

#include <openturns/Sample.hxx>

namespace OTPMML
{

/**
 * @class DAT
 *
 * DAT is a utility class to import/export Uranie .dat files.
 */
class OTPMML_API DAT
  : public OT::Object
{
  CLASSNAME;

public:
  typedef OT::Collection<OT::Sample> SampleCollection;

  /** Import experiment plane from a .dat file */
  static SampleCollection Import(const OT::FileName& datFile);

  /** Export experiment plane into a .dat file */
  static void Export(const OT::FileName& datFile, const OT::Sample& input, const OT::Sample& output);

  /** Export experiment plane into a .dat file */
  static void Export(const OT::FileName& datFile, const OT::Sample& inputOutput);

}; /* class DAT */

} /* namespace OTPMML */

#endif /* OTPMML_DAT_HXX */
