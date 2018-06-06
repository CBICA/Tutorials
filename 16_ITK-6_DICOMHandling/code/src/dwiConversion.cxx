#include <type_traits>
#include <algorithm>

//! ITK headers
#include "itkImage.h"
#include "itkVectorImage.h"
#include "itkImageFileReader.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"

#include "gdcmDictEntry.h"
#include "gdcmDict.h"           // access to dictionary
#include "gdcmFile.h"           // access to dictionary
#include "gdcmDictEntry.h"      // access to dictionary
#include "gdcmGlobal.h"         // access to dictionary
#include "gdcmElement.h"
#include "gdcmPrivateTag.h"
#include "gdcmStringFilter.h"

#include "cbicaUtilities.h"

// relevant GE private tags
const gdcm::DictEntry GEDictBValue("0x0043", "0x1039", gdcm::VR::IS, gdcm::VM::VM1, "B Value of diffusion weighting");
const gdcm::DictEntry GEDictXGradient("0x0019", "0x10bb", gdcm::VR::DS, gdcm::VM::VM1, "X component of gradient direction");
const gdcm::DictEntry GEDictYGradient("0x0019", "0x10bc", gdcm::VR::DS, gdcm::VM::VM1, "Y component of gradient direction");
const gdcm::DictEntry GEDictZGradient("0x0019", "0x10bd", gdcm::VR::DS, gdcm::VM::VM1, "Z component of gradient direction");

// relevant Siemens private tags
const gdcm::DictEntry SiemensMosiacParameters("0x0051", "0x100b", gdcm::VR::IS, gdcm::VM::VM1, "Mosiac Matrix Size");
const gdcm::DictEntry SiemensDictNMosiac("0x0019", "0x100a", gdcm::VR::US, gdcm::VM::VM1, "Number of Images In Mosaic");
const gdcm::DictEntry SiemensDictBValue("0x0019", "0x100c", gdcm::VR::IS, gdcm::VM::VM1, "B Value of diffusion weighting");
const gdcm::DictEntry SiemensDictDiffusionDirection("0x0019", "0x100e", gdcm::VR::FD, gdcm::VM::VM3, "Diffusion Gradient Direction");
const gdcm::DictEntry SiemensDictDiffusionMatrix("0x0019", "0x1027", gdcm::VR::FD, gdcm::VM::VM6, "Diffusion Matrix");

// private tags from other vendors ??

// private tags from other vendors ??

const int Dimensions = 3;
typedef short PixelValueType;
typedef itk::Image< PixelValueType, Dimensions > VolumeType;
typedef itk::ImageSeriesReader< VolumeType > ReaderType;
typedef itk::GDCMImageIO ImageIOType;
typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
typedef itk::MetaDataDictionary DictionaryType;
typedef itk::VectorImage< PixelValueType, Dimensions > VectorImageType;

