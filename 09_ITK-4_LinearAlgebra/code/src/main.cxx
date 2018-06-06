/**
\brief ITK Linear Algebra Tutorial
*/

//! ITK headers
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImportImageFilter.h"

//! VNL headers
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_matrix_ref.h"
#include <vnl/algo/vnl_matrix_inverse.h>

const unsigned int SupportedDimensions = 2;

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

void echoUsage(const std::string &exeName)
{
  std::cout << exeName << " <inputImageFile1> <outputFileName>\n" <<
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

    std::string inputFName1 = "", outputFName = "";
    
    inputFName1 = argv[2];
    outputFName = argv[3];

    // perform sanity check
    itk::ImageIOBase::Pointer im_base = itk::ImageIOFactory::CreateImageIO(inputFName1.c_str(), itk::ImageIOFactory::ReadMode);
    im_base->SetFileName(inputFName1);
    im_base->ReadImageInformation();
    if (im_base->GetNumberOfDimensions() != SupportedDimensions)
    {
      std::cerr << "Unsupported Image Dimension. Only 2D images are currently supported.\n";
      return EXIT_FAILURE;
    }

    typedef float PixelType; // default pixel type is float, all voxel data is static-casted
    typedef itk::Image<PixelType, SupportedDimensions> ImageType; // define image type
    ImageType::Pointer inputImage = ImageType::New(); // initialize new image
    SafeReadImage<ImageType>(inputImage, im_base->GetFileName()); // read image along with exceptions
    
    const unsigned int rows = inputImage->GetBufferedRegion().GetSize()[0];
    const unsigned int cols = inputImage->GetBufferedRegion().GetSize()[1];

    typedef vnl_matrix< PixelType > MatrixType; // define the default matrix type for VNL
    vnl_matrix_ref< PixelType > inputMatrix(rows, cols,
      inputImage->GetBufferPointer()                // pointer to image data
      );    

    MatrixType outputMatrix = vnl_matrix_inverse<PixelType>(inputMatrix); // calculate inverse of image matrix

    typedef itk::ImportImageFilter<PixelType, SupportedDimensions> ImportFilterType;
    ImportFilterType::Pointer filter = ImportFilterType::New();
    // keep image properties same
    filter->SetOrigin(inputImage->GetOrigin());
    filter->SetDirection(inputImage->GetDirection());
    filter->SetSpacing(inputImage->GetSpacing());
    filter->SetRegion(inputImage->GetBufferedRegion());
    bool letFilterManageMemory = true;
    filter->SetImportPointer(outputMatrix.data_block(), // set start of memory pointer
      rows * cols, // total number of pixels for the image
      letFilterManageMemory // filter takes care of memory management
      );

    // write the result out
    typedef itk::ImageFileWriter<ImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetInput(filter->GetOutput());
    writer->SetFileName(outputFName);
    writer->Write();
  }
  catch (itk::ExceptionObject &error)
  {
    std::cerr << "Exception caught: " << error << "\n";
    return EXIT_FAILURE;
  }
  
  std::cout << "Finished successfully.\n";
  return EXIT_SUCCESS;
}