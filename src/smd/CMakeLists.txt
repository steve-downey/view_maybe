add_subdirectory(views)

add_library(maybe STATIC "")

target_sources(
  maybe
  PRIVATE
  maybe.cpp)

include(GNUInstallDirs)

target_include_directories(maybe PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_LOWER_PROJECT_NAME}>  # <prefix>/include/smd
  )

install(
  TARGETS maybe
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

string(TOLOWER ${CMAKE_PROJECT_NAME} CMAKE_LOWER_PROJECT_NAME)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_LOWER_PROJECT_NAME}
  FILES_MATCHING PATTERN "*.h"
  )

target_link_libraries(maybe)

## Tests
add_executable(maybe_test "")

target_sources(
  maybe_test
  PRIVATE
  maybe.t.cpp)

target_link_libraries(maybe_test view_maybe)
target_link_libraries(maybe_test gtest)
target_link_libraries(maybe_test gtest_main)

include(GoogleTest)
gtest_discover_tests(maybe_test)
