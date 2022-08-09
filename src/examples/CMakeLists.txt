
include(GNUInstallDirs)

add_executable(main "")

target_sources(
  main
  PRIVATE
  main.cpp)

target_link_libraries(main view_maybe)

add_executable(before_after "")

target_sources(
  before_after
  PRIVATE
  before_after.cpp)

target_link_libraries(before_after view_maybe)

install(
  TARGETS main
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
