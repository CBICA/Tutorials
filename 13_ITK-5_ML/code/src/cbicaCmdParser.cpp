/**
\file  cbicaCmdParser.cpp

\brief Implementation file for CmdParser class.

https://www.cbica.upenn.edu/sbia/software/ <br>
software@cbica.upenn.edu

Copyright (c) 2015 University of Pennsylvania. All rights reserved. <br>
See COPYING file or https://www.cbica.upenn.edu/sbia/software/license.html

*/
#include <functional>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#if (_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif


#include "cbicaCmdParser.h"

#ifndef PROJECT_VERSION
#define PROJECT_VERSION "0.0.1"
#endif


namespace cbica
{
  CmdParser::CmdParser(int argc, char **argv, const std::string &exe_name /*= ""*/)
  {
#ifdef PROJECT_VERSION
    m_version = PROJECT_VERSION;
#else
    m_version = 0.1.0;
#endif    
    if (exe_name == "")
    {
      m_exeName = cbica::getExecutableName();
    }
    else
    {
      m_exeName = exe_name;
    }
    m_maxLength = 0;
    checkMaxLen = false;
    helpRequested = false;
    m_exampleOfUsage = "";

    m_argc = argc;
    m_argv = argv;

    m_optionalParameters.push_back(Parameter("u", "usage", cbica::Parameter::NONE, "", "Prints basic usage message.", "", "", "", ""));
    m_optionalParameters.push_back(Parameter("h", "help", cbica::Parameter::NONE, "", "Prints verbose usage information.", "", "", "", ""));
    m_optionalParameters.push_back(Parameter("v", "version", cbica::Parameter::NONE, "", "Prints information about software version.", "", "", "", ""));
  }

  CmdParser::CmdParser(int argc, const char **argv, const std::string &exe_name /*= ""*/)
  {
    char *argv2[FILENAME_MAX];
    //*argv[0] = " ";
    //*argv[1] = "1";
    for (int i = 0; i < argc; i++)
    {
      argv2[i] =
#ifdef _WIN32
        _strdup
#else
        strdup
#endif    
      (argv[i]);
    }
    CmdParser(argc, argv2, exe_name);
  }

  CmdParser::~CmdParser()
  {

  }

  inline void copyrightNotice()
  {
    std::cout <<
      "\n==========================================================================\n" <<
      "Contact: software@cbica.upenn.edu\n\n" <<
      "Copyright (c) 2016 University of Pennsylvania. All rights reserved.\n" <<
      "See COPYING file or https://www.cbica.upenn.edu/sbia/software/license.html" <<
      "\n==========================================================================\n";
  }

  inline void CmdParser::getMaxLength()
  {
    m_minVerboseLength = 1024;
    m_maxLaconicLength = 0;
    m_maxLength = 0; // maximum length of laconic + verbose

    // loop through optional and required parameters separately
    for (size_t i = 0; i<m_optionalParameters.size(); ++i)
    {
      m_maxLength = m_maxLength < m_optionalParameters[i].length ? m_optionalParameters[i].length : m_maxLength;
      m_minVerboseLength = m_minVerboseLength > m_optionalParameters[i].verbose.length() ? m_optionalParameters[i].verbose.length() : m_minVerboseLength;
      m_maxLaconicLength = m_maxLaconicLength < m_optionalParameters[i].laconic.length() ? m_optionalParameters[i].laconic.length() : m_maxLaconicLength;
    }

    for (size_t i = 0; i < m_requiredParameters.size(); ++i)
    {
      m_maxLength = m_maxLength < m_requiredParameters[i].length ? m_requiredParameters[i].length : m_maxLength;
      m_minVerboseLength = m_minVerboseLength > m_requiredParameters[i].verbose.length() ? m_requiredParameters[i].verbose.length() : m_minVerboseLength;
      m_maxLaconicLength = m_maxLaconicLength < m_requiredParameters[i].laconic.length() ? m_requiredParameters[i].laconic.length() : m_maxLaconicLength;
    }

    checkMaxLen = true; // trigger flag for future checks

    if (!helpRequested && (m_argc != 1))
    {
      for (size_t i = 0; i<m_requiredParameters.size(); ++i)
      {
        // check if current required parameter has been supplied in the command line (obtained from argv)
        int tempPos;
        if (!CmdParser::compareParameter(m_requiredParameters[i].laconic, tempPos) && !helpRequested)
        {
          std::cout << "The required parameter '" << m_requiredParameters[i].laconic << "' is missing from the command line arguments you provided. See '" <<
            m_exeName << " --help' for extended help.\n\n";

          std::string m_exeName_wrap;
#ifdef _WIN32
          m_exeName_wrap = m_exeName + ".exe";
#else
          m_exeName_wrap = "./" + m_exeName;
#endif

          std::cout << "An exemplary usage scenario: \n\n" << m_exeName_wrap << " " << m_exampleOfUsage << "\n\n";

          exit(EXIT_FAILURE);
        }
      }
    }
  }

