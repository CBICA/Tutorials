/**
\brief ITK Registration Tutorial
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
\brief Apply the registration filter

\param fixedImage itk::Image::Pointer to fixed image
\param movingImage itk::Image::Pointer to moving image
\param maskImage itk::Image::Pointer to mask of fixed image
\param outputFileName File name of output
*/
template <typename TImageType, typename TMaskImageType>
void registrationFilter(typename TImageType::Pointer fixedImage,
  typename TImageType::Pointer movingImage,
  typename TMaskImageType::Pointer maskImage,
  const std::string &outputFileName)
{
  // convert mask image to input image type
  typedef itk::CastImageFilter<TMaskImageType, TImageType> CastFilterType;
  typename CastFilterType::Pointer cast_filter = CastFilterType::New();
  cast_filter->SetInput(maskImage);
  cast_filter->Update();
  typename TImageType::Pointer mask = cast_filter->GetOutput();

  typedef itk::ImageRegistrationMethod<TImageType, TImageType> RegistrationType;
  typedef itk::AffineTransform<double, 3> TransformType;
  typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
  typedef itk::MeanSquaresImageToImageMetric<TImageType, TImageType> MetricType;
  typedef itk::LinearInterpolateImageFunction<TImageType, double> InterpolatorType;
  typedef itk::NearestNeighborInterpolateImageFunction<TImageType, double> NNInterpolatorType;

  typename MetricType::Pointer metric = MetricType::New(); // typename required here because template class used -> syntax
  typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
  typename NNInterpolatorType::Pointer nn_interpolator = NNInterpolatorType::New();
  typename RegistrationType::Pointer registration = RegistrationType::New();
  TransformType::Pointer transform = TransformType::New();
  OptimizerType::Pointer optimizer = OptimizerType::New();

  // set the parameters 
  registration->SetMetric(metric);
  registration->SetOptimizer(optimizer);
  registration->SetTransform(transform);
  registration->SetInterpolator(interpolator);
  //registration->SetInterpolator(nn_interpolator);

  // set the inputs
  registration->SetFixedImage(fixedImage);
  registration->SetMovingImage(movingImage);
  registration->SetFixedImageRegion(fixedImage->GetLargestPossibleRegion());

  typename RegistrationType::ParametersType initialParameters(transform->GetNumberOfParameters());

  initialParameters[0] = 1.0; // rotation
  initialParameters[1] = 0.0;
  initialParameters[2] = 0.0;
  initialParameters[3] = 0.0;
  initialParameters[4] = 1.0;
  initialParameters[5] = 0.0;
  initialParameters[6] = 0.0;
  initialParameters[7] = 0.0;
  initialParameters[8] = 1.0;
  initialParameters[9] = 0.0; // translation
  initialParameters[10] = 0.0;
  initialParameters[11] = 0.0;

  registration->SetInitialTransformParameters(initialParameters);

  optimizer->SetMaximumStepLength(0.25);
  optimizer->SetMinimumStepLength(0.0001);
  optimizer->SetNumberOfIterations(20);

  registration->Update();

  typename RegistrationType::ParametersType finalParameters = registration->GetLastTransformParameters();
  std::cout << "Final parameters: " << finalParameters << std::endl;

  // apply transformation matrix to moving image
  typedef itk::ResampleImageFilter<TImageType, TImageType> ResampleFilterType;
  typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
  
  resampler->SetInput(movingImage);
  resampler->SetTransform(registration->GetOutput()->Get());
  resampler->SetSize(movingImage->GetLargestPossibleRegion().GetSize());
  resampler->SetOutputOrigin(movingImage->GetOrigin());
  resampler->SetOutputSpacing(movingImage->GetSpacing());
  resampler->SetOutputDirection(movingImage->GetDirection());
  resampler->SetDefaultPixelValue(0);
  resampler->SetInterpolator(interpolator);

  typedef itk::ImageFileWriter<TImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputFileName);
  writer->SetInput(resampler->GetOutput());
  writer->Update();
}

void echoUsage(const std::string &exeName)
{
  std::cout << exeName << " <fixedImage> <movingImage> <outputFileName> <movingImageMask>\n" <<
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

    std::string inputFName1 = "", inputFName2 = "", inputMask2 = "", outputFName = "";
    bool segFlag = false, mulFlag = false, regFlag = false;

    inputFName1 = argv[2];
    inputFName2 = argv[3];
    inputMask2 = argv[4];
    outputFName = argv[5];

    //std::string iterations_string = argv[5];
    //outputFName = outputFName + iterations_string + ".nii";
    //unsigned int iterations = std::atoi(argv[5]);

    itk::ImageIOBase::Pointer im_base = itk::ImageIOFactory::CreateImageIO(inputFName1.c_str(), itk::ImageIOFactory::ReadMode);
    im_base->SetFileName(inputFName1);
    im_base->ReadImageInformation();

    itk::ImageIOBase::Pointer im_base_2 = itk::ImageIOFactory::CreateImageIO(inputFName2.c_str(),
    im_base_2->SetFileName(inputFName2);
    im_base_2->ReadImageInformation();
    
    if (im_base->GetComponentType() != im_base_2->GetComponentType())
    {
      std::cerr << "Image type mismatch between images 1 & 2. Please check files\n" <<
        inputFName1 << " and " << inputFName2 << "\n";
      return EXIT_FAILURE;
    }
    else if (im_base->GetNumberOfDimensions() != im_base_2->GetNumberOfDimensions())
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

    itk::ImageIOBase::Pointer im_base_mask;
    typedef itk::Image<unsigned char, 3> MaskImageType; // assume mask to be in short range
    typedef itk::ImageFileReader<MaskImageType> MaskReaderType;
    MaskReaderType::Pointer mask_reader = MaskReaderType::New();

    im_base_mask = itk::ImageIOFactory::CreateImageIO(inputMask2.c_str(), itk::ImageIOFactory::ReadMode);
    
    im_base_mask->SetFileName(inputMask2);
    im_base_mask->ReadImageInformation();
    
    mask_reader->SetFileName(inputMask2);
    mask_reader->Update();
    
    if (im_base_mask->GetNumberOfDimensions() != 3)
    {
        std::cerr << "Unsupported Image Dimension for image mask.\n";
        return EXIT_FAILURE;
    }
    
    typedef float PixelType; // default pixel type is float, all voxel data is static-casted
    typedef itk::Image<PixelType, 3> ImageType; // define image type
    ImageType::Pointer image_1 = ImageType::New(); // initialize new image
    SafeReadImage<ImageType>(image_1, im_base->GetFileName()); // read image along with exceptions
    
    std::cout << "Doing registration...\n";
    ImageType::Pointer image_2 = ImageType::New();
    SafeReadImage<ImageType>(image_2, inputFName2);
    registrationFilter<ImageType, MaskImageType>(image_1, image_2, mask_reader->GetOutput(),
        
  }
  catch (itk::ExceptionObject &error)
  {
    std::cerr << "Exception caught: " << error << "\n";
    return EXIT_FAILURE;
  }
  
  std::cout << "Finished successfully.\n";
  return EXIT_SUCCESS;
}