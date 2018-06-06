#include <iostream>
#include <type_traits>
#include <algorithm>

#include "itkImage.h"
#include "itkImageIOBase.h"
#include "itkImageIOFactory.h"
#include "itkImageFileReader.h"

#include "cbicaCmdParser.h"
#include "cbicaUtilities.h"
#include "cbicaITKSafeImageIO.h"

#include "BasicApp.h"

// main entry of program
int main(int argc, char *argv[])
{
  cbica::CmdParser parser = cbica::CmdParser(argc, argv);
  parser.addOptionalParameter("r", "runTest", cbica::Parameter::FILE, ".nii.gz", "This takes the input image file for testing");

  std::string inputFile;
  parser.getParameterValue("r", inputFile);

  const unsigned int Dimensions = 2;

  typedef itk::Image< float, Dimensions > ImageType;
  ImageType::Pointer inputImage = ImageType::New();

  inputImage = cbica::ReadImage<ImageType>(inputFile);

  BasicApp<ImageType> app = BasicApp<ImageType>();
  app.SetInputImage(inputImage);
  app.SetScale(1);
  app.Run();

  ImageType::Pointer outputImage = app.GetOutput();

  // check overall size, spacing and origin of the images
  for (size_t i = 0; i < Dimensions; i++)
  {
    if (inputImage->GetBufferedRegion().GetSize()[i] != outputImage->GetBufferedRegion().GetSize()[i])
    {
      return EXIT_FAILURE;
    }
    if (inputImage->GetSpacing()[i] != outputImage->GetSpacing()[i])
    {
      return EXIT_FAILURE;
    }
    if (inputImage->GetOrigin()[i] != outputImage->GetOrigin()[i])
    {
      return EXIT_FAILURE;
    }
  }

  itk::ImageRegionConstIterator<ImageType> inputImageIterator(inputImage, inputImage->GetBufferedRegion()),
    outputImageIterator(outputImage, outputImage->GetBufferedRegion());

  // check every single pixel/voxel location
  for (inputImageIterator.GoToBegin(), outputImageIterator.GoToBegin();
    !inputImageIterator.IsAtEnd(), !outputImageIterator.IsAtEnd();
    ++inputImageIterator, ++outputImageIterator)
  {
    if (inputImageIterator.Get() != outputImageIterator.Get())
    {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}