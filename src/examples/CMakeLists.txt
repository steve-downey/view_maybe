
include(GNUInstallDirs)

add_executable(main "")

target_sources(
  main
  PRIVATE
  main.cpp)

target_link_libraries(main view_maybe)

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

target_link_libraries(concepts)

install(
  TARGETS concepts
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

add_executable(allview "")

target_sources(
  allview
  PRIVATE
  all.cpp)

target_link_libraries(allview)

install(
  TARGETS allview
  EXPORT ${TARGETS_EXPORT_NAME}
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
