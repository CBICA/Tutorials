CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# Assign a separate variable for project name
SET( PROJECT_NAME CMake_Tutorial )

# Change this if you want different executable name
SET( EXE_NAME ${PROJECT_NAME} )

# Add definitions to use inside source code
ADD_DEFINITIONS( -DPROJECT_NAME="${PROJECT_NAME}" )

# Set project name 
PROJECT( ${PROJECT_NAME} 
VERSION 1.0.0
)

# Add all include directories 
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src )

ADD_EXECUTABLE( 
  ${EXE_NAME}  # executable or library name here
  ${PROJECT_SOURCE_DIR}/src/main.cxx  # the file which contains the entry to the program
)
