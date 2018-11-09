// SWIG file

%{
#include <openturns/PythonWrappingFunctions.hxx>
#include "otpmml/DAT.hxx"
#include <openturns/PersistentCollection.hxx>
#include <openturns/Sample.hxx>
%}

%include DAT_doc.i

%template(SampleCollection) OT::Collection<OT::Sample>;
%template(SamplePersistentCollection) OT::PersistentCollection<OT::Sample>;

%include otpmml/DAT.hxx

%extend OT::Collection<OT::Sample>
{

OT::Collection<OT::Sample> (PyObject * pyObj)
{
  return OT::buildCollectionFromPySequence<OT::Sample>( pyObj );
}

}

