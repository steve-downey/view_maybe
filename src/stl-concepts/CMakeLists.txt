add_library(stl-concepts STATIC "")

target_sources(
  stl-concepts
  PRIVATE
  stl-concepts.cpp)

include(GNUInstallDirs)

target_include_directories(stl-concepts PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_LOWER_PROJECT_NAME}>  # <prefix>/include/stl-concepts
  )

install(
  TARGETS stl-concepts
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

string(TOLOWER ${CMAKE_PROJECT_NAME} CMAKE_LOWER_PROJECT_NAME)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_LOWER_PROJECT_NAME}
  FILES_MATCHING PATTERN "*.h"
  )


## Tests
add_executable(stl-concepts_test "")

target_sources(
  stl-concepts_test
  PRIVATE
  stl-concepts.t.cpp)

target_link_libraries(stl-concepts_test stl-concepts)
target_link_libraries(stl-concepts_test gtest)
target_link_libraries(stl-concepts_test gtest_main)

include(GoogleTest)
gtest_discover_tests(stl-concepts_test)