  void CmdParser::addOptionalParameter(const std::string &laconic, const std::string &verbose, const int &expectedDataType, const std::string &dataRange,
    const std::string &description_line1,
    const std::string &description_line2,
    const std::string &description_line3,
    const std::string &description_line4,
    const std::string &description_line5)
  {
    if ((laconic == "u") || (laconic == "h") || (laconic == "v"))
    {
      return;
    }
    if (laconic == "")
    {
      std::cerr << "Laconic parameter cannot be empty";
      exit(EXIT_FAILURE);
    }
    if (verbose == "")
    {
      std::cerr << "Verbose parameter cannot be empty";
      exit(EXIT_FAILURE);
    }
    if (description_line1 == "")
    {
      std::cerr << "Failure to initialize an empty string as description_line1";
      exit(EXIT_FAILURE);
    }

    m_optionalParameters.push_back(Parameter(laconic, verbose, expectedDataType, dataRange, description_line1, description_line2, description_line3, description_line4, description_line5));
  }

  void CmdParser::addRequiredParameter(const std::string &laconic, const std::string &verbose, const int &expectedDataType, const std::string &dataRange,
    const std::string &description_line1,
    const std::string &description_line2,
    const std::string &description_line3,
    const std::string &description_line4,
    const std::string &description_line5)
  {
    if ((laconic == "u") || (laconic == "h") || (laconic == "v"))
    {
      return;
    }
    if (laconic == "")
    {
      std::cerr << "Laconic parameter cannot be empty";
      exit(EXIT_FAILURE);
    }
    if (verbose == "")
    {
      std::cerr << "Verbose parameter cannot be empty";
      exit(EXIT_FAILURE);
    }
    if (description_line1 == "")
    {
      std::cerr << "Failure to initialize an empty string as description_line1";
      exit(EXIT_FAILURE);
    }

    m_requiredParameters.push_back(Parameter(laconic, verbose, expectedDataType, dataRange, description_line1, description_line2, description_line3, description_line4, description_line5));
  }

  void CmdParser::addParameter(const std::string &laconic, const std::string &verbose, const int &expectedDataType, const std::string &dataRange,
    const std::string &description_line1,
    const std::string &description_line2,
    const std::string &description_line3,
    const std::string &description_line4,
    const std::string &description_line5)
  {
    CmdParser::addOptionalParameter(laconic, verbose, expectedDataType, dataRange, description_line1, description_line2, description_line3, description_line4, description_line5);
  }

