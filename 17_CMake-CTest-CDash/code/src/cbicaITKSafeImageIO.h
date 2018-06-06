/**
\file  cbicaITKSafeImageIO.h

\brief Defines safe input and output of itk::Images

Read and Write itk::Image data in a safe manner. Header-only

https://www.cbica.upenn.edu/sbia/software/ <br>
software@cbica.upenn.edu

Copyright (c) 2016 University of Pennsylvania. All rights reserved. <br>
See COPYING file or https://www.cbica.upenn.edu/sbia/software/license.html

*/
#pragma once

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"
#include "itkCastImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkImageIOBase.h"
#include "itkImageIOFactory.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"

#include "cbicaUtilities.h"

typedef itk::Image< float, 3 > ImageTypeFloat3D;

namespace cbica
{
  /**
  \brief Get the itk::ImageFileReader from input file name. This is useful for scenarios where reader meta information is needed for later writing step(s).

  Usage:
  \verbatim
  typedef itk::Image< float, 3 > ExpectedImageType;
  std::string inputFileName = parser.getParameterValue("inputImage");
  ExpectedImageType::Pointer inputImage_1 = GetImageReader< ExpectedImageType >(inputFileName)->GetOutput();
  ExpectedImageType::Pointer inputImage_2 = GetImageReader< ExpectedImageType >(inputFileName, ".nii.gz,.img")->GetOutput();
  DoAwesomeStuffWithImage( inputImage );
  \endverbatim

  \param fName name of the image
  \param supportedExtensions Supported extensions, defaults to ".nii.gz,.nii"
  \return itk::ImageFileReader::Pointer templated over the same as requested by user
  */
  template <class TImageType = ImageTypeFloat3D >
  typename itk::ImageFileReader< TImageType >::Pointer GetImageReader(const std::string &fName, const std::string &supportedExtensions = ".nii.gz,.nii", const std::string &delimitor = ",")
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
    if (im_base->GetNumberOfDimensions() != TImageType::ImageDimension)
    {
      std::cerr << "Image Dimension mismatch. Return image is expected to be '" << TImageType::ImageDimension <<
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

    return reader;
  }

  /**
  \brief Get the itk::Image from input file name

  Usage:
  \verbatim
  typedef itk::Image< float, 3 > ExpectedImageType;
  std::string inputFileName = parser.getParameterValue("inputImage");
  ExpectedImageType::Pointer inputImage_1 = ReadImage< ExpectedImageType >(inputFileName);
  ExpectedImageType::Pointer inputImage_2 = ReadImage< ExpectedImageType >(inputFileName, ".nii.gz,.img");
  DoAwesomeStuffWithImage( inputImage );
  \endverbatim

  \param fName name of the image
  \param supportedExtensions Supported extensions, defaults to ".nii.gz,.nii"
  \return itk::ImageFileReader::Pointer templated over the same as requested by user
  */
  template <class TImageType = ImageTypeFloat3D >
  typename TImageType::Pointer ReadImage(const std::string &fName, const std::string &supportedExtensions = ".nii.gz,.nii", const std::string &delimitor = ",")
  {
    return GetImageReader< TImageType >(fName, supportedExtensions, delimitor)->GetOutput();
  }

  /**
  \brief Get the itk::Image from input file name

  Usage:
  \verbatim
  typedef itk::Image< float, 3 > ExpectedImageType;
  std::string inputFileName = parser.getParameterValue("inputImage");
  ExpectedImageType::Pointer inputImage_1 = ReadImage< ExpectedImageType >(inputFileName);
  ExpectedImageType::Pointer inputImage_2 = ReadImage< ExpectedImageType >(inputFileName, ".nii.gz,.img");
  DoAwesomeStuffWithImage( inputImage );
  \endverbatim

  This function calls Same as ReadImage internally
  \param fName name of the image
  \param supportedExtensions Supported extensions, defaults to ".nii.gz,.nii"
  \return itk::ImageFileReader::Pointer templated over the same as requested by user
  */
  template <class TImageType = ImageTypeFloat3D >
  typename TImageType::Pointer GetImage(const std::string &fName, const std::string &supportedExtensions = ".nii.gz,.nii", const std::string &delimitor = ",")
  {
    return ReadImage< TImageType >(fName, supportedExtensions, delimitor);
  }

