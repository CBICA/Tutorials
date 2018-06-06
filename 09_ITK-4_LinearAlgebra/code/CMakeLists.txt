CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET( PROJECT_NAME ITK_LinearAlgebra_Tutorial )

# Set project name 
PROJECT( ${PROJECT_NAME} )
 
#Find libraries
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

# Add sources to executable
ADD_EXECUTABLE(
  ${PROJECT_NAME} 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cxx
)

# Link the libraries to be used
TARGET_LINK_LIBRARIES(
  ${PROJECT_NAME}
  ${ITK_LIBRARIES}
)