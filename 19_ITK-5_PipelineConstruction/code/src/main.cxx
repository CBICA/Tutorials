/**
\brief ITK Segmentation Tutorial
*/

#include <chrono>
//! ITK headers
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkOtsuThresholdImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkEllipseSpatialObject.h"
#include "itkImage.h"
#include "itkImageRegistrationMethod.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkResampleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSpatialObjectToImageFilter.h"
#include "itkAffineTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

std::string inputFile, referenceFile, outputFile;

/**
\brief Get the itk::Image

\param The itk::Image which will contain the image data
\param File name of the image
*/
template <class TImageType>
typename TImageType::Pointer SafeReadImage(const std::string &fName)
{
  typedef TImageType ImageType;
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typename ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName(fName);

  try
  {
    reader->Update();
  }
  catch (itk::ExceptionObject& e)
  {
    std::cerr << "Exception caught: " << e.what() << "\n";
    return reader->GetOutput();
  }

  return reader->GetOutput();
}

/**
\brief Apply connected segmentation filter

\param image itk::Image::Pointer to input image
\param outputFileName File name of output
*/
template <typename TImageType>
void PipelineFilter()
{
  // histogram matching
  auto histoMatch = itk::HistogramMatchingImageFilter< TImageType, TImageType >::New();
  histoMatch->SetInput(SafeReadImage< TImageType >(inputFile));
  histoMatch->SetReferenceImage(SafeReadImage< TImageType >(referenceFile));
  histoMatch->SetNumberOfHistogramLevels(125);
  histoMatch->ThresholdAtMeanIntensityOn();
  histoMatch->SetNumberOfMatchPoints(100);
  histoMatch->Update();

  // gaussian filter
  auto gaussianFilter = itk::DiscreteGaussianImageFilter< TImageType, TImageType >::New();
  gaussianFilter->SetInput(histoMatch->GetOutput());
  gaussianFilter->SetVariance(5.0);
  gaussianFilter->Update();

  // otsu threshold
  auto otsuThreshold = itk::OtsuThresholdImageFilter< TImageType, TImageType >::New();
  otsuThreshold->SetInput(gaussianFilter->GetOutput());
  otsuThreshold->Update();
  
  auto writer = itk::ImageFileWriter< TImageType >::New();
  writer->SetFileName(outputFile);
  writer->SetInput(otsuThreshold->GetOutput());
  writer->Update();
}

void echoUsage(const std::string &exeName)
{
  std::cout << exeName << " <inputImageFile> <referenceImageFile> <outputFileName>\n" <<
    "NOTE - Only 3D images are supported in this example.\n";
}

// main entry of program
int main(int argc, char *argv[])
{
  auto t1 = std::chrono::high_resolution_clock::now();
  try // to catch exceptions
  {
    // basic check to see image file has been put in by the user
    if( (argc < 3) || (argc > 4) )
    {
      std::cerr << "Usage: " << std::endl;
      echoUsage(argv[0]);
      return EXIT_FAILURE;
    }

    inputFile = argv[1];
    referenceFile = argv[2];
    outputFile = argv[3];

    //auto im_base = itk::ImageIOFactory::CreateImageIO(inputFile.c_str(), itk::ImageIOFactory::ReadMode);
    //im_base->ReadImageInformation();

    //auto im_base_2 = itk::ImageIOFactory::CreateImageIO(referenceFile.c_str(), itk::ImageIOFactory::ReadMode);
    //im_base_2->ReadImageInformation();

    //// perform basic sanity check
    //if (im_base->GetNumberOfDimensions() != 3)
    //{
    //  std::cerr << "Unsupported Image Dimension. Only 3D images are currently supported.\n";
    //  return EXIT_FAILURE;
    //}
    //if (im_base_2->GetNumberOfDimensions() != 3)
    //{
    //  std::cerr << "Unsupported Image Dimension. Only 3D images are currently supported.\n";
    //  return EXIT_FAILURE;
    //}

    std::cout << "Starting pipeline.\n";

    PipelineFilter< itk::Image< float, 3 > >();
  }
  catch (itk::ExceptionObject &error)
  {
    std::cerr << "Exception caught: " << error << "\n";
    return EXIT_FAILURE;
  }

  auto t2 = std::chrono::high_resolution_clock::now();
  std::cout << "Finished successfully in " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " milliseconds\n";
  return EXIT_SUCCESS;
}