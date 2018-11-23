
include(GNUInstallDirs)

add_library(view_maybe STATIC "")

target_sources(view_maybe
  PRIVATE
  view_maybe.cpp)

target_include_directories(view_maybe PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_LOWER_PROJECT_NAME}>  # <prefix>/include/fringetree
  )

target_link_libraries(view_maybe stl2)

add_executable(main "")

target_sources(
  main
  PRIVATE
  main.cpp)

target_link_libraries(main stl2)

install(
  TARGETS main
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

add_executable(concepts "")

target_sources(
  concepts
  PRIVATE
  concepts.cpp)

target_link_libraries(concepts stl2)

install(
  TARGETS concepts
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
