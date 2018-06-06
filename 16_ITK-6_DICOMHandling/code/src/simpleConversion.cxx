#include <algorithm>

#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaDataDictionary.h"

#include "cbicaUtilities.h"

typedef signed short PixelType;
const unsigned int Dimensions = 3;
typedef std::vector< std::string > StringVector;
typedef itk::Image< PixelType, Dimensions > ImageType;
typedef itk::ImageSeriesReader< ImageType > ReaderType;
typedef itk::GDCMImageIO ImageIOType;
typedef itk::GDCMSeriesFileNames NamesGeneratorType;
typedef itk::MetaDataDictionary DictionaryType;

void echoUsage(const std::string &exeName)
{
  std::cout << exeName << " <inputDicomDirectory> <outputFileName> [seriesName]\n" <<
    "NOTE - Only dicom are supported in this example. Series name should be in format '$|$'.\n";
}

int main(int argc, char **argv)
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << std::endl;
    echoUsage(argv[0]);
    exit(EXIT_FAILURE);
  }

  std::string seriesIdentifier;
  const std::string inputDirectory = argv[1], outputFileName = argv[2];
  if (argc == 4)
  {
    seriesIdentifier = argv[3];
  }

  ReaderType::Pointer reader = ReaderType::New();
  ImageIOType::Pointer dicomIO = ImageIOType::New();
  reader->SetImageIO(dicomIO);

  // set up the file names for reading
  NamesGeneratorType::Pointer namesGenerator = NamesGeneratorType::New();
  namesGenerator->SetUseSeriesDetails(true);
  namesGenerator->AddSeriesRestriction(seriesIdentifier);
  namesGenerator->SetInputDirectory(inputDirectory);
  const ReaderType::FileNamesContainer & allFilenames = namesGenerator->GetInputFileNames();

  // read the first image to ascertain the modality type
  typedef itk::Image< PixelType, 2 > SliceType;
  typedef itk::ImageFileReader< SliceType > SliceReaderType;
  SliceReaderType::Pointer sReader = SliceReaderType::New();
  sReader->SetImageIO(dicomIO);
  sReader->SetFileName(allFilenames[0]);

  try
  {
    sReader->Update();
  }
  catch (itk::ExceptionObject &excp)
  {
    std::cerr << "Exception thrown while reading the first file in the series: " << excp.what() << std::endl;
    exit(EXIT_FAILURE);
  }

  // check the tag 0008|0060 for modality information; for more information check http://dicom.nema.org/medical/dicom/current/output/chtml/part03/sect_C.17.6.html
  DictionaryType sliceDict = sReader->GetMetaDataDictionary();
  std::string modalityType;
  itk::ExposeMetaData< std::string >(sliceDict, "0008|0060", modalityType);
  if ((modalityType != "MR") || (modalityType != "MRI")) // for a full list of modalities, check http://www.dicomlibrary.com/dicom/modality/
  {    
    std::cerr << "Only MRI data is supported for this conversion.\n";
    exit(EXIT_FAILURE);
  }

  itk::ExposeMetaData< std::string >(sliceDict, "0008|103E", modalityType);
  if ((modalityType != "DWI") || (modalityType != "DTI")) // for a full list of modalities, check http://www.dicomlibrary.com/dicom/modality/
  {
    // call dwiConversion class here
  }

  const StringVector &seriesUID = namesGenerator->GetSeriesUIDs();
  //StringVector::const_iterator seriesItr = seriesUID.begin();
  //for (StringVector::const_iterator it = seriesUID.begin(); it != seriesUID.end(); ++it)
  //{
  //  std::cout << it->c_str() << "\n";
  //}
  if (seriesIdentifier == "")
  {
    seriesIdentifier = seriesUID.begin()->c_str();
  }

  StringVector fileNames = namesGenerator->GetFileNames(seriesIdentifier);
  reader->SetFileNames(fileNames);
  try
  {
    reader->Update();
  }
  catch (itk::ExceptionObject &ex)
  {
    std::cout << "Exception caught trying to read DICOM images: " << ex << std::endl;
    exit(EXIT_FAILURE);
  }

  ImageType::Pointer inputImage = ImageType::New();
  inputImage->Graft(reader->GetOutput());

  // do tralalala with inputImage

  return EXIT_SUCCESS;
}