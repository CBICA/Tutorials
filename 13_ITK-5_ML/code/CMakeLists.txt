CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET( PROJECT_NAME ITK_Tutorial_ML )

# Set project name 
PROJECT( ${PROJECT_NAME} )
 
#Find libraries
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

SET( CMAKE_CXX_STANDARD 11 )

FIND_PACKAGE(OpenCV 3.0 REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

# Add sources to executable
ADD_EXECUTABLE(
  ${PROJECT_NAME} 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cxx
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbicaITKSafeImageIO.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbicaUtilities.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbicaUtilities.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbicaCmdParser.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbicaCmdParser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/TestITK.h # example on how to write a templated class
  ${CMAKE_CURRENT_SOURCE_DIR}/src/TestITK.hxx # example on how to write a templated class
)

# Link the libraries to be used
TARGET_LINK_LIBRARIES(
  ${PROJECT_NAME}
  ${ITK_LIBRARIES}
  ${OpenCV_LIBRARIES}
)