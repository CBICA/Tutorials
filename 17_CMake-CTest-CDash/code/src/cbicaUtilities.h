/**
\file  cbicaUtilities.h

\brief Some basic utility functions.

This needs c++11 flag enabled in gcc < 4.

https://www.cbica.upenn.edu/sbia/software/ <br>
software@cbica.upenn.edu

Copyright (c) 2016 University of Pennsylvania. All rights reserved. <br>
See COPYING file or https://www.cbica.upenn.edu/sbia/software/license.html

*/
#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <iterator>
#include <cmath>
#include <memory.h>

//#include <type_traits>

/**
\brief Int separators for Parameter, DataType and DataRange to Write and Read Config Files
*/
enum Separator
{
  Param, DataType, DataRange
};

//! String separators corresponding to Separator
#if defined(__GNUC__)  && (__GNUC__ < 5)
  static const char *SeparatorStrings[] = { ":", "%", "*" };
#else
  static std::vector< std::string > SeparatorStrings = { ":", "%", "*" };
#endif

//! Get the Separator as a string from the enum Separator
static inline std::string getSeparator(int enumVal)
{
  return SeparatorStrings[enumVal];
}

/**
\struct CSVDict

\brief A Dictionary structure to for CSV parsing

The CSV File in question is basically a list file consisting of file names (or paths) and labels
*/
struct CSVDict
{
  //! Contains input image file names
  std::vector< std::string > inputImages;

  //! Contains Labels that correspond to each file
  std::vector< double > inputLabels;

  //! Constructor
  CSVDict(const CSVDict &origin) :
    inputImages(origin.inputImages), inputLabels(origin.inputLabels)
  { };

  //! Constructor
  CSVDict()
  {
    inputImages.resize(0);
    inputLabels.resize(0);
  }

  //! Constructor
  CSVDict(const std::vector< std::string > &inputImagesVector, const std::vector< double > &inputLabelVector) :
    inputImages(inputImagesVector), inputLabels(inputLabelVector) {};
};

namespace cbica
{

  //====================================== String stuff ====================================//

  /**
  \brief Splits the input file name into its constituents

  \param dataFile The full file name which is the input
  \param baseName Overwritten with file name without extension
  \param extension Overwritten with extension without '.'
  \param path Overwritten with path to file

  \return True if successful
  */
  bool splitFileName(const std::string &dataFile, std::string &path,
    std::string &baseName, std::string &extension);

  /**
  \brief Splits the string

  \param str String to split
  \param delim Delimiter on the basis of which splitting is to be done
  \return results Output in the form of vector of strings
  */
  std::vector<std::string> stringSplit(const std::string &str, const std::string &delim);

  /**
  \brief Searches for smaller string in larger string and then replaces it with user-defined input

  \param entireString String to search
  \param toReplace String to replace
  \param replaceWith String to replace toReplace with

  \return std::string of result
  */
  std::string replaceString(const std::string &entireString,
    const std::string &toReplace,
    const std::string &replaceWith);

  /**
  \brief Convert const char* to char*

  \param input constant std::string

  \return character pointer
  */
  char* constCharToChar(const std::string &input);

  /**
  \brief Convert const char* to char*

  \param input constant character pointer

  \return character pointer
  */
  char* constCharToChar(const char *input);

  //====================================== Structs that need string stuff ====================================//

  /**
  \struct Parameter

  \brief Holds individual parameter information

  This is a helper struct for internal usage of different functions and classes (right now, the function ReadConfigFile()
  and the class CmdParser() use it). It is not meant to be used from a program directly.
  All variables are self-explanatory. Currently, a maxium of five lines of description are supported.
  */
  struct Parameter
  {
    enum Type
    {
      FILE, DIRECTORY, STRING, INTEGER, FLOAT, BOOLEAN, NONE
    };

    std::string laconic;
    std::string verbose;
    int dataType_enumCode;
    std::string dataType_string;
    std::string dataRange;
    std::string descriptionLine1;
    std::string descriptionLine2; //! defaults to blank
    std::string descriptionLine3; //! defaults to blank
    std::string descriptionLine4; //! defaults to blank
    std::string descriptionLine5; //! defaults to blank

    size_t length;

