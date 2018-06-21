#include "TestITK.h"

template< class TImage > // note the different class name to the declaration; since this is a template, the name is truly irrelevant
TestITK< TImage >::TestITK()
{
  // initialize all your member variables
}

template< class TImage >
TestITK< TImage >::~TestITK()
{
  // ideally, this should be empty since you are using smart pointers (itk::Pointers *are* smart)
}

template< class TImage >
void TestITK< TImage >::memberFunctionUsingClassTemplate(typename TImage::Pointer input)
{
  // some fancy processing with input 
}

template< class TImage >
template< class TAnotherImage >
void TestITK< TImage >::memberFunctionUsingClassAndAnotherTemplate(typename TImage::Pointer input, typename TAnotherImage::Pointer anotherImage)
{
  // some even more fancy processing with input and anotherImage
}