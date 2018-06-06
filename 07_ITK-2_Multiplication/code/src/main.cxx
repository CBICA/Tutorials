/**
\brief ITK Multiplication Tutorial
*/

//! ITK headers
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"

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

#include "itkMultiplyImageFilter.h"

#include <itkCorrelationCoefficientHistogramImageToImageMetric.h>


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
\brief Apply multiplication filter

\param image_1 itk::Image::Pointer to first image
\param image_2 itk::Image::Pointer to second image
\param fOutName File name of output 
*/
template <typename TImageType>
void multiplicationFilter(typename TImageType::Pointer image_1,
  typename TImageType::Pointer image_2,
  const std::string &fOutName)
{
  typedef itk::MultiplyImageFilter<TImageType, TImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();

  filter->SetInput1(image_1);
  filter->SetInput2(image_2);
  filter->Update();
  //typename TImageType::Pointer result = filter->GetOutput();

  typedef itk::ImageFileWriter<TImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetInput(filter->GetOutput());
  writer->SetFileName(fOutName);
  writer->Write();
}

void echoUsage(const std::string &exeName)
{
  std::cout << exeName << " <inputImageFile1> <inputImageFile2> <outputFileName>\n" <<
    "NOTE - Only 3D images are supported in this example.\n";
}

// main entry of program
int main(int argc, char *argv[])
{
  try // to catch exceptions
  {
    // basic check to see image file has been put in by the user
    if( (argc < 2) )
    {
      std::cerr << "Usage: " << std::endl;
      echoUsage(argv[0]);
      return EXIT_FAILURE;
    }

    std::string inputFName1 = "", inputFName2 = "", outputFName = "";
    
    inputFName1 = argv[2];
    inputFName2 = argv[3];
    inputMask2 = argv[4];
    outputFName = argv[5];

    // perform sanity check
    itk::ImageIOBase::Pointer im_base = itk::ImageIOFactory::CreateImageIO(inputFName1.c_str(), itk::ImageIOFactory::ReadMode);
    im_base->SetFileName(inputFName1);
    im_base->ReadImageInformation();

    itk::ImageIOBase::Pointer im_base_2 = itk::ImageIOFactory::CreateImageIO(inputFName2.c_str(),
    im_base_2->SetFileName(inputFName2);
    im_base_2->ReadImageInformation();
    
    if (im_base->GetNumberOfDimensions() != im_base_2->GetNumberOfDimensions())
    {
      std::cerr << "Image dimension mismatch between images 1 & 2. Please check files\n" <<
        inputFName1 << " and " << inputFName2 << "\n";
      return EXIT_FAILURE;
    }   
    else if (im_base->GetNumberOfDimensions() != 3)
    {
      std::cerr << "Unsupported Image Dimension. Only 3D images are currently supported.\n";
      return EXIT_FAILURE;
    }

    typedef float PixelType; // default pixel type is float, all voxel data is static-casted
    typedef itk::Image<PixelType, 3> ImageType; // define image type
    ImageType::Pointer image_1 = ImageType::New(); // initialize new image
    SafeReadImage<ImageType>(image_1, im_base->GetFileName()); // read image along with exceptions
    
    std::cout << "Doing multiplication...\n";
    ImageType::Pointer image_2 = ImageType::New();
    SafeReadImage<ImageType>(image_2, inputFName2);
    multiplicationFilter<ImageType>(image_1, image_2, outputFName);        
  }
  catch (itk::ExceptionObject &error)
  {
    std::cerr << "Exception caught: " << error << "\n";
    return EXIT_FAILURE;
  }
  
  std::cout << "Finished successfully.\n";
  return EXIT_SUCCESS;
}