    //! Constructor with five lines of description and enum_code for dataType
    Parameter(const std::string &in_laconic, const std::string &in_verbose, const int &in_dataType, const std::string &in_dataRange,
      const std::string &in_descriptionLine1, const std::string &in_descriptionLine2 = "", const std::string &in_descriptionLine3 = "",
      const std::string &in_descriptionLine4 = "", const std::string &in_descriptionLine5 = "") :
      laconic(in_laconic), verbose(in_verbose), dataType_enumCode(in_dataType), dataType_string(""), dataRange(in_dataRange),
      descriptionLine1(in_descriptionLine1), descriptionLine2(in_descriptionLine2),
      descriptionLine3(in_descriptionLine3), descriptionLine4(in_descriptionLine4), descriptionLine5(in_descriptionLine5)
    {
      laconic = cbica::replaceString(laconic, "-", "");
      laconic = cbica::replaceString(laconic, "--", "");
      verbose = cbica::replaceString(verbose, "-", "");
      verbose = cbica::replaceString(verbose, "--", "");
      length = laconic.length() + verbose.length();

      // populate dataType_string WRT dataType_enumCode
      switch (in_dataType)
      {
      case FILE:
        dataType_string = "FILE";
        break;
      case DIRECTORY:
        dataType_string = "DIRECTORY";
        break;
      case STRING:
        dataType_string = "STRING";
        break;
      case INTEGER:
        dataType_string = "INTEGER";
        break;
      case FLOAT:
        dataType_string = "FLOAT";
        break;
      case BOOLEAN:
        dataType_string = "BOOL";
        break;
      case NONE:
        dataType_string = "NONE";
        break;
      default:
        dataType_string = "UNKNOWN";
        break;
      }
    }

    //! Constructor with five lines of description and string for dataType
    Parameter(const std::string &in_laconic, const std::string &in_verbose, const std::string &in_dataType, const std::string &in_dataRange,
      const std::string &in_descriptionLine1, const std::string &in_descriptionLine2 = "", const std::string &in_descriptionLine3 = "",
      const std::string &in_descriptionLine4 = "", const std::string &in_descriptionLine5 = "") :
      laconic(in_laconic), verbose(in_verbose), dataType_enumCode(0), dataType_string(in_dataType), dataRange(in_dataRange),
      descriptionLine1(in_descriptionLine1), descriptionLine2(in_descriptionLine2),
      descriptionLine3(in_descriptionLine3), descriptionLine4(in_descriptionLine4), descriptionLine5(in_descriptionLine5)
    {
      laconic = cbica::replaceString(laconic, "-", "");
      laconic = cbica::replaceString(laconic, "--", "");
      verbose = cbica::replaceString(verbose, "-", "");
      verbose = cbica::replaceString(verbose, "--", "");
      length = laconic.length() + verbose.length();

      // populate dataType_enumCode WRT dataType_string
      if (dataType_string == "FILE")
      {
        dataType_enumCode = FILE;
      }
      else if (dataType_string == "DIRECTORY")
      {
        dataType_enumCode = DIRECTORY;
      }
      else if (dataType_string == "STRING")
      {
        dataType_enumCode = STRING;
      }
      else if (dataType_string == "INTEGER")
      {
        dataType_enumCode = INTEGER;
      }
      else if (dataType_string == "FLOAT")
      {
        dataType_enumCode = FLOAT;
      }
      else if ((dataType_string == "BOOL") || (dataType_string == "BOOLEAN"))
      {
        dataType_enumCode = BOOLEAN;
      }
      else if (dataType_string == "NONE")
      {
        dataType_enumCode = NONE;
      }
      else
      {
        dataType_enumCode = -1;
      }
    }

  };

  //======================================== OS stuff ======================================//

  /**
  \brief Check if file exists using istream

  \param fName Filename to check
  \return True if file exists
  */
  bool fileExists(const std::string &fName);

  /**
  \brief Check if directory exists

  \param dName String to check
  \return True if directory exists
  */
  bool directoryExists(const std::string &dName);

  /**
  \brief Return True if path is an existing regular file

  Reimplementation for python's "os.path.isfile": This follows symbolic links,
  so both islink() and isfile() can be true for the same path.

  [Wrap of cbica::fileExists()]

  \param path Filename of file to check

  \return True if path is an existing regular file
  */
  bool isFile(const std::string &path);

