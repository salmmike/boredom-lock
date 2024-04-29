
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

set(
    LIB_HEADERS
    ${CMAKE_SOURCE_DIR}/src/include/scheduler.h;
    ${CMAKE_SOURCE_DIR}/src/include/tools.h;
)

set(
    LIB_SOURCES
    ${CMAKE_SOURCE_DIR}/src/tools.cpp;
    ${CMAKE_SOURCE_DIR}/src/scheduler.cpp;
)

add_library(
    boredomlock
    SHARED
    ${LIB_SOURCES}
    ${LIB_HEADERS}
)
set_target_properties(boredomlock PROPERTIES PUBLIC_HEADER "${LIB_HEADERS}")

target_link_libraries(boredomlock
                      PRIVATE
                      simpleini
                      usb
                      udev
)

target_include_directories(boredomlock PUBLIC ${CMAKE_SOURCE_DIR}/src/include)
option(INSTALL_GTEST OFF)

install(TARGETS boredomlock
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