  /**
  \brief Get the Dicom image reader (not the image, the READER). This is useful for scenarios where reader meta information is needed for later writing step(s).

  Usage:
  \verbatim
  typedef itk::Image< float, 3 > ExpectedImageType;
  std::string inputDirName = parser.getParameterValue("inputDirName");
  ExpectedImageType::Pointer inputImage = GetDicomImageReader< ExpectedImageType >(inputDirName)->GetOutput(); // reads MRI and perfusion data by default tags "0008|0021,0020|0012"
  ExpectedImageType::Pointer inputImage = GetDicomImageReader< ExpectedImageType >(inputDirName, "0008|0021")->GetOutput(); // only reads images with tag "0008|0021"
  DoAwesomeStuffWithImage( inputImage );
  \endverbatim

  \param dirName This is the directory name of the DICOM image which needs to be loaded - if this is an image, the underlying path of the image is considered
  \param seriesRestrictions The DICOM series restriction(s) based on which the reader needs to act on - defaults to MRI and perfusion and delimitor is always ','
  */
  template <class TImageType = ImageTypeFloat3D >
  typename itk::ImageSeriesReader< TImageType >::Pointer GetDicomImageReader(const std::string &dirName, const std::string &seriesRestrictions = "0008|0021,0020|0012")
  {
    std::string dirName_wrap = dirName;
    if (!cbica::isDir(dirName_wrap))
    {
      dirName_wrap = cbica::getFilenamePath(dirName);
    }

    typedef std::vector< std::string > SeriesIdContainer;
    SeriesIdContainer seriesToRead = cbica::stringSplit(seriesRestrictions, ",");

    typedef itk::ImageSeriesReader< TImageType > ReaderType;
    typename ReaderType::Pointer seriesReader = ReaderType::New();

    typedef itk::GDCMImageIO ImageIOType;
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    seriesReader->SetImageIO(dicomIO);
    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer namesGenerator = NamesGeneratorType::New();
    namesGenerator->SetUseSeriesDetails(true);
    for (size_t i = 0; i < seriesToRead.size(); i++)
    {
      namesGenerator->AddSeriesRestriction(seriesToRead[i]);
    }
    namesGenerator->SetInputDirectory(dirName_wrap);

    const SeriesIdContainer &seriesUID = namesGenerator->GetSeriesUIDs();

    SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
    SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
    while (seriesItr != seriesEnd)
    {
      std::vector< std::string > fileNames;
      fileNames = namesGenerator->GetFileNames(seriesItr->c_str());
      seriesReader->SetFileNames(fileNames);
      try
      {
        seriesReader->Update();
      }
      catch (itk::ExceptionObject & err)
      {
        std::cerr << "Error while loading DICOM images: " << err.what() << "\n";
      }
      ++seriesItr;
    }

    return seriesReader;
  }

  /**
  \brief Get the itk::Image from input dir name

  Usage:
  \verbatim
  typedef itk::Image< float, 3 > ExpectedImageType;
  std::string inputDirName = parser.getParameterValue("inputDirName");
  ExpectedImageType::Pointer inputImage_1 = ReadDicomImage< ExpectedImageType >(inputFileName); // reads MRI and perfusion data by default tags "0008|0021,0020|0012"
  ExpectedImageType::Pointer inputImage_2 = ReadDicomImage< ExpectedImageType >(inputDirName, "0008|0021")->GetOutput(); // only reads images with tag "0008|0021"
  DoAwesomeStuffWithImage( inputImage );
  \endverbatim

  \param fName name of the image
  \param supportedExtensions Supported extensions
  \return itk::ImageFileReader::Pointer templated over the same as requested by user
  */
  template <class TImageType = ImageTypeFloat3D >
  typename TImageType::Pointer ReadDicomImage(const std::string &dirName, const std::string &seriesRestrictions = "0008|0021,0020|0012")
  {
    return GetDicomImageReader< TImageType >(dirName, seriesRestrictions)->GetOutput();
  }

  /**
  \brief Get the itk::Image from input dir name

  Usage:
  \verbatim
  typedef itk::Image< float, 3 > ExpectedImageType;
  std::string inputDirName = parser.getParameterValue("inputDirName");
  ExpectedImageType::Pointer inputImage_1 = ReadDicomImage< ExpectedImageType >(inputFileName); // reads MRI and perfusion data by default tags "0008|0021,0020|0012"
  ExpectedImageType::Pointer inputImage_2 = ReadDicomImage< ExpectedImageType >(inputDirName, "0008|0021")->GetOutput(); // only reads images with tag "0008|0021"
  DoAwesomeStuffWithImage( inputImage );
  \endverbatim

  This function calls ReadDicomImage<> internally

  \param fName name of the image
  \param supportedExtensions Supported extensions
  \return itk::ImageFileReader::Pointer templated over the same as requested by user
  */
  template <class TImageType
#if (_MSC_VER >= 1800) || (__GNUC__ > 4)
    = ImageTypeFloat3D
#endif
    >
    typename TImageType::Pointer GetDicomImage(const std::string &dirName, const std::string &seriesRestrictions = "0008|0021,0020|0012")
  {
    return ReadDicomImage< TImageType >(dirName, seriesRestrictions);
  }