  /**
  \brief Return True if path is an existing directory

  Reimplementation of python's "os.path.isdir": This follows symbolic links,
  so both islink() and isdir() can be true for the same path.

  [Wrap of cbica::directoryExists()]

  \param path Directory name to check

  \return True of path is an existing directory
  */
  bool isDir(const std::string &path);

  /**
  \brief Return True if path exists and false for broken symbolic links

  Reimplementation of python's "os.path.exists": On some platforms, this function may return False
  if permission is not granted to execute os.stat() on the requested file, even if the
  path physically exists.

  \param path Path to check

  \return True if path is valid and false for broken symbolic links
  */
  bool exists(const std::string &path);

  /**
  \brief Create a temporary directory

  \param returnDir Path of temporary directory

  \return True if success
  */
  std::string createTmpDir();

  /**
  \brief Create a temporary directory

  Wrap for createTmpDir()

  \return True if success
  */
  std::string createTemporaryDirectory();

  /**
  \brief Create a temporary directory

  Wrap for createTmpDir()

  \return True if success
  */
  std::string makeTemporaryDirectory();

  /**
  \brief Create a temporary directory

  Wrap for createTmpDir()

  \return True if success
  */
  std::string makeTempDir();

  /**
  \brief Create a directory

  \param dir_name Name of directory to be created with full path

  \return True if success
  */
  bool createDir(const std::string &dir_name);

  /**
  \brief Create a directory

  \param dir_name Name of directory to be created with full path
  */
  bool makeDir(const std::string &dir_name);

  /**
  \brief Create a directory

  \param dir_name Name of directory to be created with full path

  \return True if success
  */
  bool createDirectory(const std::string &dir_name);

  /**
  \brief Create a directory

  \param dir_name Name of directory to be created with full path
  */
  bool makeDirectory(const std::string &dir_name);

  /**
  \brief Create a directory

  \param dir_name Name of directory to be created with full path

  \return True if success
  */
  bool createFolder(const std::string &dir_name);

  /**
  \brief Create a directory

  \param dir_name Name of directory to be created with full path
  */
  bool makeFolder(const std::string &dir_name);

  /**
  \brief Recursively delete a folder and contents [internal function]

  \param dirname Folder to delete

  \return true for success
  */
  int removeDirectoryRecursively(const std::string &dirname, bool bDeleteSubdirectories);

  /**
  \brief Delete a folder and contents

  \param path Folder to delete

  \return true for success
  */
  bool removeDir(const std::string &path);

  /**
  \brief Delete a folder and contents

  \param path Folder to delete

  \return true for success
  */
  bool deleteDir(const std::string &path);

  /**
  \brief Copy a folder and if recursion enabled, all its contents

  https://msdn.microsoft.com/en-us/library/hh874694.aspx?f=255&MSPPError=-2147217396

  \param inputFolder Folder to copy
  \param destination Where to copy to
  \param recursion Do recursion and copy, defaults to true

  \return true for success
  */
  bool copyDir(const std::string &inputFolder, const std::string &destination, bool recursion = true);

  /**
  \brief Copy a folder and if recursion enabled, all its contents

  https://msdn.microsoft.com/en-us/library/hh874694.aspx?f=255&MSPPError=-2147217396

  \param inputFolder Folder to copy
  \param destination Where to copy to
  \param recursion Do recursion and copy, defaults to true

  \return true for success
  */
  bool copyDirectory(const std::string &inputFolder, const std::string &destination, bool recursion = true);

  /**
  \brief Copy a folder and if recursion enabled, all its contents

  https://msdn.microsoft.com/en-us/library/hh874694.aspx?f=255&MSPPError=-2147217396

  \param inputFolder Folder to copy
  \param destination Where to copy to
  \param recursion Do recursion and copy, defaults to true

  \return true for success
  */
  bool copyFolder(const std::string &inputFolder, const std::string &destination, bool recursion = true);

  /**
  \brief Copy a folder and if recursion enabled, all its contents

  \param inputFile File to copy
  \param destination Where to copy to
  \return true for success
  */
  bool copyFile(const std::string &inputFile, const std::string &destination);

