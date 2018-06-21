/**
\brief ITK-ML-2: Advanced machine learning example
*/
#include <vector>
#include <string>
#include <algorithm>
#include <tuple>

//! ITK headers
#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkImageIOBase.h"
#include "itkImageIOFactory.h"
#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIterator.h"
#include "itkResampleImageFilter.h"

#include "itkMaskImageFilter.h"
#include "itkImageToListSampleFilter.h"

#include "opencv2/core.hpp"
#include "opencv2/ml.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"

#include "cbicaUtilities.h"
#include "cbicaCmdParser.h"
#include "cbicaITKSafeImageIO.h"

// main entry of program
int main(int argc, char *argv[])
{
  cbica::CmdParser parser = cbica::CmdParser(argc, argv);
  parser.addRequiredParameter("c", "csvFile", cbica::Parameter::FILE, ".csv file", "CSV File containing input image paths");
  parser.addRequiredParameter("i", "images", cbica::Parameter::STRING, "Delimiter needs to be ','", "Columns of the CSV file which are to be", "considered as input images");
  parser.addRequiredParameter("s", "saveFile", cbica::Parameter::FILE, ".xml", "File to save the trained SVM");
  parser.exampleUsage("ITK_Tutorial_ML.exe --csvFile C:/Tutorials/13_ITK-5_ML/code/data/machine_learning/list.csv --images 'T1,T2,FL,PD,MANUAL,FOREGROUND' --saveFile C:/Tutorials/13_ITK-5_ML/code/data/machine_learning/trained.xml");

  if ((argc <= 1) || (argc > 9))
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

  std::string csvFile, inputImageCols, inputLabelCols, saveFile;

  parser.getParameterValue("c", csvFile);
  parser.getParameterValue("i", inputImageCols);
  parser.getParameterValue("s", saveFile);

  csvFile = cbica::replaceString(csvFile, "\\", "/");
  saveFile = cbica::replaceString(saveFile, "\\", "/");

  //if (csvFile.empty() || inputImageCols.empty() || inputLabelCols.empty() || saveFile.empty())
  //{
  //  std::cerr << "Required parameter(s) cannot be empty. Check help or usage for details.\n";
  //  return EXIT_FAILURE;
  //}

  try // to catch exceptions
  {
    std::vector< CSVDict > sortedSubjectsAndFiles = cbica::parseCSVFile(csvFile, inputImageCols, "");

    std::vector< std::string > inputImageCols_vector = cbica::stringSplit(inputImageCols, ",");

    size_t maskLocation, lesionLocation;
    for (size_t i = 0; i < inputImageCols_vector.size(); i++)
    {
      std::string tempString = inputImageCols_vector[i];
      std::transform(tempString.begin(), tempString.end(), tempString.begin(), ::tolower);
      if (tempString == "manual")
      {
        maskLocation = i;
      }
      else if (tempString == "foreground")
      {
        lesionLocation = i;
      }
    }

    // helpful typedefs to make code reading easier
    typedef float PixelType; 
    typedef itk::Image< PixelType, 3 > FloatImageType;
    typedef FloatImageType::Pointer ImagePointerType;
    typedef itk::ImageRegionConstIterator<FloatImageType> IteratorType;
      
    // initialize the training and labels cv::Mat structures as null matrices
    cv::Mat training_data, labels;

    for (size_t i = 0; i < sortedSubjectsAndFiles.size(); i++)
    {
      // pre-initialize input image pointers and their respective iterators for quick reading
      std::vector< ImagePointerType > inputImagePointers;
      std::vector< IteratorType > inputImageIterators;
      inputImagePointers.resize(sortedSubjectsAndFiles[i].inputImages.size() - 2);
      inputImageIterators.resize(sortedSubjectsAndFiles[i].inputImages.size() - 2);

      // loop through all the input images
      for (size_t j = 0; j < sortedSubjectsAndFiles[i].inputImages.size(); j++)
      {
        // if input image is not the mask or the lesion, go in to read the image and initialize its respective iterator
        if ((j != maskLocation) && (j != lesionLocation))
        {
          inputImagePointers[j] = FloatImageType::New();
          // single image read per iteration
          inputImagePointers[j] = cbica::ReadImage< FloatImageType >(sortedSubjectsAndFiles[i].inputImages[j]);
          inputImageIterators[j] = IteratorType(inputImagePointers[j], inputImagePointers[j]->GetBufferedRegion());
        }
      }

      // read the mask and foreground images and initialize their iterators
      ImagePointerType maskImage = FloatImageType::New(), lesionImage = FloatImageType::New();
      maskImage = cbica::ReadImage<FloatImageType>(sortedSubjectsAndFiles[i].inputImages[maskLocation]);
      lesionImage = cbica::ReadImage<FloatImageType>(sortedSubjectsAndFiles[i].inputImages[lesionLocation]);
      IteratorType maskIt(maskImage, maskImage->GetBufferedRegion()),
        lesionIt(lesionImage, lesionImage->GetBufferedRegion());

      // loop through the entire mask per subject
      for (maskIt.GoToBegin(); !maskIt.IsAtEnd(); ++maskIt)
      {
        // a temporary cv::Mat to hold voxel intensities
        cv::Mat tempMat;
        // if mask contains a non-zero value, go in and store intensities
        if (maskIt.Get() != static_cast<PixelType>(0))
        {
          // use foreground as labels
          lesionIt.SetIndex(maskIt.GetIndex());
          labels.push_back(lesionIt.Get());

          tempMat = cv::Mat();
          for (size_t j = 0; j < inputImageIterators.size(); j++)
          {
            inputImageIterators[j].SetIndex(maskIt.GetIndex());
            tempMat.push_back(inputImageIterators[j].Get()); // construct temporary cv::Mat of voxel intensities
          }
        }
        if (tempMat.cols > 0)
        {
          training_data.push_back(
            tempMat.t() // take its transpose to make it a column matrix (it defaults to row-major)
            );
        }
      }
      inputImageIterators.resize(0);
      inputImagePointers.resize(0);
    }

    ////// start teaching the machine
    auto svm = cv::ml::SVM::create(); // create a new instance of cv::ml::SVM (http://docs.opencv.org/3.0-beta/modules/ml/doc/support_vector_machines.html)

    // set up the SVM parameters (http://docs.opencv.org/3.0-beta/modules/ml/doc/support_vector_machines.html#svm-params-params)
    svm->setType(cv::ml::SVM::C_SVC); 
    svm->setKernel(cv::ml::SVM::LINEAR);
    svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, // say that the termination criteria is maximum number of iterations
      100, // set the maximum number of iterations
      1e-6) // set the accuracy at which the iterations stop
      );
    svm->setClassWeights(cv::Mat()); // there are are no weights to be assigned for the classes (both classes are distributed equally)

    // train the SVM
    svm->train(training_data, // set training data
      cv::ml::ROW_SAMPLE, // tell SVM that it is row-major
      labels // set the labels
      );

    svm->save(saveFile);

  }
  catch (itk::ExceptionObject &error)
  {
    std::cerr << "Exception caught: " << error << "\n";
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}