/**
\file  cbicaCmdParser.h

\brief Declaration of the CmdParser class

https://www.cbica.upenn.edu/sbia/software/ <br>
software@cbica.upenn.edu

Copyright (c) 2016 University of Pennsylvania. All rights reserved. <br>
See COPYING file or https://www.cbica.upenn.edu/sbia/software/license.html

*/
#pragma once

#include <string>
#include <stdio.h>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>

#include "cbicaUtilities.h"

namespace cbica
{
  /**
  \class CmdParser

  \brief Simple command line parsing

  This is a pure c++ implementation. Executable name and project version are picked up automatically
  from the main CMakeLists file. Only the executable name can be modified in this class.

  An example of usage is shown below:

  \verbatim
  cbica::CmdParser parser = cbica::CmdParser(argc, argv); // OR,
  //cbica::CmdParser parser = cbica::CmdParser(argc, argv, "exe_name"); // if a different exe_name is desired

  /// The parameters "u", "usage", "h", "help", "v" and "version" are automatically added ///

  // add parameters to the variable
  parser.addOptionalParameter("m","marvel", cbica::Parameter::INTEGER, "1 to 10", "I like The Avengers");
  parser.addOptionalParameter("d", "dc", cbica::Parameter::FLOAT, "1.00 to 10.00", "I prefer the Justice League");
  parser.addRequiredParameter("p", "people", cbica::Parameter::STRING, "max length = 1024", "People are always required");

  /// checks for required parameters are done internally.

  std::string peopleString;
  parser.getParameterValue("p", peopleString);

  int marvelValue = 5; // set default value
  parser.getParameterValue("m", marvelValue);

  float dcValue = 5.15; // set default value
  parser.getParameterValue("d", dcValue);

  doSomethingWithTheParameters( peopleString, marvelValue, dcValue );
  \endverbatim
  */
  class CmdParser
  {
  public:
    /**
    \brief The Constructor

    \param argc The "argc" from executable
    \param argv The "argv" from executable
    \param exe_name Name of the executable, defaults to picking up from cbica::getExecutableName()
    */
    explicit CmdParser(const int argc, char **argv, const std::string &exe_name = "");

    /**
    \brief The Constructor

    \param argc The "argc" from executable
    \param argv The "argv" from executable
    \param exe_name Name of the executable, defaults to picking up from cbica::getExecutableName()
    */
    explicit CmdParser(const int argc, const char **argv, const std::string &exe_name = "");

    /**
    \brief The Destructor
    */
    virtual ~CmdParser();

    /**
    \brief Set a custom executable name
    */
    void setExeName(const std::string exeName){ m_exeName = exeName; };

    /**
    \brief Adding parameters: defaults to optional parameters

    As a standard, neither the laconic nor verbose parameters should have any '-' in the constructor.

    \param laconic The laconic variant
    \param verbose The verbose variant
    \param expectedDataType The data type expected for this parameter
    \param dataRange The range of data expected for this parameter
    \param description_line1 The first line of description for parameter
    \param description_line2 The second line of description for parameter, defaults to a blank string
    \param description_line3 The third line of description for parameter, defaults to a blank string
    \param description_line4 The fourth line of description for parameter, defaults to a blank string
    \param description_line5 The fifth line of description for parameter, defaults to a blank string
    */
    void addParameter(const std::string &laconic, const std::string &verbose, const int &expectedDataType, const std::string &dataRange,
      const std::string &description_line1, const std::string &description_line2 = "", const std::string &description_line3 = "",
      const std::string &description_line4 = "", const std::string &description_line5 = "");

    /**
    \brief Adding Optional parameters

    As a standard, neither the laconic nor verbose parameters should have any '-' in the constructor.

    \param laconic The laconic variant
    \param verbose The verbose variant
    \param expectedDataType The data type expected for this parameter
    \param dataRange The range of data expected for this parameter
    \param description_line1 The first line of description for parameter
    \param description_line2 The second line of description for parameter, defaults to a blank string
    \param description_line3 The third line of description for parameter, defaults to a blank string
    \param description_line4 The fourth line of description for parameter, defaults to a blank string
    \param description_line5 The fifth line of description for parameter, defaults to a blank string
    */
    void addOptionalParameter(const std::string &laconic, const std::string &verbose, const int &expectedDataType, const std::string &dataRange,
      const std::string &description_line1, const std::string &description_line2 = "", const std::string &description_line3 = "",
      const std::string &description_line4 = "", const std::string &description_line5 = "");

    /**
    \brief Adding Required parameters

    As a standard, neither the laconic nor verbose parameters should have any '-' in the constructor.

    \param laconic The laconic variant
    \param verbose The verbose variant
    \param expectedDataType The data type expected for this parameter
    \param dataRange The range of data expected for this parameter
    \param description_line1 The first line of description for parameter
    \param description_line2 The second line of description for parameter, defaults to a blank string
    \param description_line3 The third line of description for parameter, defaults to a blank string
    \param description_line4 The fourth line of description for parameter, defaults to a blank string
    \param description_line5 The fifth line of description for parameter, defaults to a blank string
    */
    void addRequiredParameter(const std::string &laconic, const std::string &verbose, const int &expectedDataType, const std::string &dataRange,
      const std::string &description_line1, const std::string &description_line2 = "", const std::string &description_line3 = "",
      const std::string &description_line4 = "", const std::string &description_line5 = "");

