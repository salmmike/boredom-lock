cmake_minimum_required(VERSION 3.16)

project(boredom-lock VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

set (CMAKE_FLAGS "-DINSTALL_GTEST=OFF")

#enable_testing()

add_subdirectory(src)
add_subdirectory(simpleini)
add_subdirectory(test)
