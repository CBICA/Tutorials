SET( TEST_EXE_NAME Test_BasicApp )

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/src # where all the include files are present
	${CMAKE_CURRENT_SOURCE_DIR}
)

#Find libraries
FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( 
  ${TEST_EXE_NAME}
  testExe.cxx 
  ${PROJECT_SOURCE_DIR}/src/BasicApp.h
  ${PROJECT_SOURCE_DIR}/src/cbicaITKWriteImage.h
  ${PROJECT_SOURCE_DIR}/src/cbicaUtilities.h
  ${PROJECT_SOURCE_DIR}/src/cbicaUtilities.cpp
  ${PROJECT_SOURCE_DIR}/src/cbicaCmdParser.h
  ${PROJECT_SOURCE_DIR}/src/cbicaCmdParser.cpp
)

# Link the libraries to be used
TARGET_LINK_LIBRARIES(
  ${TEST_EXE_NAME}
  ${ITK_LIBRARIES}
)

SET( DATA_DIR ${PROJECT_SOURCE_DIR}/data )

# Test that usage prints properly
ADD_TEST( NAME Project_Test COMMAND ${TEST_EXE_NAME} -runTest ${DATA_DIR}/testImage.nii.gz)
