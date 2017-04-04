//                                               -*- C++ -*-
/**
 *  @file  DAT.cxx
 *  @brief Utility class to import/export DAT files
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
#include "DAT.hxx"
#include "PMMLDoc.hxx"
#include "PMMLNeuralNetwork.hxx"

#include <openturns/Log.hxx>
#include <openturns/Exception.hxx>
#include <openturns/Sample.hxx>
#include <openturns/Os.hxx>

#include <libxml/parser.h>

#include <fstream>
#include <cstdio>  // errno
#include <cstring> // strerror

using namespace OT;

namespace OTPMML
{

CLASSNAMEINIT(DAT);

/** Import experiment plane from a .dat file */
DAT::SampleCollection DAT::Import(const FileName& datFile)
{
  // Read file contents into a single Sample
  Sample inputOutput(Sample::ImportFromTextFile(datFile));

  // Re-read file to parse comments and set NS description
  std::ifstream inputFile(datFile.c_str());
  if (!inputFile.is_open())
    throw FileNotFoundException(HERE) << "Could not open file " << datFile << " for reading, reason: " << std::strerror(errno);
  String line;
  while (!inputFile.eof())
  {
    std::getline(inputFile, line);
    if (inputFile.eof() || line.empty() || line[0] != '#')
    {
      LOGWARN(OSS() << "Unable to find column description in file " << datFile);
      line = "";
      break;
    }
    if (line.substr(0, 14) == "#COLUMN_NAMES:")
    {
      line = line.substr(14);
      break;
    }
  }
  inputFile.close();
  Description fileDescription;
  if (!line.empty())
  {
    // Now extract column description
    // We do not care about trailing spaces, they will be taken care of automatically
    std::size_t pos;
    while ((pos = line.find("|")) != std::string::npos)
    {
      String desc(line.substr(0, pos));
      line = line.substr(pos+1);
      // Trim whitespace
      while (!desc.empty() && (desc[0] == ' ' || desc[0] == '\t'))
        desc = desc.substr(1);
      fileDescription.add(desc);
    }
    // Trim whitespace
    while (!line.empty() && (line[0] == ' ' || line[0] == '\t'))
      line = line.substr(1);
    fileDescription.add(line);
    inputOutput.setDescription(fileDescription);
  }

  // Now split into 2 Sample for input and output.
  // There is no "unstack" method, do it by hand.
  const UnsignedInteger size(inputOutput.getSize());
  const UnsignedInteger dimension(inputOutput.getDimension());
  Sample x(size, dimension - 1);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j + 1 < dimension; ++j)
      x[i][j] = inputOutput[i][j];
  if (!fileDescription.isEmpty())
  {
    fileDescription.erase(fileDescription.begin() + dimension - 1);
    x.setDescription(fileDescription);
  }
  Collection<Sample> result(2);
  result[0] = x;
  result[1] = inputOutput.getMarginal(dimension - 1);
  return result;
}

/** Export experiment plane into a .dat file.
    Sample contains both input and output */
void DAT::Export(const FileName& datFile, const Sample& inputOutput)
{
  const UnsignedInteger size(inputOutput.getSize());
  const Description description(inputOutput.getDescription());
  const UnsignedInteger dimension(inputOutput.getDimension());

  std::ofstream outFile(datFile.c_str());
  if (outFile.fail())
    throw FileOpenException(HERE) << "Could not open file " << datFile << " for writing, reason: " << std::strerror(errno);

  outFile.imbue(std::locale("C"));
  outFile.precision(16);
  // Export the description
  outFile << "#COLUMN_NAMES: ";
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    if (i != 0) outFile << "| ";
    outFile << description[i];
  }
  outFile << Os::GetEndOfLine() << Os::GetEndOfLine();

  // Write the data
  for(UnsignedInteger i = 0; i < size; ++i)
  {
    String separator;
    for(UnsignedInteger j = 0; j < dimension; ++j, separator = " ")
      outFile << separator << std::scientific << inputOutput(i, j);
    outFile << Os::GetEndOfLine();
  }

  // Close the file
  outFile.close();
}

/** Export experiment plane into a .dat file */
void DAT::Export(const FileName& datFile, const Sample& input, const Sample& output)
{
  if (output.getSize() != input.getSize())
    throw InvalidArgumentException(HERE) << "Size mismatch: input size != output size";
  Sample inputOutput(input);
  inputOutput.stack(output);
  Export(datFile, inputOutput);
}

} /* namespace OTPMML */
