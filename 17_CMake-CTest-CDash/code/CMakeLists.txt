CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET( PROJECT_NAME UnitTesting_Tutorial )

# Set project name 
PROJECT( ${PROJECT_NAME} )
SET(EXE_NAME "${PROJECT_NAME}" CACHE STRING "Executable name" FORCE)
 
#Find libraries
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

SET_PROPERTY( GLOBAL PROPERTY USE_FOLDERS ON )

# add c++11 flag if it is GCC v<5  
IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)  
  IF(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)  
    INCLUDE( CheckCXXCompilerFlag )  
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)  
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)  
    IF( COMPILER_SUPPORTS_CXX11 )  
    	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")  
    ELSEIF(COMPILER_SUPPORTS_CXX0X )  
    	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")  
    ELSE()  
    	MESSAGE(ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")  
    ENDIF()  
  ENDIF()  
ENDIF()   

# Add sources to executable
ADD_EXECUTABLE(
  ${EXE_NAME} 
  ${PROJECT_SOURCE_DIR}/src/BasicApp.h
  ${PROJECT_SOURCE_DIR}/src/cbicaITKSafeImageIO.h
  ${PROJECT_SOURCE_DIR}/src/cbicaUtilities.h
  ${PROJECT_SOURCE_DIR}/src/cbicaUtilities.cpp
  ${PROJECT_SOURCE_DIR}/src/cbicaCmdParser.h
  ${PROJECT_SOURCE_DIR}/src/cbicaCmdParser.cpp
  ${PROJECT_SOURCE_DIR}/src/main.cxx
)

# Link the libraries to be used
TARGET_LINK_LIBRARIES(
  ${EXE_NAME}
  ${ITK_LIBRARIES}
)

ENABLE_TESTING()
INCLUDE(CTest)
ADD_SUBDIRECTORY(testing)