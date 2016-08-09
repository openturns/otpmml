// SWIG file

%{
#include <openturns/PythonWrappingFunctions.hxx>
#include "DAT.hxx"
#include <openturns/PersistentCollection.hxx>
#include <openturns/NumericalSample.hxx>
%}

%include DAT_doc.i

%template(NumericalSampleCollection) OT::Collection<OT::NumericalSample>;
%template(NumericalSamplePersistentCollection) OT::PersistentCollection<OT::NumericalSample>;

%include DAT.hxx

%extend OT::Collection<OT::NumericalSample>
{

OT::Collection<OT::NumericalSample> (PyObject * pyObj)
{
  return OT::buildCollectionFromPySequence<OT::NumericalSample>( pyObj );
}

}