void echoUsage(const std::string &exeName)
{
  std::cout << exeName << " <inputDicomDirectory> <outputFileName>\n" <<
    "NOTE - Only dicom are supported in this example.\n";
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
      exit(EXIT_FAILURE);
    }

    const std::string inputDirName = argv[2], outputFName = argv[3];
    std::vector< std::string > allFilesInDirectory = cbica::filesInDirectory(inputDirName);

    for (size_t i = 0; i < allFilesInDirectory.size(); i++)
    {
      std::string path, base, ext;
      cbica::splitFileName(allFilesInDirectory[i], path, base, ext);

      // check for incompatible files
      if ((ext == ".nii") || (ext == ".nii.gz") || (ext == ".img.gz") || (ext == ".img") || (ext == ".nrrd") || (ext == ".nrrd.gz") || (ext == ".mha"))
      {
        std::cerr << "The file extension detected is '" << ext << "', which is not supported.\n";
        exit(EXIT_FAILURE);
      }
    }

    // read a single slice and figure out vendor
    ImageIOType::Pointer gdcmIO = ImageIOType::New();
    gdcmIO->LoadPrivateTagsOn();

    InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
    inputNames->SetInputDirectory(inputDirName.c_str());

    const ReaderType::FileNamesContainer & filenames = inputNames->GetInputFileNames();

    typedef itk::Image< PixelValueType, 2 > SliceType;
    typedef itk::ImageFileReader< SliceType > SliceReaderType;

    SliceReaderType::Pointer sReader = SliceReaderType::New();
    sReader->SetImageIO(gdcmIO);
    sReader->SetFileName(filenames[0]);

    try
    {
      sReader->Update();
    }
    catch (itk::ExceptionObject &excp)
    {
      std::cerr << "Exception thrown while reading the first file in the series: " << excp.what() << std::endl;
      exit(EXIT_FAILURE);
    }

    // check the tag 0008|0070 for vendor information
    DictionaryType sliceDict = sReader->GetMetaDataDictionary();
    std::string vendor;
     
    // ensure that only MRI data is being read
    itk::ExposeMetaData< std::string >(sliceDict, "0008|0060", vendor);
    if ((vendor != "MR") || (vendor != "MRI")) // for a full list of modalities, check http://www.dicomlibrary.com/dicom/modality/
    {
      std::cerr << "Only MRI data is supported for this conversion.\n";
      exit(EXIT_FAILURE);
    }

    itk::ExposeMetaData< std::string >(sliceDict, "0008|0070", vendor);
    //std::cout << vendor << std::endl;

    //  0b) Add private dictionary -- private dictionaries are not a part of DICOM 3 standard and are loaded by default in GDCM so unsure if the code specified below should be used at all
    if (vendor.find("GE") != std::string::npos)
    {
      // for GE data      
      const gdcm::PrivateDict &testDict = gdcm::Global::GetInstance().GetDicts().GetPrivateDict();
      std::remove_cv<const gdcm::PrivateDict>::type testDict2;
      testDict2.AddDictEntry(gdcm::PrivateTag(std::atoi(GEDictBValue.GetName()), std::atoi(GEDictBValue.GetKeyword())), GEDictBValue);
      testDict2.AddDictEntry(gdcm::PrivateTag(std::atoi(GEDictXGradient.GetName()), std::atoi(GEDictXGradient.GetKeyword())), GEDictXGradient);
      testDict2.AddDictEntry(gdcm::PrivateTag(std::atoi(GEDictYGradient.GetName()), std::atoi(GEDictYGradient.GetKeyword())), GEDictYGradient);
      testDict2.AddDictEntry(gdcm::PrivateTag(std::atoi(GEDictZGradient.GetName()), std::atoi(GEDictZGradient.GetKeyword())), GEDictZGradient);
      //  gdcm::Global::GetDicts()->GetDefaultPubDict()->AddEntry(GEDictBValue);
      //  gdcm::Global::GetDicts()->GetDefaultPubDict()->AddEntry(GEDictXGradient);
      //  gdcm::Global::GetDicts()->GetDefaultPubDict()->AddEntry(GEDictYGradient);
      //  gdcm::Global::GetDicts()->GetDefaultPubDict()->AddEntry(GEDictZGradient);
    }
    else if (vendor.find("SIEMENS") != std::string::npos)
    {
      // for Siemens data
      const gdcm::PrivateDict &testDict = gdcm::Global::GetInstance().GetDicts().GetPrivateDict();
      std::remove_cv<const gdcm::PrivateDict>::type testDict2;
      testDict2.AddDictEntry(gdcm::PrivateTag(std::atoi(SiemensMosiacParameters.GetName()), std::atoi(SiemensMosiacParameters.GetKeyword())), SiemensMosiacParameters);
      testDict2.AddDictEntry(gdcm::PrivateTag(std::atoi(SiemensDictNMosiac.GetName()), std::atoi(SiemensDictNMosiac.GetKeyword())), SiemensDictNMosiac);
      testDict2.AddDictEntry(gdcm::PrivateTag(std::atoi(SiemensDictBValue.GetName()), std::atoi(SiemensDictBValue.GetKeyword())), SiemensDictBValue);
      testDict2.AddDictEntry(gdcm::PrivateTag(std::atoi(SiemensDictDiffusionDirection.GetName()), std::atoi(SiemensDictDiffusionDirection.GetKeyword())), SiemensDictDiffusionDirection);
      testDict2.AddDictEntry(gdcm::PrivateTag(std::atoi(SiemensDictDiffusionMatrix.GetName()), std::atoi(SiemensDictDiffusionMatrix.GetKeyword())), SiemensDictDiffusionMatrix);
      //  gdcm::Global::GetDicts()->GetDefaultPubDict()->AddEntry(SiemensMosiacParameters);
      //  gdcm::Global::GetDicts()->GetDefaultPubDict()->AddEntry(SiemensDictNMosiac);
      //  gdcm::Global::GetDicts()->GetDefaultPubDict()->AddEntry(SiemensDictBValue);
      //  gdcm::Global::GetDicts()->GetDefaultPubDict()->AddEntry(SiemensDictDiffusionDirection);
      //  gdcm::Global::GetDicts()->GetDefaultPubDict()->AddEntry(SiemensDictDiffusionMatrix);
    }
    else if (vendor.find("PHILIPS") != std::string::npos)
    {
      // for philips data
    }
    else
    {
      std::cerr << "Unrecognized vendor.\n" << std::endl;
    }

    // 1) Read the input series as an array of slices
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetImageIO(gdcmIO);
    reader->SetFileNames(filenames);
    try
    {
      reader->Update();
    }
    catch (itk::ExceptionObject &excp)
    {
      std::cerr << "Exception thrown while reading the series" << excp.what() << std::endl;
      exit(EXIT_FAILURE);
    }

    // 2) Analyze the DICOM header to determine the number of gradient directions, gradient vectors, and form volume based on this info

    ReaderType::DictionaryArrayRawPointer inputDict = reader->GetMetaDataDictionaryArray();

    /// for debug. to check what tags have been exposed
    itk::MetaDataDictionary & debugDict = gdcmIO->GetMetaDataDictionary();
    std::vector<std::string> allKeys = debugDict.GetKeys();
    for (int k = 0; k < allKeys.size(); k++)
    {
      //std::cout << allKeys[k] << std::endl;
    }

    // load in all public tags
    int nSlice = inputDict->size();
    std::string tag;

    tag.clear();
    itk::ExposeMetaData<std::string>(*(*inputDict)[0], "0028|0010", tag);
    int nRows = atoi(tag.c_str());

    tag.clear();
    itk::ExposeMetaData<std::string>(*(*inputDict)[0], "0028|0011", tag);
    int nCols = atoi(tag.c_str());

    tag.clear();
    itk::ExposeMetaData<std::string>(*(*inputDict)[0], "0028|0030", tag);
    float xRes;
    float yRes;
    sscanf(tag.c_str(), "%f\\%f", &xRes, &yRes);

    tag.clear();
    itk::ExposeMetaData<std::string>(*(*inputDict)[0], "0020|0032", tag);
    float xOrigin;
    float yOrigin;
    float zOrigin;
    sscanf(tag.c_str(), "%f\\%f\\%f", &xOrigin, &yOrigin, &zOrigin);

    tag.clear();
    itk::ExposeMetaData<std::string>(*(*inputDict)[0], "0018|0050", tag);
    float sliceThickness = atof(tag.c_str());

    tag.clear();
    itk::ExposeMetaData<std::string>(*(*inputDict)[0], "0018|0088", tag);
    float sliceSpacing = atof(tag.c_str());

    tag.clear();
    itk::ExposeMetaData<std::string>(*(*inputDict)[0], "0020|1041", tag);
    float maxSliceLocation = atof(tag.c_str());
    float minSliceLocation = maxSliceLocation;

    // figure out the largest and smallest slice location. This does not
    // work for Siemens data since it is stored in mosaic format
    for (int k = 0; k < nSlice; k++)
    {
      tag.clear();
      itk::ExposeMetaData<std::string>(*(*inputDict)[k], "0020|1041", tag);
      float sliceLocation = atof(tag.c_str());

      if (sliceLocation > maxSliceLocation)
      {
        maxSliceLocation = sliceLocation;
      }

      if (sliceLocation < minSliceLocation)
      {
        minSliceLocation = sliceLocation;
      }
    }

    // check ImageOrientationPatient and figure out Slice direction in
    // L-P-I (right-handed) system.
    // In Dicom, the coordinate frame is L-P by default. Look at
    // http://medical.nema.org/dicom/2007/07_03pu.pdf ,  page 301
    tag.clear();
    itk::ExposeMetaData<std::string>(*(*inputDict)[0], "0020|0037", tag);
    float xRow, yRow, zRow, xCol, yCol, zCol, xSlice, ySlice, zSlice;
    sscanf(tag.c_str(), "%f\\%f\\%f\\%f\\%f\\%f", &xRow, &yRow, &zRow, &xCol, &yCol, &zCol);
    // Cross product, this gives I-axis direction
    xSlice = (yRow*zCol - zRow*yCol);
    ySlice = (zRow*xCol - xRow*zCol);
    zSlice = (xRow*yCol - yRow*xCol);

    // In Dicom, the measurement frame is L-P by default. Look at
    // http://medical.nema.org/dicom/2007/07_03pu.pdf ,  page 301, in
    // order to make this compatible with Slicer's RAS frame, we
    // multiply the direction cosines by the negatives of the resolution
    // (resolution is required by nrrd format). Direction cosine is not
    // affacted since the resulting frame is still a right-handed frame.
    xRow = -xRow;
    yRow = -yRow;
    zRow = -zRow;

    xCol = -xCol;
    yCol = -yCol;
    zCol = -zCol;

    // figure out slice order
    bool SliceOrderIS = true;
    if (vendor.find("GE") != std::string::npos)
    {
      float x0, y0, z0;
      float x1, y1, z1;
      tag.clear();
      itk::ExposeMetaData<std::string>(*(*inputDict)[0], "0020|0032", tag);
      sscanf(tag.c_str(), "%f\\%f\\%f", &x0, &y0, &z0);
      tag.clear();

      // assume volume interleving, i.e. the second dicom file stores
      // the second slice in the same volume as the first dicom file
      itk::ExposeMetaData<std::string>(*(*inputDict)[1], "0020|0032", tag);
      sscanf(tag.c_str(), "%f\\%f\\%f", &x1, &y1, &z1);
      x1 -= x0; y1 -= y0; z1 -= z0;
      x0 = x1*xSlice + y1*ySlice + z1*zSlice;
      if (x0 < 0)
      {
        SliceOrderIS = false;
      }
    }
    else if (vendor.find("SIEMENS") != std::string::npos)
    {
      // for siemens mosaic image, we have not figured out the slice
      // order yet, from the example provided by Vince, the slice order
      // within mosaic is SI.
      SliceOrderIS = false;
    }
    else
    {
    }

    if (!SliceOrderIS)
    {
      xSlice = -xSlice;
      ySlice = -ySlice;
      zSlice = -zSlice;
    }

    int nSliceInVolume;
    int nVolume;

    float bValue;
    int nBaseline = 0;
    int nMeasurement;
    std::vector< int > idVolume;
    std::vector< vnl_vector_fixed<double, Dimensions> > DiffusionVectors;
    ////////////////////////////////////////////////////////////
    // vendor dependent tags.
    // read in gradient vectors and determin nBaseline and nMeasurement

    if (vendor.find("GE") != std::string::npos)
    {
      nSliceInVolume = static_cast<int> ((maxSliceLocation - minSliceLocation) / fabs(zSlice*sliceSpacing) + 1.5);
      // .5 is for rounding up, 1 is for adding one slice at one end.
      nVolume = nSlice / nSliceInVolume;

      for (int k = 0; k < nSlice; k += nSliceInVolume)
      {
        tag.clear();
        itk::ExposeMetaData<std::string>(*(*inputDict)[k], "0043|1039", tag);
        float b = atof(tag.c_str());
        if (b == 0)
        {
          nBaseline++;
        }
        else
        {
          bValue = b;
        }
      }

      nMeasurement = nVolume - nBaseline;

      // extract gradient vectors and form DWImages
      vnl_vector_fixed<double, Dimensions> vect3d;
      idVolume.resize(nMeasurement);
      int count = 0;

      for (int k = 0; k < nSlice; k += nSliceInVolume)
      {
        tag.clear();
        itk::ExposeMetaData<std::string>(*(*inputDict)[k], "0043|1039", tag);
        float b = atof(tag.c_str());
        if (b == 0)
        {
          continue;
        }

        idVolume[count] = k / nSliceInVolume;
        count++;

        tag.clear();
        itk::ExposeMetaData<std::string>(*(*inputDict)[k], "0019|10bb", tag);
        vect3d[0] = atof(tag.c_str());

        tag.clear();
        itk::ExposeMetaData<std::string>(*(*inputDict)[k], "0019|10bc", tag);
        vect3d[1] = atof(tag.c_str());

        tag.clear();
        itk::ExposeMetaData<std::string>(*(*inputDict)[k], "0019|10bd", tag);
        vect3d[2] = atof(tag.c_str());

        DiffusionVectors.push_back(vect3d);
      }
    }
    else if (vendor.find("SIEMENS") != std::string::npos)
    {
      // each slice is a volume in mosiac form
      nVolume = nSlice;
      nSliceInVolume = 1;
      for (int k = 0; k < nSlice; k += nSliceInVolume)
      {
        tag.clear();
        itk::ExposeMetaData<std::string>(*(*inputDict)[k], "0019|100c", tag);
        float b = atof(tag.c_str());
        if (b == 0)
        {
          nBaseline++;
        }
        else
        {
          bValue = b;
        }
      }

      nMeasurement = nVolume - nBaseline;

      // extract gradient vectors and form DWImages
      vnl_vector_fixed<double, Dimensions> vect3d;
      idVolume.resize(nMeasurement);
      int count = 0;

      for (int k = 0; k < nSlice; k += nSliceInVolume)
      {
        tag.clear();
        itk::ExposeMetaData<std::string>(*(*inputDict)[k], "0019|100c", tag);
        float b = atof(tag.c_str());
        if (b == 0)
        {
          continue;
        }

        idVolume[count] = k / nSliceInVolume;
        count++;

        tag.clear();
        itk::ExposeMetaData<std::string>(*(*inputDict)[k], "0019|100e", tag);
        memcpy(&vect3d[0], tag.c_str() + 0, 8);
        memcpy(&vect3d[1], tag.c_str() + 8, 8);
        memcpy(&vect3d[2], tag.c_str() + 16, 8);

        DiffusionVectors.push_back(vect3d);
      }

    }
    // transform gradient directions into RAS frame 

    for (int k = 0; k < nMeasurement; k++)
    {
      vnl_vector_fixed<double, Dimensions> vecTemp = DiffusionVectors[k];
      //                                                   Dicom   Slicer 
      DiffusionVectors[k][0] = -DiffusionVectors[k][0];    // L -> R
      DiffusionVectors[k][1] = -DiffusionVectors[k][1];    // P -> A
      if (vendor.find("GE") != std::string::npos)
      {
        DiffusionVectors[k][2] = -DiffusionVectors[k][2];  // I -> S
      }

      DiffusionVectors[k].normalize();
      std::cout << idVolume[k] << "\t" << DiffusionVectors[k] << std::endl;
    }


    // put pixels in the right places in the raw volume
    VolumeType::Pointer rawVol;

    if (vendor.find("GE") != std::string::npos)
    {
      rawVol = reader->GetOutput();

    }
    else if (vendor.find("SIEMENS") != std::string::npos)
    {
      // de-mosaic
      tag.clear();
      itk::ExposeMetaData<std::string>(*(*inputDict)[0], "0051|100b", tag);
      int mMosaic;   // number of raws in each mosaic block;
      int nMosaic;   // number of columns in each mosaic block
      sscanf(tag.c_str(), "%dp*%ds", &mMosaic, &nMosaic);

      mMosaic = nRows / mMosaic;    // number of block rows in one dicom slice
      nMosaic = nCols / nMosaic;    // number of block columns in one
      // dicom slice
      nRows /= mMosaic;
      nCols /= nMosaic;

      nSliceInVolume = mMosaic*nMosaic;

      // center the volume since the image position patient given in the
      // dicom header was useless
      xOrigin = -(nRows*xRow + nCols*xCol + nSliceInVolume*xSlice) / 2.0;
      yOrigin = -(nRows*yRow + nCols*yCol + nSliceInVolume*ySlice) / 2.0;
      zOrigin = -(nRows*zRow + nCols*zCol + nSliceInVolume*zSlice) / 2.0;

      VolumeType::Pointer img = reader->GetOutput();

      VolumeType::RegionType region = img->GetLargestPossibleRegion();
      VolumeType::SizeType size = region.GetSize();

      VolumeType::SizeType dmSize = size;
      dmSize[0] /= mMosaic;
      dmSize[1] /= nMosaic;
      dmSize[2] *= (mMosaic*nMosaic);

      region.SetSize(dmSize);
      VolumeType::Pointer dmImage = VolumeType::New();
      dmImage->CopyInformation(img);
      dmImage->SetRegions(region);
      dmImage->Allocate();

      VolumeType::RegionType dmRegion = dmImage->GetLargestPossibleRegion();
      dmRegion.SetSize(2, 1);
      region.SetSize(0, dmSize[0]);
      region.SetSize(1, dmSize[1]);
      region.SetSize(2, 1);

      int rawMosaic = 0;
      int colMosaic = 0;
      int slcMosaic = 0;

      for (int k = 0; k < dmSize[2]; k++)
      {
        dmRegion.SetIndex(2, k);
        itk::ImageRegionIteratorWithIndex<VolumeType> dmIt(dmImage, dmRegion);

        // figure out the mosaic region for this slice
        int sliceIndex = k;

        int nBlockPerSlice = mMosaic*nMosaic;
        int slcMosaic = sliceIndex / (nBlockPerSlice);
        sliceIndex -= slcMosaic*nBlockPerSlice;
        int colMosaic = sliceIndex / mMosaic;
        int rawMosaic = sliceIndex - mMosaic*colMosaic;
        region.SetIndex(0, rawMosaic*dmSize[0]);
        region.SetIndex(1, colMosaic*dmSize[1]);
        region.SetIndex(2, slcMosaic);

        itk::ImageRegionConstIteratorWithIndex<VolumeType> imIt(img, region);

        for (dmIt.GoToBegin(), imIt.GoToBegin(); !dmIt.IsAtEnd(); ++dmIt, ++imIt)
        {
          dmIt.Set(imIt.Get());
        }
      }
      rawVol = dmImage;
    }
    else
    {
    }

    // convert raw volume into nrrd image (i.e., VectorImage) and set
    // propriate Meta Data
    VectorImageType::Pointer outputImage = VectorImageType::New();

    // set proper regions
    VectorImageType::RegionType outputImageRegion;
    //NRRDImageType::IndexType nrrdIndex;
    outputImageRegion.SetIndex(0, 0);
    outputImageRegion.SetIndex(1, 0);
    outputImageRegion.SetIndex(2, 0);
    outputImageRegion.SetSize(0, nRows);
    outputImageRegion.SetSize(1, nCols);
    outputImageRegion.SetSize(2, nSliceInVolume);
    outputImage->SetRegions(outputImageRegion);

    // set vector length
    outputImage->SetVectorLength(nVolume);
    outputImage->Allocate();

    // set origin
    VectorImageType::PointType outputImageOrigin;
    outputImageOrigin[0] = xOrigin;
    outputImageOrigin[1] = yOrigin;
    outputImageOrigin[2] = zOrigin;
    outputImage->SetOrigin(outputImageOrigin);

    // set spacing
    VectorImageType::SpacingType outputImageSpacing;
    outputImageSpacing[0] = xRes;
    outputImageSpacing[1] = yRes;
    outputImageSpacing[2] = sliceSpacing;
    outputImage->SetSpacing(outputImageSpacing);

    // SetDirections
    VectorImageType::DirectionType outputImageDirection;
    outputImageDirection[0][0] = xRow;
    outputImageDirection[0][1] = yRow;
    outputImageDirection[0][2] = zRow;
    outputImageDirection[1][0] = xCol;
    outputImageDirection[1][1] = yCol;
    outputImageDirection[1][2] = zCol;
    outputImageDirection[2][0] = xSlice;
    outputImageDirection[2][1] = ySlice;
    outputImageDirection[2][2] = zSlice;
    outputImage->SetDirection(outputImageDirection);

    // iterate to put data in rawVol into proper position in nrrdImage;
    // rawVol is already in volume interleving form
    itk::ImageRegionIteratorWithIndex<VectorImageType> outputImageIt(outputImage, outputImageRegion);
    for (outputImageIt.GoToBegin(); !outputImageIt.IsAtEnd(); ++outputImageIt)
    {
      VectorImageType::IndexType outputImageIdx = outputImageIt.GetIndex();
      VectorImageType::PixelType outputImagePixel = outputImageIt.Get();

      VolumeType::IndexType volIdx;
      volIdx[0] = outputImageIt.GetIndex()[0];
      volIdx[1] = outputImageIt.GetIndex()[1];

      for (int k = 0; k < nVolume; k++)
      {
        volIdx[2] = outputImageIt.GetIndex()[2] + k*nSliceInVolume;
        outputImageIt.Get()[k] = rawVol->GetPixel(volIdx);
      }

      outputImageIt.Set(outputImagePixel);
    }

    // construct meta dictionary
    itk::MetaDataDictionary outputImageMetaDictionary;
    std::string metaString;
    std::string metaKey;

    itk::EncapsulateMetaData<std::string>(outputImageMetaDictionary, "DWI/DTI_content", "exists(MyVectorImage.raw,0)");
    itk::EncapsulateMetaData<std::string>(outputImageMetaDictionary, "ITK_InputFilterName", "VectorImageIO");
    itk::EncapsulateMetaData<std::string>(outputImageMetaDictionary, "VectorImage_space", "right-anterior-superior");

    // the following key/value pairs are for each axis
    itk::EncapsulateMetaData<std::string>(outputImageMetaDictionary, "VectorImage_centerings[0]", "cell");
    itk::EncapsulateMetaData<std::string>(outputImageMetaDictionary, "VectorImage_centerings[1]", "cell");
    itk::EncapsulateMetaData<std::string>(outputImageMetaDictionary, "VectorImage_centerings[2]", "cell");

    itk::EncapsulateMetaData<std::string>(outputImageMetaDictionary, "VectorImage_kinds[0]", "space");
    itk::EncapsulateMetaData<std::string>(outputImageMetaDictionary, "VectorImage_kinds[1]", "space");
    itk::EncapsulateMetaData<std::string>(outputImageMetaDictionary, "VectorImage_kinds[2]", "space");
    itk::EncapsulateMetaData<std::string>(outputImageMetaDictionary, "VectorImage_kinds[3]", "list");

    // for measurement frame
    std::vector<std::vector<double> > msrFrame(Dimensions);
    for (unsigned int k = 0; k < Dimensions; k++)
    {
      msrFrame[k].resize(Dimensions);
      for (unsigned int m = 0; m < Dimensions; m++)
      {
        msrFrame[k][m] = 0;
      }
      msrFrame[k][k] = 1;
    }

    itk::EncapsulateMetaData<std::vector<std::vector<double> > >(outputImageMetaDictionary, "VectorImage_measurement frame", msrFrame);


    // the following are key-value pairs
    itk::EncapsulateMetaData<std::string>(outputImageMetaDictionary, "modality", "DWMRI");

    std::ostringstream outputImageValueStream;
    outputImageValueStream << bValue;
    itk::EncapsulateMetaData<std::string>(outputImageMetaDictionary, "DWMRI_b-value", outputImageValueStream.str());
    itk::EncapsulateMetaData<std::string>(outputImageMetaDictionary, "DWMRI_gradient_0000", "0   0   0");
    outputImageValueStream.str("");
    outputImageValueStream << nBaseline;
    itk::EncapsulateMetaData<std::string>(outputImageMetaDictionary, "DWMRI_NEX_0000", outputImageValueStream.str());

    // need to check
    for (int k = nBaseline; k < nVolume; k++)
    {
      outputImageValueStream.str("");
      outputImageValueStream << DiffusionVectors[k - nBaseline][0] << "   " << DiffusionVectors[k - nBaseline][1] << "   " << DiffusionVectors[k - nBaseline][2];

      std::ostringstream nrrdKeyStream("");
      nrrdKeyStream << "DWMRI_gradient_" << std::setw(4) << std::setfill('0') << k;
      itk::EncapsulateMetaData<std::string>(outputImageMetaDictionary, nrrdKeyStream.str(), outputImageValueStream.str());
    }


    outputImage->SetMetaDataDictionary(outputImageMetaDictionary);

    /*itk::ImageFileWriter<NRRDImageType>::Pointer nrrdWriter = itk::ImageFileWriter<NRRDImageType>::New();
    nrrdWriter->SetFileName(dwiImage.c_str());
    nrrdWriter->SetInput(nrrdImage);
    nrrdWriter->Update();*/

    return EXIT_SUCCESS;

    
  }
  catch (itk::ExceptionObject &error)
  {
    std::cerr << "Exception caught: " << error << "\n";
    return EXIT_FAILURE;
  }
  
  std::cout << "Finished successfully.\n";
  return EXIT_SUCCESS;
}