  /**
  \brief Get the size of the file in bytes

  \param inputFile The input file
  */
  size_t getFileSize(const std::string &inputFile);

  /**
  \brief Get the size of the folder

  \param rootFolder The input folder
  */
  size_t getFolderSize(const std::string &rootFolder);

  /**
  \brief Get the size of the folder

  \param rootFolder The input folder
  */
  size_t getDirSize(const std::string &rootFolder);

  /**
  \brief Get the size of the folder

  \param rootFolder The input folder
  */
  size_t getDirectorySize(const std::string &rootFolder);

  /**
  \brief Gets the extension of the supplied file name using splitFileName()

  Prefer to use "/" as file path delimiter.

  \param filename The input filename
  \param checkFile Checks existence of file using fileExists
  \return std::string which has the file extension
  */
  std::string getFilenameExtension(const std::string &filename, bool checkFile = true);

  /**
  \brief Gets the base of the supplied file name using splitFileName()

  Prefer to use "/" as file path delimiter.

  \param filename The input filename
  \param checkFile Checks existence of file using fileExists
  \return std::string which has the file extension
  */
  std::string getFilenameBase(const std::string &filename, bool checkFile = true);

  /**
  \brief Gets the path of the supplied file name using splitFileName()

  Prefer to use "/" as file path delimiter.

  \param filename The input filename
  \param checkFile Checks existence of file using fileExists
  \return std::string which has the file extension
  */
  std::string getFilenamePath(const std::string &filename, bool checkFile = true);

  /**
  \brief Get the name of the Executable which is calling the function

  \return exe name
  */
  std::string getExecutableName();

  /**
  \brief Get the path of the Executable which is calling the function

  returns the path after calling splitFileName on getFullPath

  \return exe name
  */
  std::string getExecutablePath();

  /**
  \brief Get the name of the Executable which is calling the function

  \return exe name
  */
  std::string getFullPath();

  /**
  \brief Get the name of the user who is calling the function

  \return user name
  */
  std::string getUserName();

  /**
  \brief Get the home directory of the user

  Windows - C:/Users/XYZ
  Linux - /home/XYZ

  \return user directory
  */
  std::string getUserHomeDirectory();

  /**
  \brief Get the current working directory

  [Wrap for cbica::getcwd()]

  \return Current working directory
  */
  std::string getCWD();

  /**
  \brief Normalize a pathname by collapsing redundant separators and up-level references

  Reimplementation of python's "os.path.normpath": Normalize a pathname by collapsing redundant separators
  and up-level references so that A//B, A/B/, A/./B and A/foo/../B all become A/B. This string
  manipulation may change the meaning of a path that contains symbolic links. On Windows, it
  converts forward slashes to backward slashes.

  \param path Path to normalize

  \return std::string Normalized path
  */
  std::string normPath(const std::string &path);

  /**
  \brief Normalize a pathname by collapsing redundant separators and up-level references

  [Wrap for cbica::normPath()]

  \param path Path to normalize

  \return std::string Normalized path
  */
  std::string normalizePath(const std::string &path);

  /**
  \brief Return a relative filepath to path

  Reimplementation for python's "os.path.relpath": Return a relative filepath to path either from the
  current directory or from an optional start directory. This is a path computation: the
  filesystem is not accessed to confirm the existence or nature of path or start.

  \param path Path to check
  \param base Base file name

  \return Relative filepath to path either from current directory or from an optional start dir
  */
  std::string relPath(const std::string &path, const std::string &base);

  /**
  \brief Return a relative filepath to path

  Wrap of cbica::relPath()

  \param path Path to check
  \param base Base file name

  \return Relative filepath to path either from current directory or from an optional start dir
  */
  std::string relativePath(const std::string &path, const std::string &base);

  /**
  \brief Return the canonical path of the specified filename

  Reimplementation of python's "os.path.realpath": Return the canonical path of the specified
  filename, eliminating any symbolic links encountered in the path (if they are
  supported by the operating system).

  \param path Filename to check

  \return canonical path of path
  */
  std::string realPath(const std::string &path);