    /**
    \brief Display the usage
    */
    void echoUsage();

    /**
    \brief Display verbose usage
    */
    void echoHelp();

    /**
    \brief Display the version details
    */
    void echoVersion();

    /**
    \brief Check parameters WITHOUT hyphens

    Checks for both laconic and verbose variants of the specified parameter.

    \param execParamToCheck Which parameter to check
    \param position Position of parameter in argv else -1
    \return True if parameter found else False
    */
    bool compareParameter(const std::string &execParamToCheck, int &position);

    /**
    \brief Check parameters WITHOUT hyphens

    Checks for both laconic and verbose variants of the specified parameter. Can be used to see if the parameter is present or not.

    \param execParamToCheck Which parameter to check
    \return True if parameter found else False
    */
    bool compareParameter(const std::string &execParamToCheck);

    /**
    \brief Check if supplied parameter is present in the argument list

    Checks for both laconic and verbose variants of the specified parameter. Uses compareParameter() internally.

    \param execParamToCheck Which parameter to check
    \return True if parameter found else False
    */
    bool isPresent(const std::string &execParamToCheck);

    /**
    \brief Get the description analogous with the parameter

    Can search using both laconic and verbose parameters.

    \param parameter Parameter whose description is requested
    \param "NewLine Return with "\n" between description lines if true, defaults to space between lines
    \return Description of parameter
    */
    std::string getDescription(const std::string &execParamToCheck, bool NewLine);

    /**
    \brief Get the data type analogous with the parameter

    Can search using both laconic and verbose parameters.

    \param parameter Parameter whose description is requested
    \return Description of parameter as string
    */
    std::string getDataTypeAsString(const std::string &execParamToCheck);

    /**
    \brief Get the data type analogous with the parameter

    Can search using both laconic and verbose parameters.

    \param parameter Parameter whose description is requested
    \return Description of parameter as Enum Code (Parameter::Type)
    */
    int getDataTypeAsEnumCode(const std::string &execParamToCheck);

    /**
    \brief Write the configuration file for the executable for use in the common GUI framework

    The generated config file is always named 'EXE_NAME.txt'.

    \param dirName The full path of the directory to save the file; defaults to directory specified in cbica::makeTempDir()
    */
    void writeConfigFile(const std::string &dirName = "");

    /**
    \brief Gives a brief example of how to use the executable
    
    This should not contain any references to the executable name (it is automatically picked up). 
    It should start directly with the parameters to be put in.

    \param usageOfExe A string which would correspond to the command line usage AFTER the executable has been called
    */
    void exampleUsage(const std::string &usageOfExe);

    /**
    \brief Get the value of the parameter

    Can search using both laconic and verbose parameters.

    \param execParamToCheck The laconic or verbose variant of the parameter
    \param parameterValue The return value of the parameter as bool
    */
    void getParameterValue(const std::string &execParamToCheck, bool &parameterValue);

    /**
    \brief Get the value of the parameter

    Can search using both laconic and verbose parameters.

    \param execParamToCheck The laconic or verbose variant of the parameter
    \param parameterValue The return value of the parameter as int
    */
    void getParameterValue(const std::string &execParamToCheck, int &parameterValue);

    /**
    \brief Get the value of the parameter

    Can search using both laconic and verbose parameters.

    \param execParamToCheck The laconic or verbose variant of the parameter
    \param parameterValue The return value of the parameter as float
    */
    void getParameterValue(const std::string &execParamToCheck, float &parameterValue);

    /**
    \brief Get the value of the parameter

    Can search using both laconic and verbose parameters.

    \param execParamToCheck The laconic or verbose variant of the parameter
    \param parameterValue The return value of the parameter as std::string (valid for Parameter::Type::FILE, Parameter::Type::DIRECTORY, Parameter::Type::STRING)
    */
    void getParameterValue(const std::string &execParamToCheck, std::string &parameterValue);

  private:
    //! Executable name
    std::string m_exeName;
    //! Version
    std::string m_version;
    //! Example of how to use the executable in question
    std::string m_exampleOfUsage;
    //! CMD variable, used to ensure that 'const' based variables are taken into consideration
    int m_argc;
    //! CMD variable, used to ensure that 'const' based variables are taken into consideration
    std::vector< std::string > m_argv;
    //! Collection of required and optional parameters
    std::vector< Parameter > m_requiredParameters, m_optionalParameters;
    //! Max length of parameters for echoUsage()
    size_t m_maxLength;
    //! Flag to toggle check for maximum overall length
    bool checkMaxLen;
    //! Flag to check for requested help/usage
    bool helpRequested;
    //! Initialize the class
    inline void initializeClass(int &input_argc, std::vector< std::string > &input_argv, const std::string &input_exeName = "");
    //! Get max length
    inline void getMaxLength();
    //! Internal function to check for verbose parameter
    inline void verbose_check(std::string &input_string);
    //! Internal function to write vector of parameters
    inline void writeParameters(const std::vector< Parameter > &inputParameters, bool verbose);
    
    size_t m_maxLaconicLength, //! maximum length of laconic parameters
      m_minVerboseLength; //! maximum length of verbose parameters

  };
}