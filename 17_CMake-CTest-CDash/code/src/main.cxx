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

template< typename TImageType, typename TOutputImageType = TImageType >
void helperFunction(const std::string &inputImageFile, const std::string &outputImageFile, float inputScale)
{
  TImageType::Pointer inputImage = TImageType::New(), outputImage = TImageType::New();

  // read the input image
  inputImage = cbica::ReadImage<TImageType>(inputImageFile);
  auto application = BasicApp<TImageType>();

  // set the application parameters
  application.SetInputImage(inputImage);
  application.SetScale(inputScale);
  application.Run();

  // get the output
  outputImage = application.GetOutput();

  // write the image
  cbica::WriteImage<TImageType, TOutputImageType>(outputImage, outputImageFile);

  return;
}

// main entry of program
int main(int argc, char *argv[])
{
  cbica::CmdParser parser = cbica::CmdParser(argc, argv);
  parser.addRequiredParameter("i", "inputImage", cbica::Parameter::FILE, ".nii.gz", "This is the input image file to be passed", "Can be 3D or 2D image");
  parser.addRequiredParameter("o", "outputImage", cbica::Parameter::FILE, ".nii.gz", "This is the output image file to be passed", "Can be 3D or 2D image");
  parser.addOptionalParameter("s", "scale", cbica::Parameter::FLOAT, "-1000 to 1000", "This is the scaling factor for the app", "Defaults to '1'");

  // check for default arguments
  if (argc < 2)
  {
    parser.echoUsage();
    return EXIT_FAILURE;
  }

  if (parser.isPresent("u"))
  {
    parser.echoUsage();
    return EXIT_SUCCESS;
  }

  if (parser.isPresent("h"))
  {
    parser.echoHelp();
    return EXIT_SUCCESS;
  }

  if (parser.isPresent("v"))
  {
    parser.echoVersion();
    return EXIT_SUCCESS;
  }

  std::string inputFile, outputFile;
  float scale = 1;
  
  // get input and output file names from the command line
  parser.getParameterValue("i", inputFile);
  parser.getParameterValue("o", outputFile);

  if (parser.isPresent("s"))
  {
    parser.getParameterValue("s", scale);
  }
  
  // check for supported parameters
  itk::ImageIOBase::Pointer im_base = itk::ImageIOFactory::CreateImageIO(inputFile.c_str(), itk::ImageIOFactory::ReadMode);
  im_base->SetFileName(inputFile);
  im_base->ReadImageInformation();

  switch (im_base->GetNumberOfDimensions())
  {
  case 2:
  {
    using ImageType = itk::Image< float, 2 >;
    using OutputImageType = itk::Image< float, 2 >;
    
    helperFunction< ImageType, OutputImageType >(inputFile, outputFile, scale);

    break;
  }
  case 3:
  {
    using ImageType = itk::Image< float, 3 >;
    using OutputImageType = itk::Image< float, 3 >;

    helperFunction< ImageType, OutputImageType >(inputFile, outputFile, scale);

    break;
  }
  default:
  {
    std::cerr << "Unsupported Image Dimension. Only 2D or 3D images are currently supported.\n";
    parser.echoUsage();
    return EXIT_FAILURE;
  }
  }
  
  std::cout << "Finished successfully.\n";
  return EXIT_SUCCESS;
}