  /**
  \brief Return True if path refers to a directory entry that is a symbolic link

  Reimplementation of python's "os.path.islink": Always False if symbolic links are not
  supported by the python runtime.

  \param path Path to check

  \return True if path is symbolic link
  */
  bool isLink(const std::string &path);

  /**
  \brief Check if path refers to a symbolic entry

  [Wrap of cbica::isLink()]

  \param path Path to check

  \return True if path is symbolic link
  */
  bool isSymbolicLink(const std::string &path);

  /**
  \brief Make a symbolic link from file to another

  \param input_fileName Input file name for which symbolic link needs to be created
  \param output_fileName Output file name which is the symbolic link for input_fileName

  \return True if symbolic link successfully created
  */
  bool makeSymbolicLink(const std::string &input_fileName, const std::string &ouput_fileName);

  /**
  \brief Sets the environment variable

  \param variable_name Name of the Variable
  \param variable_value Value of variable_name

  \return True if successful
  */
  bool setEnvironmentVariable(const std::string &variable_name, const std::string &variable_value);

  /**
  \brief Gets the value of the specified environment variable
  */
  std::string getEnvironmentVariableValue(const std::string &environmentVariable);

  /**
  \brief Delete the environment variable

  \param variable_name Name of the Variable

  \return True if successful
  */
  bool deleteEnvironmentVariable(const std::string &variable_name);

  /**
  \brief Find all files inside a directory

  \param dirName The directory to do the search in
  */
  std::vector< std::string > filesInDirectory(const std::string &dirName);

  /**
  \brief Find all sub-directories inside a directory

  \param dirName The directory to do the search in
  \param recursiveSearch Whether to do a recursive search or on a single level
  */
  std::vector<std::string> subdirectoriesInDirectory(const std::string &dirName, bool recursiveSearch = false);

  /**
  \brief Find number of rows in CSV file
  */
  size_t numberOfRowsInFile(const std::string &csvFileName, const std::string &delim = "\n");

  /**
  \brief Find number of cols in CSV file
  */
  size_t numberOfColsInFile(const std::string &csvFileName, const std::string &delim = ",");

  /**
  \brief Parse the supplied CSV File and obtain Row and Column information.

  Assumptions:
  1. Header information is in first row
  2. Full paths of images are given
  3. Image paths are either absolute (default behaviour) OR relative to the location of CSV file OR relative to CWD

  \param csvFileName The full path of the file to parse, all paths are absolute or relative to current working directory
  \param inputColumns The string of input columns which contain the data to be used for further processing
  \param inputLabels The string of input labels per subject based on which further processing is to be done; if this is empty, it is initialized as 1 for all subjects
  \param checkFile Check the validity of the file; defaults to true
  \param pathsRelativeToCSV The paths in the CSV file are relative to the location of CSV file otherwise, they are checked relative to the CWD
  \param rowsDelimiter The delimiters used to distinguish rows in the file
  \param colsDelimiter The delimiters used to distinguish cols in the file
  \param optionsDelimiter The delimiters used in inputColumns and inputLabel
  \return Vector of CSV Dictionary items: Collection of input images are respective labels
  */
  std::vector< CSVDict > parseCSVFile(const std::string &csvFileName, const std::string &inputColumns, const std::string &inputLabels, bool checkFile = true, bool pathsRelativeToCSV = false, const std::string &rowsDelimiter = "\n", const std::string &colsDelimiter = ",", const std::string &optionsDelimiter = ",");

  /**
  \brief Reads a pre-written configuration file using CmdParser::WriteConfigFile()

  \param inputConfigFile Full path to the configuration file which needs to be read
  \return Vector of the Parameter structure where laconic paramter is always empty for all variables
  */
  std::vector< Parameter > readConfigFile(const std::string &inputConfigFile, bool getDescription = true);

  /**
  \brief Get current local time as string delineated as YYYY:MM:DD
  */
  std::string getCurrentLocalDate();

  /**
  \brief Get current local time as string delineated as HH:MM:SS
  */
  std::string getCurrentLocalTime();

  /**
  \brief Get current local time as string delineated as YYYY:MM:DD,HH:MM:SS
  */
  std::string getCurrentLocalDateAndTime();

  /**
  \brief Get current GMT as string delineated as YYYY:MM:DD
  */
  std::string getCurrentGMTDate();

