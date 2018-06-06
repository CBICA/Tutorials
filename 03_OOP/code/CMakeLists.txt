CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Assign a separate variable for project name
SET( PROJECT_NAME OOP_Tutorial )

# Change this if you want different executable name
SET( EXE_NAME ${PROJECT_NAME} )

# Add definitions to use inside source code
ADD_DEFINITIONS( -DPROJECT_NAME="${PROJECT_NAME}" )

# Set project name 
PROJECT( ${PROJECT_NAME} )

# Set source code directory
SET( SOURCE_DIRECTORY ${PROJECT_SOURCE_DIR}/src )
SET( CLASS_DIRECTORY ${SOURCE_DIRECTORY}/classes )

# Add all include directories 
INCLUDE_DIRECTORIES( 
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${SOURCE_DIRECTORY} 
  ${CLASS_DIRECTORY} 
)

# Add sources to executable
ADD_EXECUTABLE( ${EXE_NAME} ${SOURCE_DIRECTORY}/main.cxx)
