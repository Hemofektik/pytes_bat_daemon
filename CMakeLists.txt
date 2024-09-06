cmake_minimum_required(VERSION 3.9)

project (pytes)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/bin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23 -Wconversion -Wall -Wextra -Wpedantic -Wno-deprecated-copy -fno-omit-frame-pointer")

find_package(cpprestsdk REQUIRED)
find_package(CppLinuxSerial REQUIRED)

add_subdirectory(src)
#add_subdirectory(test)