  /**
  \brief Get current GMT as string delineated as HH:MM:SS
  */
  std::string getCurrentGMT();

  /**
  \brief Get current GMT as string delineated as YYYY:MM:DD,HH:MM:SS
  */
  std::string getCurrentGMTDateAndTime();

  /**
  \brief Get current Year as string delineated as YYYY
  */
  std::string getCurrentYear();

  /**
  \brief Cross platform sleep

  Defaults to "std::rand() % 1000 + 1"
  */
  void sleep(size_t ms = std::rand() % 1000 + 1);
  //==================================== Template stuff ==================================//

  /**
  \brief Searches for an element in a vector and returns true/false and position

  Templated function to take in any kind of vector and element.

  \param vector_to_search_in Vector to do the search in
  \param element_to_search_for Element to search for
  \param position Last position of found element in vector (-1) if not found

  \return True if found
  \return Position if found (-1) if not
  */
  template<typename TContainerType = std::string >
  std::pair<bool, int> findInVector(std::vector<TContainerType> &vector_to_search_in,
    TContainerType element_to_search_for)
  {
    int position = -1;
    //std::vector<int>::const_iterator
    typename std::vector<TContainerType>::const_iterator iterator =
      std::find(vector_to_search_in.begin(), vector_to_search_in.end(), element_to_search_for);
    if (iterator != vector_to_search_in.end())
    {
      position = iterator - vector_to_search_in.begin();
      return std::make_pair(true, position);
    }
    else
      return std::make_pair(false, position);
  }

  /**
  \brief Convert first character to integer, double, unsigned int, etc.

  \param input_string Input character to be converted
  \return Templated to the type of return required
  */
  template<typename TConvertType
#if (_MSC_VER >= 1800) || (__GNUC__ > 4)
    = int
#endif
>
  /*typename*/ TConvertType convertCharacter(const std::string &input_string)
  {
    return static_cast<TConvertType>(input_string.at(0));
  }

  /**
  \brief Convert entire string to integer, double, unsigned int, etc.

  \param input_string Input character to be converted
  \return Templated vector to the type of return required
  */
  template<typename TConvertType = int >
  std::vector</*typename*/ TConvertType> convertString(const std::string &input_string)
  {
    std::vector</*typename*/TConvertType>return_vector;
    for (int i = 0; i<input_string.length(); ++i)
      return_vector.push_back(static_cast<TConvertType>(input_string.at(i)));

    return return_vector;
  }
  
#if (_MSC_VER >= 1800) || __GXX_EXPERIMENTAL_CXX0X__ || (__GNUC__ > 4)
  /**
  \brief Base for compareEqual(...)
  */
  template <typename A = int, typename B = A>
  inline bool compareEqual(const A x, const B y)
  {
    return (x == y);
  }

  /**
  \brief Compare if equal for multiple inputs
  */
  template <typename A = int, typename B = A, typename... Others>
  bool compareEqual(const A x, const B y, Others const ... args)
  {
    return (x == y) && compareEqual(y, args...);
  }

  /**
  \brief Base for compareGreater(...)
  */
  template <typename A = int, typename B = A>
  inline bool compareGreater(const A x, const B y)
  {
    return (x > y);
  }

  /**
  \brief Compare if greater for multiple inputs
  */
  template <typename A = int, typename B = A, typename... Others>
  bool compareGreater(const A x, const B y, Others const ... args)
  {
    return (x > y) && compareGreater(y, args...);
  }

  /**
  \brief Base for compareLesser(...)
  */
  template <typename A = int, typename B = A>
  inline bool compareLesser(const A x, const B y)
  {
    return (x < y);
  }

  /**
  \brief Compare if lesser for multiple inputs
  */
  template <typename A = int, typename B = A, typename... Others>
  bool compareLesser(const A x, const B y, Others const ... args)
  {
    return (x < y) && compareLesser(y, args...);
  }

#endif

  /**
  \brief Compute the MD5 checksum for supplied file

  This class uses the gdcm::MD5 class and that is a dependency for this function.
  */
  std::string computeMD5Sum(const std::string &fileName);


};

