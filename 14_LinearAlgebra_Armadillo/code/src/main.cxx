#include <type_traits>
#include <algorithm>

//! ITK headers
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImportImageFilter.h"

//! Armadillo headers
#include "armadillo"

#include "cbicaUtilities.h"
#include "cbicaCmdParser.h"
#include "cbicaITKSafeImageIO.h"

const int SupportedDimensions = 2;
typedef double PrecisionType;

// main entry of program
int main(int argc, char *argv[])
{
  cbica::CmdParser parser = cbica::CmdParser(argc, argv);
  parser.addRequiredParameter("i", "inputFile", cbica::Parameter::FILE, ".nii.gz, 3D", "Input File");
  parser.addRequiredParameter("o", "outputFile", cbica::Parameter::FILE, ".nii.gz, 3D", "Output File");
  parser.exampleUsage("Armadillo_Tutorial.exe --inputFile C:/Tutorials/14_LinearAlgebra_Armadillo/data/testImage.nii.gz --output C:/Tutorials/14_LinearAlgebra_Armadillo/data/testOutput.nii.gz");

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

  if (parser.isPresent("v"))
  {
    parser.echoVersion();
    return EXIT_SUCCESS;
  }

  if (parser.isPresent("h"))
  {
    parser.echoHelp();
    return EXIT_SUCCESS;
  }

  std::string inputFile, outputFile;
  parser.getParameterValue("i", inputFile);
  parser.getParameterValue("o", outputFile);

  if (inputFile.empty() || outputFile.empty() )
  {
    std::cerr << "Required parameter(s) not provided. Check usage or help.\n";
    return EXIT_FAILURE;
  }

  try // to catch exceptions
  {
    typedef itk::Image< PrecisionType, SupportedDimensions > ImageType;
    typedef ImageType::Pointer ImagePointerType;
    
    ImagePointerType inputImage = cbica::ReadImage< ImageType >(inputFile);
    
    // get size of the input image
    ImageType::SizeType imageSize = inputImage->GetBufferedRegion().GetSize();

    // 2D matrix
    arma::mat inputImageMat(inputImage->GetBufferPointer(), imageSize[0], imageSize[1]);

    //// 3D 'matrix' - for a 3D image
    //arma::cube testCube(reinterpret_cast<PrecisionType *>(inputImage->GetBufferPointer()), imageSize[0], imageSize[1], imageSize[2]);

    // initialize an identity matrix
    arma::mat addMat = arma::eye(imageSize[0], imageSize[1]);

    // see http://arma.sourceforge.net/docs.html for a list of functions and matrix types

    arma::mat resultMat = inputImageMat + addMat;
    
    typedef itk::ImportImageFilter< PrecisionType, SupportedDimensions > ImportFilterType;
    ImportFilterType::Pointer importFilter = ImportFilterType::New();
    importFilter->SetOrigin(inputImage->GetOrigin());
    importFilter->SetSpacing(inputImage->GetSpacing());
    importFilter->SetDirection(inputImage->GetDirection());
    importFilter->SetMetaDataDictionary(inputImage->GetMetaDataDictionary());
    importFilter->SetRegion(inputImage->GetLargestPossibleRegion());
    importFilter->SetImportPointer(resultMat.memptr(), imageSize[0] * imageSize[1], 
      false // importFilter should not have ownership of the buffer since it won't be able to deallocate resultMat automatically
      );
    importFilter->SetRegion(inputImage->GetLargestPossibleRegion());
    importFilter->Update();
    
    cbica::WriteImage<ImageType, ImageType>(importFilter->GetOutput(), outputFile);
  }
  catch (itk::ExceptionObject &error)
  {
    std::cerr << "Exception caught: " << error << "\n";
    return EXIT_FAILURE;
  }
  
  std::cout << "Finished successfully.\n";
  return EXIT_SUCCESS;
}