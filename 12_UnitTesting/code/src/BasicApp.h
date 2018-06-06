/**
\file BasicApp.h

\brief This file gives a basic example of itk::Image iteration
*/

#pragma once

#include "itkImage.h"
#include "itkImageIterator.h"
#include "itkImageRegionConstIterator.h"

//typedef itk::Image< double, 2 > TImageType;

template <class TImageType>
class BasicApp
{
public:
  //! Default Constructor
  BasicApp()
  {
    m_inputImage = typename TImageType::New();
    m_outputImage = typename TImageType::New();
  }

  //! Default Destructor
  ~BasicApp()
  {

  }

  //! Sets the Input Image
  void SetInputImage(typename TImageType::Pointer inputImage)
  {
    // get the pointer to the inputImage and copy it to the member variable
    m_inputImage = inputImage;

    // this is done so that the original image isn't accidentally modified, increases memory requirement
    // not needed if ImageRegionConstIterator is used for iteration
    m_inputImage->DisconnectPipeline();
  }

  //! Sets the Input Image
  void SetScale(float inputScaleFactor)
  {
    m_scaleFactor = inputScaleFactor;
  }

  //! Do the computation here
  void Run()
  {
    // set the buffer regions as the same for both in the input and output images
    m_outputImage->SetSpacing(m_inputImage->GetSpacing());
    m_outputImage->SetOrigin(m_inputImage->GetOrigin());
    m_outputImage->SetLargestPossibleRegion(m_inputImage->GetLargestPossibleRegion());
    m_outputImage->SetBufferedRegion(m_inputImage->GetBufferedRegion());
    m_outputImage->SetRequestedRegionToLargestPossibleRegion();
    m_outputImage->Allocate();

    // initializing the iterator for input image as a const iterator since its value will not be changed
    // if this is done, calling "disconnect pipeline" during construction is not needed
    itk::ImageRegionConstIterator< TImageType > inputIterator(m_inputImage, m_inputImage->GetBufferedRegion());

    // output image iterator is something that we need to be able to change so it isn't const
    itk::ImageRegionIterator< TImageType > outputIterator(m_outputImage, m_outputImage->GetBufferedRegion());

    // iterate through every pixel value of the input image
    for (inputIterator.GoToBegin(); !inputIterator.IsAtEnd(); ++inputIterator)
    {
      // copy image index from inputIterator to the outputIterator
      outputIterator.SetIndex(inputIterator.GetIndex());

      outputIterator.Set( // set a pixel value to the index pointed at by the outputIterator
        static_cast<TImageType::PixelType>( // cast the result of the multiplication to the pixel type to circumvent errors
        inputIterator.Get() * m_scaleFactor) // multiply input image pixel by the scale factor
        );
    }

    // at this point, the output is already populate with the resule we want
  }

  //! Get the output here
  typename TImageType::Pointer GetOutput()
  {
    return m_outputImage;
  }

private:
  typename TImageType::Pointer m_inputImage, m_outputImage;

  float m_scaleFactor;
};