#if defined(__GNUC__)  && (__GNUC__ < 5) && !(__APPLE__) && !(__GXX_EXPERIMENTAL_CXX0X__)
namespace std
{
  //! std::round wrap for GCC
  inline int round(const float &input)
  {
    int returnValue;
    const float inputWrap = abs(input);
    inputWrap >= floor(inputWrap) + 0.5 ? returnValue = ceil(inputWrap) : returnValue = floor(inputWrap);

    if (input < 0)
    {
      returnValue = -(returnValue);
    }

    return returnValue;
  }

  //! std::round wrap for GCC
  inline int round(const double &input)
  {
    int returnValue;
    const double inputWrap = abs(input);
    inputWrap >= floor(inputWrap) + 0.5 ? returnValue = ceil(inputWrap) : returnValue = floor(inputWrap);

    if (input < 0)
    {
      returnValue = -(returnValue);
    }

    return returnValue;
  }

  //! std::round wrap for GCC
  inline int round(const long int &input)
  {
    int returnValue;
    const long int inputWrap = abs(input);
    inputWrap >= floor(inputWrap) + 0.5 ? returnValue = ceil(inputWrap) : returnValue = floor(inputWrap);

    if (input < 0)
    {
      returnValue = -(returnValue);
    }

    return returnValue;
  }

  //! std::round wrap for GCC
  inline int round(const unsigned int &input)
  {
    int returnValue;
    const unsigned int inputWrap = abs(input);
    inputWrap >= floor(inputWrap) + 0.5 ? returnValue = ceil(inputWrap) : returnValue = floor(inputWrap);

    if (input < 0)
    {
      returnValue = -(returnValue);
    }

    return returnValue;
  }

  //! std::round wrap for GCC
  inline int round(const short &input)
  {
    int returnValue;
    const short inputWrap = abs(input);
    inputWrap >= floor(inputWrap) + 0.5 ? returnValue = ceil(inputWrap) : returnValue = floor(inputWrap);

    if (input < 0)
    {
      returnValue = -(returnValue);
    }

    return returnValue;
  }

  //! std::to_string wrap for GCC
  inline std::string to_string(const int &input)
  {
    std::ostringstream ss;
    ss << input;
    return ss.str();
  }

  //! std::to_string wrap for GCC
  inline std::string to_string(const unsigned int &input)
  {
    std::ostringstream ss;
    ss << input;
    return ss.str();
  }

  //! std::to_string wrap for GCC
  inline std::string to_string(const size_t &input)
  {
    std::ostringstream ss;
    ss << input;
    return ss.str();
  }

  //! std::to_string wrap for GCC
  inline std::string to_string(const long int &input)
  {
    std::ostringstream ss;
    ss << input;
    return ss.str();
  }

  //! std::to_string wrap for GCC
  inline std::string to_string(const double &input)
  {
    std::ostringstream ss;
    ss << input;
    return ss.str();
  }

  //! std::to_string wrap for GCC
  inline std::string to_string(const float &input)
  {
    std::ostringstream ss;
    ss << input;
    return ss.str();
  }
  
};
#endif

/**
\struct FileNameParts

\brief Holds the different parts of a file name (path, base and extension)

This is a helper struct for uses concerning uses where different parts of a file (path, base, ext) need to be used multiple times.
*/
struct FileNameParts
{
  std::string fullFileName, path, base, extension;

  //! Default Constructor
  FileNameParts();

  /**
  \brief Constructor with input file string

  \param inputFileName The complete input file name
  */
  FileNameParts(const std::string &inputFileName) : fullFileName(inputFileName)
  {
    cbica::replaceString(fullFileName, "\\", "/");
    if (cbica::fileExists(inputFileName))
      cbica::splitFileName(fullFileName, path, base, extension);
    else
    {
      std::cerr << "The input file '" << inputFileName << "' wasn't found on disk.\n";
    }
  }

  /**
  \brief Member function to set the fullFileName

  \param inputFileName The complete input file name
  */
  void SetFileName(const std::string &inputFileName)
  {
    fullFileName = inputFileName;
    cbica::replaceString(fullFileName, "\\", "/");
    if (cbica::fileExists(fullFileName))
      cbica::splitFileName(fullFileName, path, base, extension);
    else
    {
      std::cerr << "The input file '" << inputFileName << "' wasn't found on disk.\n";
    }
  }
};
