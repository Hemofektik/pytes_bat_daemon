
Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)

FetchContent_MakeAvailable(Catch2)

add_executable(test_parse test_parse.cpp)
target_link_libraries(test_parse PRIVATE Catch2::Catch2WithMain pytes)

include(Catch)
catch_discover_tests(test_parse)