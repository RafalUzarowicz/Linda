cmake_minimum_required(VERSION 3.17)

project(LINDA)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -pedantic)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.10.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories(headers)

file(GLOB SOURCES CONFIGURE_DEPENDS "sources/*.cpp")

add_executable(LINDA_MAIN)
target_sources(LINDA_MAIN PRIVATE main.cpp ${SOURCES})

add_library(Linda STATIC ${SOURCES})

add_executable(RAFAL)
target_sources(RAFAL PRIVATE tuple_pattern_test.cpp)
target_link_libraries(RAFAL Linda)

add_executable(LINDA_TEST)
target_sources(LINDA_TEST PRIVATE main_linda_test.cpp ${SOURCES})

add_executable(SIGNAL_TEST)
target_sources(SIGNAL_TEST PRIVATE signal_handling_test.cpp ${SOURCES})

add_executable(ALL_TEST)
target_sources(ALL_TEST PRIVATE test_everything.cpp ${SOURCES})

add_executable(GOOGLE_TEST)
target_sources(GOOGLE_TEST PRIVATE tests/test1.cpp tests/test2.cpp tests/test3.cpp ${SOURCES})
target_link_libraries(GOOGLE_TEST gtest_main)

include(GoogleTest)
gtest_discover_tests(GOOGLE_TEST)