  inline void CmdParser::writeParameters(const std::vector< Parameter > &inputParameters, bool verbose)
  {
    std::string spaces_verb_line2;
    for (size_t n = 0; n < m_maxLength + 9; n++)
    {
      spaces_verb_line2.append(" ");
    }
    for (size_t i = 0; i < inputParameters.size(); i++)
    {
      std::string spaces_lac, spaces_verb;

      for (size_t n = 0; n < m_maxLaconicLength - inputParameters[i].laconic.length(); n++)
      {
        spaces_lac.append(" ");
      }

      for (size_t n = 0; n < m_maxLength - inputParameters[i].length - spaces_lac.length(); n++)
      {
        spaces_verb.append(" ");
      }

      std::cout << "[" << spaces_lac << "-" << inputParameters[i].laconic << ", --" <<
        inputParameters[i].verbose << spaces_verb << "]  " <<
        inputParameters[i].descriptionLine1 << "\n";

      if (inputParameters[i].descriptionLine2 != "")
      {
        std::cout << spaces_verb_line2 << inputParameters[i].descriptionLine2 << "\n";
        if (inputParameters[i].descriptionLine3 != "")
        {
          std::cout << spaces_verb_line2 << inputParameters[i].descriptionLine3 << "\n";
          if (inputParameters[i].descriptionLine4 != "")
          {
            std::cout << spaces_verb_line2 << inputParameters[i].descriptionLine4 << "\n";
            if (inputParameters[i].descriptionLine5 != "")
            {
              std::cout << spaces_verb_line2 << inputParameters[i].descriptionLine5 << "\n";
            }
          }
        }
      }

      if (verbose && (inputParameters[i].laconic != "u") && (inputParameters[i].laconic != "h") && (inputParameters[i].laconic != "v"))
      {
        std::cout << spaces_verb_line2 << "Expected Type  :: " << inputParameters[i].dataType_string << "\n" <<
          spaces_verb_line2 << "Expected Range :: " << inputParameters[i].dataRange << "\n";
      }

      std::cout << "\n"; // an extra to keep coherence
    }
  }

  void CmdParser::echoUsage()
  {
    if (!checkMaxLen)
    {
      getMaxLength();
    }
    std::cout << "Executable Name: " << m_exeName << " v" << m_version
      << "\n\n" << "Usage:\n\n";

    std::cout << "Required parameters:\n\n";
    writeParameters(m_requiredParameters, false);
    std::cout << "Optional parameters:\n\n";
    writeParameters(m_optionalParameters, false);

    copyrightNotice();
  }

  void CmdParser::echoHelp()
  {
    if (!checkMaxLen)
    {
      getMaxLength();
    }
    std::cout << "Executable Name: " << m_exeName << " v" << m_version
      << "\n\n" << "Usage:\n\n";

    std::cout << ":::Required parameters:::\n\n";
    writeParameters(m_requiredParameters, true);
    std::cout << ":::Optional parameters:::\n\n";
    writeParameters(m_optionalParameters, true);

    std::string m_exeName_wrap;
#ifdef _WIN32
    m_exeName_wrap = m_exeName + ".exe";
#else
    m_exeName_wrap = "./" + m_exeName;
#endif

    if (m_exampleOfUsage != "")
    {
      std::cout << "For example: \n\n" << 
        m_exeName_wrap << " " << m_exampleOfUsage << "\n";
    }

    copyrightNotice();
  }

  void CmdParser::echoVersion()
  {
    std::cout << "Executable Name: " << m_exeName << "\n" << "        Version: " <<
      m_version << "\n";

    copyrightNotice();
  }

  inline std::string internal_compare(const std::string &check_string, const int check_length)
  {
    switch (std::abs(static_cast<int>(check_string.length() - check_length)))
    {
    case 1:
      return ("-" + check_string);
      break;
    case 2:
      return ("--" + check_string);
      break;
    default:
      return (check_string);
      break;
    }
  }

  inline void CmdParser::verbose_check(std::string &input_string)
  {
    if (input_string == "usage")
    {
      input_string = "u";
    }
    else if (input_string == "help")
    {
      input_string = "h";
    }
    else if (input_string == "version")
    {
      input_string = "v";
    }

    if (input_string.length() > m_maxLaconicLength)
    {
      input_string = cbica::replaceString(input_string, "--", "");
      input_string = cbica::replaceString(input_string, "-", "");

      for (size_t i = 0; i < m_requiredParameters.size(); i++)
      {
        input_string = m_requiredParameters[i].verbose == input_string ? m_requiredParameters[i].laconic : input_string;
      }

      for (size_t i = 0; i < m_optionalParameters.size(); i++)
      {
        input_string = m_optionalParameters[i].verbose == input_string ? m_optionalParameters[i].laconic : input_string;
      }

      return;
    }
  }

