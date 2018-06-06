CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET( PROJECT_NAME DICOM_Conversion )

# Set project name 
PROJECT( ${PROJECT_NAME} )
 
#Find libraries
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

SET( CommonSources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbicaUtilities.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cbicaUtilities.cpp
)

# Add sources to executable
ADD_EXECUTABLE(
  ${PROJECT_NAME}_DWI 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/dwiConversion.cxx
  ${CommonSources}
)

# Link the libraries to be used
TARGET_LINK_LIBRARIES(
  ${PROJECT_NAME}_DWI
  ${ITK_LIBRARIES}
)

# Add sources to executable
ADD_EXECUTABLE(
  ${PROJECT_NAME}_Simple 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/simpleConversion.cxx
  ${CommonSources}
)

# Link the libraries to be used
TARGET_LINK_LIBRARIES(
  ${PROJECT_NAME}_Simple
  ${ITK_LIBRARIES}
)