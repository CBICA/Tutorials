# Tutorials

Welcome to CBICA’s C++ learning resource.

Here, we will be showcasing our seminar series "CPP for Image Processing and Machine Learning" including presentations and code examples. 

There are image processing and machine learning libraries out there which use C++ as a base and have become industry standards (<a href="http://www.itk.org/">ITK</a> for medical imaging, <a href="http://opencv.org">OpenCV</a> for computer vision and machine learning, <a href="http://arma.sourceforge.net/">Armadillo</a> for linear algebra, <a href="http://image.diku.dk/shark/">Shark</a>, <a href="http://dlib.net">dlib</a> and <a href="http://www.shogun-toolbox.org/">Shogun</a> for machine learning and so on). 

The documentation provided with these packages, though extensive, assume a certain level of experience with C++ (familiarity with syntax, mainly). Our tutorials are intended for those people who have basic understanding of medical image processing and machine learning but who are just starting to get their toes wet with C++ (and possibly have prior experience with Python or MATLAB).

Here we will be focusing on how someone with a good theoretical background in image processing and machine learning can quickly prototype algorithms using CPP and extend them to create meaningful software packages.

<b>NOTE:</b> To download any of the tutorials, you will need an SVN client (Windows users can turn to <a href="https://tortoisesvn.net/">Tortoise SVN</a>). Please copy the link and do an <a href="http://vegastrike.sourceforge.net/wiki/HowTo:Checkout_SVN">SVN checkout</a> (this will be covered in detail in the first tutorial) with username and password set to 'anonymous'.

For example, to get the first ITK tutorial, the command to run will be:
\verbatim
svn --username anonymous --password anonymous export https://www.nitrc.org/svn/cbica_tutorials/06_ITK-1_Segmentation/ ${directory_where_you_want}
\endverbatim

The list of tutorials already given:
-# <a hred="https://www.nitrc.org/svn/cbica_tutorials/00_Versioning/">Introduction to Versioning with SVN</a>
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/01_Setup/">Setup and Hello World</a>
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/02_CMake/">Introduction to CMake</a>
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/03_OOP/">Object Oriented Programming</a>: Basic concepts and usage example (directly correlates with ITK development)
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/04_STL/">Introducing the Standard Template Library</a>
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/05_CPP11/">Introduction to the C++11 standard</a> (and how it makes programming easier)
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/06_ITK-1_Segmentation/">ITK Tutorial 1</a>: Basic example with connectivity segmentation
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/07_ITK-2_Multiplication/">ITK Tutorial 2</a>: Basic example with voxel-wise multiplcation
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/08_ITK-3_Registration/">ITK Tutorial 3</a>: Basic example with affine registration
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/09_ITK-4_LinearAlgebra/">ITK Tutorial 4</a>: Linear Algebra using built in library (VNL)
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/11_ErrorHandling/">Error & Exception Handling</a>: Giving meaningful messages to the user (and not "segmentation fault") and telling it what <b>not</b> to do
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/12_UnitTesting/">Unit Testing</a>: Backbone of a good software package
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/13_ITK-5_ML/">ITK Tutorial 5</a>: Machine learning with ITK while harnessing power of OpenCV
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/14_LinearAlgebra_Armadillo/">Advanced Linear Algebra</a> using the power of LAPACK/BLAS with Armadillo
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/16_ITK-6_DICOMHandling/">ITK Tutorial 6</a>: Working with DICOM to write a clinically relevant application
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/17_CMake-CTest-CDash/">Unit Testing Advanced</a>: Combining the power of CMake, CTest and CDash for a cohesive software build test suite
-# <a href="https://www.nitrc.org/svn/cbica_tutorials/18_MatlabToArma/">MATLAB to Armadillo code conversion</a>: Leverage the power of low level libraries (BLAS/LAPACK) via easy to use wrappers in Armadillo.


There will be more tutorials as and when we finalize the topics. Please contact us at <a href="mailto:tutorials@cbica.upenn.edu">tutorials[at]cbica[dot]upenn[dot]edu</a> for topic suggestions and questions.

<br><br>

--------------------

The University of Pennsylvania and the Center for Biomedical Image Computing and Analytics assume no responsibility for the code provided in these tutorials. The user is free to use and distribute the code as they see fit as long as they cite the relevant source(s).