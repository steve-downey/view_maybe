add_library(view_maybe STATIC "")

target_sources(
  view_maybe
  PRIVATE
  concepts.cpp
  movable_box.cpp
  nullable.cpp)

include(GNUInstallDirs)

target_include_directories(view_maybe PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_LOWER_PROJECT_NAME}>  # <prefix>/include/scratch
  )

install(
  TARGETS view_maybe
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

string(TOLOWER ${CMAKE_PROJECT_NAME} CMAKE_LOWER_PROJECT_NAME)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_LOWER_PROJECT_NAME}
  FILES_MATCHING PATTERN "*.h"
  )

target_link_libraries(view_maybe)

## Tests
add_executable(view_maybe_test "")

target_sources(
  view_maybe_test
  PRIVATE
  concepts.t.cpp
  movable_box.t.cpp
  nullable.t.cpp)

target_link_libraries(view_maybe_test view_maybe)
target_link_libraries(view_maybe_test gtest)
target_link_libraries(view_maybe_test gtest_main)

include(GoogleTest)
gtest_discover_tests(view_maybe_test)
