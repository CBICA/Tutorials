## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
##
## # The following are required to submit to the CDash dashboard:
##   ENABLE_TESTING()
##   INCLUDE(CTest)

set(CTEST_PROJECT_NAME "${project_name_from_CDash}")
set(CTEST_NIGHTLY_START_TIME "01:00:00 UTC")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "cbica-infr-vweb")
set(CTEST_DROP_LOCATION "/cdash/submit.php?project=${project_name_from_CDash}")
set(CTEST_DROP_SITE_CDASH TRUE)