  bool CmdParser::compareParameter(const std::string &execParamToCheck, int &position)
  {
    std::string execParamToCheck_wrap = execParamToCheck;
    verbose_check(execParamToCheck_wrap);

    for (int i = 1; i < m_argc; i++)
    {
      std::string inputParamToCheck = m_argv[i];
      if (!checkMaxLen)
      {
        getMaxLength();
      }
      verbose_check(inputParamToCheck);
      if ((inputParamToCheck == "u") || (inputParamToCheck == "h") || (inputParamToCheck == "v"))
      {
        helpRequested = true;
        position = i;
        //return true;
      }

      if (inputParamToCheck == execParamToCheck_wrap)
      {
        position = i;
        return true;
      }
      else
      {
        std::string inputCheck, execCheck;
        const unsigned int minLength = static_cast<unsigned int>(std::max(
          inputParamToCheck.length(), execParamToCheck_wrap.length()));

        inputCheck = internal_compare(inputParamToCheck, minLength);
        execCheck = internal_compare(execParamToCheck_wrap, minLength);

        if (inputCheck == execCheck)
        {
          position = i;
          return true;
        }
      }
    }

    return false;
  }

  bool CmdParser::compareParameter(const std::string &execParamToCheck)
  {
    int position;
    return compareParameter(execParamToCheck, position);
  }

  bool CmdParser::isPresent(const std::string &execParamToCheck)
  {
    return compareParameter(execParamToCheck);
  }

  std::string CmdParser::getDescription(const std::string &execParamToCheck, bool NewLine = false)
  {
    int noMoreChecks = 0; // ensures that extra checks are not done for parameters
    if (execParamToCheck == "")
    {
      std::cerr << "Parameter cannot be an empty string. Please try again.\n";
      exit(EXIT_FAILURE);
    }
    if (!checkMaxLen)
    {
      getMaxLength();
    }

    size_t i = 0;
    while ((i < m_requiredParameters.size()) && (noMoreChecks < 1))
    {
      if ((m_requiredParameters[i].laconic == execParamToCheck) ||
        (m_requiredParameters[i].verbose == execParamToCheck))
      {
        if (NewLine)
        {
          return m_requiredParameters[i].descriptionLine1 + "\n" + m_requiredParameters[i].descriptionLine2 + "\n" +
            m_requiredParameters[i].descriptionLine3 + "\n" + m_requiredParameters[i].descriptionLine4 + "\n" +
            m_requiredParameters[i].descriptionLine5;
        }
        else
        {
          return m_requiredParameters[i].descriptionLine1 + " " + m_requiredParameters[i].descriptionLine2 + " " +
            m_requiredParameters[i].descriptionLine3 + " " + m_requiredParameters[i].descriptionLine4 + " " +
            m_requiredParameters[i].descriptionLine5;
        }
        noMoreChecks = 1;
      }
      i++;
    }

    i = 0;
    while ((i < m_optionalParameters.size()) && (noMoreChecks < 1))
    {
      if ((m_optionalParameters[i].laconic == execParamToCheck) ||
        (m_optionalParameters[i].verbose == execParamToCheck))
      {
        if (NewLine)
        {
          return m_optionalParameters[i].descriptionLine1 + "\n" + m_optionalParameters[i].descriptionLine2 + "\n" +
            m_optionalParameters[i].descriptionLine3 + "\n" + m_optionalParameters[i].descriptionLine4 + "\n" +
            m_optionalParameters[i].descriptionLine5;
        }
        else
        {
          return m_optionalParameters[i].descriptionLine1 + " " + m_optionalParameters[i].descriptionLine2 + " " +
            m_optionalParameters[i].descriptionLine3 + " " + m_optionalParameters[i].descriptionLine4 + " " +
            m_optionalParameters[i].descriptionLine5;
        }
        noMoreChecks = 1;
      }
      i++;
    }

    return "";
  }

