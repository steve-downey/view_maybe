
include(GNUInstallDirs)

add_executable(main "")

target_sources(
  main
  PRIVATE
  main.cpp)

target_link_libraries(main view_maybe stl2)

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
