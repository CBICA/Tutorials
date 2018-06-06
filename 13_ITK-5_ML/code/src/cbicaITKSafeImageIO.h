/**
\file  cbicaITKSafeImageIO.h

\brief Defines safe input and output of itk::Images

Read and Write itk::Image data in a safe manner. Header-only

https://www.cbica.upenn.edu/sbia/software/ <br>
software@cbica.upenn.edu

Copyright (c) 2015 University of Pennsylvania. All rights reserved. <br>
See COPYING file or https://www.cbica.upenn.edu/sbia/software/license.html

*/
#pragma once

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkCastImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkImageIOBase.h"
#include "itkImageIOFactory.h"

#include "cbicaUtilities.h"

//typedef itk::Image< float, 3 > TImageType; // debugging purposes only

namespace cbica
{
  /**
  \brief Get the itk::Image from input file name 

  Usage:
  \verbatim
  typedef itk::Image< float, 3 > ExpectedImageType;
  std::string inputFileName = parser.getParameterValue("inputImage");
  ExpectedImageType::Pointer inputImage = ReadImage<ExpectedImageType>(inputFileName);
  ExpectedImageType::Pointer inputImage_2 = ReadImage<ExpectedImageType>(inputFileName, ".nii.gz");
  DoAwesomeStuffWithImage( inputImage );
  \endverbatim

  \param fName name of the image
  \param supportedExtensions Supported extensions
  \return itk::Image templated over the same as requested by user
  */
  template <class TImageType>
  typename TImageType::Pointer ReadImage(const std::string &fName, const std::string &supportedExtensions = "", const std::string &delimitor = ",")
  {
    if (supportedExtensions != "")
    {
      std::vector< std::string > extensions = cbica::stringSplit(supportedExtensions, delimitor);

      bool supportedExtensionFound = false;
      for (size_t i = 0; i < extensions.size(); i++)
      {
        if (extensions[i] == cbica::getFilenameExtension(fName))
        {
          supportedExtensionFound = true;
        }
      }

      if (!supportedExtensionFound)
      {
        std::cerr << "Supplied file name '" << fName << "' doesn't have a supported extension. \nSupported Extensions: " << supportedExtensions << "\n";
        exit(EXIT_FAILURE);
      }
    }

	// ensure that the requested image dimensions and read image dimensions match up
    itk::ImageIOBase::Pointer im_base = itk::ImageIOFactory::CreateImageIO(fName.c_str(), itk::ImageIOFactory::ReadMode);
    im_base->SetFileName(fName);
    im_base->ReadImageInformation();

    // perform basic sanity check
    if (im_base->GetNumberOfDimensions() != typename TImageType::ImageDimension)
    {
      std::cerr << "Image Dimension mismatch. Return image is expected to be '" << typename TImageType::ImageDimension << 
        "'D and doesn't match the image dimension read from the input file, which is '" << im_base->GetNumberOfDimensions() << "'.\n";
      exit(EXIT_FAILURE);
    }

    typedef itk::ImageFileReader< TImageType > ImageReaderType;
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName(fName);

    try
    {
      reader->Update();
    }
    catch (itk::ExceptionObject& e)
    {
      std::cerr << "Exception caught while reading the image '" << fName << "': " << e.what() << "\n";
      exit(EXIT_FAILURE);
    }

    return reader->GetOutput();
  }

  /**
  \brief Write the itk::Image to the file name

  Usage:
  \verbatim
  typedef itk::Image< float, 3 > ProcessedImageType;
  typedef itk::Image< unsigned char, 3 > WrittenImageType;
  ProcessedImageType::Pointer outputImage = ProcessedImageType::New();
  outputImage = GetImageSomehow();
  WriteImage< ProcessedImageType, WrittenImageType >(fileNameToWriteImage); 
  // at this point, the image has already been written
  \endverbatim

  \param inputImage Pointer to processed image data which is to be written
  \param fileName File containing the image
  \return itk::Image of specified pixel and dimension type
  */
  template <typename ComputedImageType, typename ExpectedImageType>
  void WriteImage(typename ComputedImageType::Pointer imageToWrite, const std::string &fileName)
  {
    typedef itk::CastImageFilter<ComputedImageType, ExpectedImageType> CastFilterType;
    typename CastFilterType::Pointer filter = CastFilterType::New();    
    filter->SetInput(imageToWrite);
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
      std::cerr << "Error occurred while trying to write the image '" << fileName << "': " << e.what() << "\n";
      exit(EXIT_FAILURE);
    }

    return;
  }

}