  std::string CmdParser::getDataTypeAsString(const std::string &execParamToCheck)
  {
    int noMoreChecks = 0; // ensures that extra checks are not done for parameters
    if (execParamToCheck == "")
    {
      std::cerr << "Parameter cannot be an empty string. Please try again.\n";
      exit(EXIT_FAILURE);
    }
    if (!checkMaxLen)
    {
      getMaxLength();
    }

    size_t i = 0;
    while ((i < m_requiredParameters.size()) && (noMoreChecks < 1))
    {
      if ((m_requiredParameters[i].laconic == execParamToCheck)  ||
        (m_requiredParameters[i].verbose == execParamToCheck))
      {
        return m_requiredParameters[i].dataType_string;
        noMoreChecks = 1;
      }
      i++;
    }

    i = 0;
    while ((i < m_optionalParameters.size()) && (noMoreChecks < 1))
    {
      if ((m_optionalParameters[i].laconic == execParamToCheck) ||
        (m_optionalParameters[i].verbose == execParamToCheck))
      {
        return m_optionalParameters[i].dataType_string;
        noMoreChecks = 1;
      }
      i++;
    }

    return "";
  }

  int CmdParser::getDataTypeAsEnumCode(const std::string &execParamToCheck)
  {
    bool noMoreChecks = false; // ensures that extra checks are not done for parameters
    if (execParamToCheck == "")
    {
      std::cerr << "Parameter cannot be an empty string. Please try again.\n";
      exit(EXIT_FAILURE);
    }
    if (!checkMaxLen)
    {
      getMaxLength();
    }

    size_t i = 0;
    while ((i < m_requiredParameters.size()) && !noMoreChecks)
    {
      if ((m_requiredParameters[i].laconic == execParamToCheck) ||
        (m_requiredParameters[i].verbose == execParamToCheck))
      {
        return m_requiredParameters[i].dataType_enumCode;
        noMoreChecks = true;
      }
      i++;
    }

    i = 0;
    while ((i < m_optionalParameters.size()) && !noMoreChecks)
    {
      if ((m_optionalParameters[i].laconic == execParamToCheck) ||
        (m_optionalParameters[i].verbose == execParamToCheck))
      {
        return m_optionalParameters[i].dataType_enumCode;
        noMoreChecks = true;
      }
      i++;
    }

    return -1;
  }

  void CmdParser::getParameterValue(const std::string &execParamToCheck, bool &parameterValue)
  {
    if (getDataTypeAsEnumCode(execParamToCheck) != cbica::Parameter::BOOLEAN)
    {
      std::cerr << "The data type of the requested parameter, '" << execParamToCheck << "' is classified as '" << getDataTypeAsString(execParamToCheck) <<
        "' and cannot be returned as a BOOL.\n";
      exit(EXIT_FAILURE);
    }
    int position;
    if (compareParameter(execParamToCheck, position))
    {
      std::string rawValue = m_argv[position + 1];
      if ((rawValue == "1") || (rawValue == "true") || (rawValue == "True") || (rawValue == "TRUE") ||
        (rawValue == "yes") || (rawValue == "Yes") || (rawValue == "YES") ||
        (rawValue == "")) // if the parameter is just passed as a flag, assume that the user wants it enabled
      {
        parameterValue = true; // return value is a bool
        return;
      }
      else
      {
        parameterValue = false; // return value is a bool
        return;
      }
    }
  }

  void CmdParser::getParameterValue(const std::string &execParamToCheck, int &parameterValue)
  {
    if (getDataTypeAsEnumCode(execParamToCheck) != cbica::Parameter::INTEGER)
    {
      std::cerr << "The data type of the requested parameter, '" << execParamToCheck << "' is classified as '" << getDataTypeAsString(execParamToCheck) <<
        "' and cannot be returned as a INTEGER.\n";
      exit(EXIT_FAILURE);
    }
    int position;
    if (compareParameter(execParamToCheck, position))
    {
      parameterValue = std::atoi(m_argv[position + 1]); // return value is an integer
      return;
    }
    else
    {
      parameterValue = -1;
      return;
    }
  }

  void CmdParser::getParameterValue(const std::string &execParamToCheck, float &parameterValue)
  {
    if (getDataTypeAsEnumCode(execParamToCheck) != cbica::Parameter::FLOAT)
    {
      std::cerr << "The data type of the requested parameter, '" << execParamToCheck << "' is classified as '" << getDataTypeAsString(execParamToCheck) <<
        "' and cannot be returned as a FLOAT.\n";
      exit(EXIT_FAILURE);
    }
    int position;
    if (compareParameter(execParamToCheck, position))
    {
      parameterValue = static_cast<float>(std::atof(m_argv[position + 1])); // return value is a float
      return;
    }
    else
    {
      parameterValue = -1;
      return;
    }
  }

