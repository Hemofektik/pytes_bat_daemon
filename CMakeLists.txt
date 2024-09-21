cmake_minimum_required(VERSION 3.9)

project (pytes)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/bin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23 -Wconversion -Wall -Wextra -Wpedantic -Wno-deprecated-copy -fno-omit-frame-pointer")

find_package(cpprestsdk REQUIRED)
find_package(CppLinuxSerial REQUIRED)
find_package(OpenSSL REQUIRED)

include(FetchContent)

FetchContent_Declare(
    Backward
    GIT_REPOSITORY https://github.com/bombela/backward-cpp
    GIT_TAG master
)
FetchContent_MakeAvailable(Backward)

include_directories("include")

add_subdirectory(src)

if(CMAKE_TESTING_ENABLED)
    add_subdirectory(test)
endif()