/**
\brief ITK Segmentation Tutorial
*/

//! ITK headers
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"

#include "itkHistogramMatchingImageFilter.h"
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



/**
\brief Get the itk::Image

\param The itk::Image which will contain the image data
\param File name of the image
*/
template <class TImageType>
void SafeReadImage(typename TImageType::Pointer image, const std::string &fName)
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
    return;
  }

  image->Graft(reader->GetOutput());
  return;
}

/**
\brief Apply connected segmentation filter

\param image itk::Image::Pointer to input image
\param outputFileName File name of output
*/
template <typename TImageType>
void PipelineFilter(typename TImageType::Pointer image, const std::string &outputFileName)
{
  auto filter = itk::HistogramMatchingImageFilter< TImageType, TImageType >::New();
  filter->SetInput(inputImage);
  filter->SetReferenceImage(referenceImage);
  if (numberOfHistogramLevels != 100)
  {
    filter->SetNumberOfHistogramLevels(numberOfHistogramLevels);
  }
  filter->ThresholdAtMeanIntensityOn();
  filter->SetNumberOfMatchPoints(numberOfMatchPoints);
  filter->Update();

  typedef itk::Image<short, 3> OImageType;
  typedef itk::ConnectedThresholdImageFilter<TImageType, OImageType> ConnectedFilterType;
  typename ConnectedFilterType::Pointer filter = ConnectedFilterType::New();
  filter->SetInput(image);

  filter->SetReplaceValue(1000);
  filter->SetLower(1100);
  filter->SetUpper(2000);

  typename TImageType::IndexType index;
  // place a random seed point - values are in accordance with example data
  index[0] = 90;
  index[1] = 120;
  index[2] = 67;

  filter->AddSeed(index);
  //filter->AddSeed(index);
  filter->Update();

  typedef itk::ImageFileWriter<OImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputFileName);

  writer->SetInput(filter->GetOutput());
  writer->Update();
}

void echoUsage(const std::string &exeName)
{
  std::cout << exeName << " <inputImageFile> <outputFileName>\n" <<
    "NOTE - Only 3D images are supported in this example.\n";
}

// main entry of program
int main(int argc, char *argv[])
{
  try // to catch exceptions
  {
    // basic check to see image file has been put in by the user
    if( (argc < 2) || (argc > 2) )
    {
      std::cerr << "Usage: " << std::endl;
      echoUsage(argv[0]);
      return EXIT_FAILURE;
    }

    std::string inputFName1 = "", outputFName = "";

    inputFName1 = argv[2];
    outputFName = argv[3];

    itk::ImageIOBase::Pointer im_base = itk::ImageIOFactory::CreateImageIO(inputFName1.c_str(), itk::ImageIOFactory::ReadMode);
    im_base->SetFileName(inputFName1);
    im_base->ReadImageInformation();
    
    // perform basic sanity check
    if (im_base->GetNumberOfDimensions() != 3)
    {
      std::cerr << "Unsupported Image Dimension. Only 3D images are currently supported.\n";
      return EXIT_FAILURE;
    }

    typedef float PixelType; // default pixel type is float, all voxel data is static-casted
    typedef itk::Image<PixelType, 3> ImageType; // define image type
    ImageType::Pointer image_1 = ImageType::New(); // initialize new image
    SafeReadImage<ImageType>(image_1, im_base->GetFileName()); // read image along with exceptions
    
    std::cout << "Doing connectivity segmentation...\n";
    segmentationFilter<ImageType>(image_1, outputFName);
    
  }
  catch (itk::ExceptionObject &error)
  {
    std::cerr << "Exception caught: " << error << "\n";
    return EXIT_FAILURE;
  }
  
  std::cout << "Finished successfully.\n";
  return EXIT_SUCCESS;
}