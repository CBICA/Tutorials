/**
\file  cbicaReadUnknownImage.h

\brief Declaration of the ReadUnknownImage class

Using the file name, reads an image and returns an itk::Image<>.

https://www.cbica.upenn.edu/sbia/software/ <br>
sbia-software@uphs.upenn.edu

Copyright (c) 2015 University of Pennsylvania. All rights reserved. <br>
See COPYING file or https://www.cbica.upenn.edu/sbia/software/license.html

*/
#pragma once

#include "itkImage.h"
#include "itkCastImageFilter.h"
#include "itkImageFileWriter.h"

namespace cbica
{
  /**
  \brief Write the itk::Image to the file name

  Usage:
  \verbatim
  typedef itk::Image< float, 3 > ProcessedImageType;
  typedef itk::Image< unsigned char, 3 > WrittenImageType;
  ProcessedImageType::Pointer outputImage = ProcessedImageType::New();
  outputImage = GetImageSomehow();
  WriteImage< ProcessedImageType, WrittenImageType >(fileNameToWriteImage); 
  \endverbatim

  \param inputImage Pointer to processed image data which is to be written
  \param fileName File containing the image
  \return itk::Image of specified pixel and dimension type
  */
  template <typename ComputedImageType, typename ExpectedImageType>
  void WriteImage(typename ComputedImageType::Pointer inputImage, const std::string &fileName)
  {
    typedef itk::CastImageFilter<ComputedImageType, ExpectedImageType> CastFilterType;
    typename CastFilterType::Pointer filter = CastFilterType::New();    
    filter->SetInput(inputImage);
    filter->Update();

    typedef typename itk::ImageFileWriter<ExpectedImageType> WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetInput(filter->GetOutput());
    writer->SetFileName(fileName);

    try
    {
      writer->Write();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << "Error occurred while trying to write the image: " << e.what() << "\n";
      exit(EXIT_FAILURE);
    }

    return;
  }

}