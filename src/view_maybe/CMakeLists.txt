
include(GNUInstallDirs)

add_library(view_maybe STATIC "")

target_sources(view_maybe
  PRIVATE
  view_maybe.cpp)

target_include_directories(view_maybe PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_LOWER_PROJECT_NAME}>  # <prefix>/include/fringetree
  )

target_link_libraries(view_maybe stl2)

## Tests
add_executable(view_maybe_test "")

target_sources(
  view_maybe_test
  PRIVATE
  view_maybe.t.cpp)

target_link_libraries(view_maybe_test view_maybe)
target_link_libraries(view_maybe_test gtest)
target_link_libraries(view_maybe_test gtest_main)

include(GoogleTest)
gtest_discover_tests(view_maybe_test)
