
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

add_executable(boredom-lock
    main.cpp
    tools.cpp
    scheduler.cpp
)

target_link_libraries(boredom-lock
                      PRIVATE
                      simpleini
                      usb
)

include(GNUInstallDirs ./include)
install(TARGETS boredom-lock
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