  /**
  \brief Write the itk::Image to the file name

  Usage:
  \verbatim
  typedef itk::Image< float, 3 > ComputedImageType;
  typedef itk::Image< unsigned char, 3 > WrittenImageType;
  ComputedImageType::Pointer imageToWrite = ComputedImageType::New();
  imageToWrite = GetImageSomehow();
  WriteImage< ComputedImageType >(imageToWrite, fileNameToWriteImage); // casts imageToWrite to WrittenImageType
  WriteImage< ComputedImageType, WrittenImageType >(imageToWrite, fileNameToWriteImage);  // writes imageToWrite as ComputedImageType
  // at this point, the image has already been written
  \endverbatim

  \param inputImage Pointer to processed image data which is to be written
  \param fileName File containing the image
  \return itk::Image of specified pixel and dimension type
  */
  template <typename ComputedImageType = ImageTypeFloat3D, typename ExpectedImageType = ComputedImageType>
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

  /**
  \brief Write the itk::Image as a DICOM to the specified directory

  Usage:
  \verbatim
  typedef itk::Image< float, 3 > ComputedImageType;
  typedef itk::Image< unsigned char, 3 > WrittenImageType;
  itk::ImageSeriesReader< ComputedImageType >::Pointer inputImageReader = GetDicomImageReader< ComputedImageType >(inputDirName);
  ComputedImageType::Pointer imageToWrite = GetImageAfterProcessing( inputImageReader->GetOutput() );
  WriteImage< ComputedImageType, WrittenImageType >(inputImageReader, imageToWrite, dirNameToWriteImage); // casts imageToWrite to WrittenImageType
  WriteImage< ComputedImageType >(inputImageReader, imageToWrite, dirNameToWriteImage); // writes imageToWrite as ComputedImageType
  // at this point, the image has already been written
  \endverbatim

  \param inputImageReader The image reader for DICOM - this is necessary to populate the DICOM dictionary properly
  \param imageToWrite Pointer to processed image data which is to be written
  \param dirName File containing the image
  \return itk::Image of specified pixel and dimension type
  */
  template <typename ComputedImageType, typename ExpectedImageType = ComputedImageType>
  void WriteDicomImage(const typename itk::ImageSeriesReader< ComputedImageType >::Pointer inputImageReader, const typename ComputedImageType::Pointer imageToWrite, const std::string &dirName)
  {
    if (!cbica::isDir(dirName))
    {
      std::cout << "Specified directory wasn't found, creating...\n";
      cbica::createDir(dirName);
    }
    typedef itk::CastImageFilter<ComputedImageType, ExpectedImageType> CastFilterType;
    typename CastFilterType::Pointer castFilter = CastFilterType::New();
    castFilter->SetInput(imageToWrite);
    castFilter->Update();

    typedef typename ExpectedImageType::PixelType DicomPixelType;

    typedef itk::GDCMImageIO ImageIOType;
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    dicomIO->SetMetaDataDictionary(inputImageReader->GetMetaDataDictionary());

    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer namesGenerator = NamesGeneratorType::New();
    namesGenerator->SetUseSeriesDetails(false);
    namesGenerator->SetOutputDirectory(dirName);
    namesGenerator->Update();

    typedef itk::Image<DicomPixelType, 2> DicomImage2DType; // dicom images are always 2D
    typedef itk::ImageSeriesWriter<ExpectedImageType, DicomImage2DType> SeriesWriterType;
    typename SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();
    seriesWriter->SetInput(castFilter->GetOutput());
    seriesWriter->SetImageIO(dicomIO);
    seriesWriter->SetFileNames(namesGenerator->GetOutputFileNames());
    seriesWriter->SetMetaDataDictionaryArray(inputImageReader->GetMetaDataDictionaryArray());

    try
    {
      seriesWriter->Write();
    }
    catch (itk::ExceptionObject &e)
    {
      std::cerr << "Error occurred while trying to write the image '" << dirName << "': " << e.what() << "\n";
      exit(EXIT_FAILURE);
    }

  }
}