  void CmdParser::getParameterValue(const std::string &execParamToCheck, std::string &parameterValue)
  {
    int returnCode = getDataTypeAsEnumCode(execParamToCheck);
    if ((returnCode != cbica::Parameter::STRING))
    {
      if (!((returnCode == cbica::Parameter::NONE) || // check if type is NONE or FILE or DIR, if yes then it is not an error
        (returnCode == cbica::Parameter::FILE) ||
        (returnCode == cbica::Parameter::DIRECTORY)))
      {
        std::cerr << "The data type of the requested parameter, '" << execParamToCheck << "' is classified as '" << getDataTypeAsString(execParamToCheck) <<
          "' and cannot be returned as a STRING.\n";
        exit(EXIT_FAILURE);
      }
    }
    int position;
    if (compareParameter(execParamToCheck, position))
    {
      parameterValue = m_argv[position + 1]; // return value is a string
      return;
    }
    else
    {
      parameterValue = "";
      return;
    }
  }

  void CmdParser::exampleUsage(const std::string &usageOfExe)
  {
    m_exampleOfUsage = usageOfExe;
    m_exampleOfUsage = cbica::replaceString(m_exampleOfUsage, m_exeName + ".exe", "");
    m_exampleOfUsage = cbica::replaceString(m_exampleOfUsage, "./" + m_exeName, "");
  }

  void CmdParser::writeConfigFile(const std::string &dirName)
  {
    if (!checkMaxLen)
    {
      getMaxLength();
    }

    std::string dirName_wrap;
    if (!cbica::directoryExists(dirName) || (dirName == ""))
    {
      dirName_wrap = cbica::makeTempDir();
    }
    dirName_wrap = cbica::replaceString(dirName, "\\", "/");
    if (dirName_wrap.substr(dirName_wrap.length() - 1) != "/")
    {
      dirName_wrap += "/";
    }

    std::string fileName = dirName_wrap + m_exeName + ".txt";

#if (_WIN32)
    if (_access(fileName.c_str(), 6) == -1)
    {
      std::cerr << "No write permission for the specified config file.\n";
      exit(EXIT_FAILURE);
    }
#else
    if (access(fileName.c_str(), R_OK && W_OK) != 0)
    {
      std::cerr << "No write permission for the specified config file.\n";
      exit(EXIT_FAILURE);
    }
#endif

    std::ofstream file;
    file.open(fileName.c_str());

    if (file.is_open())
    {
      for (size_t i = 0; i < m_requiredParameters.size(); i++)
      {
        file << getSeparator(Param) << m_requiredParameters[i].verbose << getSeparator(Param) <<
          " " << getSeparator(DataType) << m_requiredParameters[i].dataType_string << getSeparator(DataType) <<
          " " << getSeparator(DataRange) << m_requiredParameters[i].dataRange << getSeparator(DataRange) <<
          " " << m_requiredParameters[i].descriptionLine1 + " " + m_requiredParameters[i].descriptionLine2 + " " +
          m_requiredParameters[i].descriptionLine3 + " " + m_requiredParameters[i].descriptionLine4 + " " +
          m_requiredParameters[i].descriptionLine5 << "\n";
      }

      for (size_t i = 0; i < m_optionalParameters.size(); i++)
      {
        file << getSeparator(Param) << m_optionalParameters[i].verbose << getSeparator(Param) <<
          " " << getSeparator(DataType) << m_optionalParameters[i].dataType_string << getSeparator(DataType) <<
          " " << getSeparator(DataRange) << m_optionalParameters[i].dataRange << getSeparator(DataRange) <<
          " " << m_optionalParameters[i].descriptionLine1 + " " + m_optionalParameters[i].descriptionLine2 + " " +
          m_optionalParameters[i].descriptionLine3 + " " + m_optionalParameters[i].descriptionLine4 + " " +
          m_optionalParameters[i].descriptionLine5 << "\n";
      }
    }

    file.close();

    //std::cout << "Config file written with path: '" << fileName << "'\n";
    return;
  }

}