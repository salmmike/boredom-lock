include(FetchContent)


FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)


add_executable(
    test_scheduler
    test.cpp
    ${CMAKE_SOURCE_DIR}/src/tools.cpp
    ${CMAKE_SOURCE_DIR}/src/scheduler.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/src/include/)

target_link_libraries(
    test_scheduler
    PRIVATE
    GTest::GTest
    simpleini
    usb
)


add_test(test_scheduler test_scheduler)
