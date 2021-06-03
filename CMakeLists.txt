cmake_minimum_required(VERSION 3.17)

project(LINDA)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -pedantic)

include_directories(headers sources)

file(GLOB HEADERS CONFIGURE_DEPENDS "headers/*.h")
file(GLOB SOURCES CONFIGURE_DEPENDS "sources/*.cpp")

message("${HEADERS}")
message("${SOURCES}")

add_executable(LINDA)

target_sources(LINDA PRIVATE main.cpp ${SOURCES})
target_include_directories(LINDA PRIVATE ${HEADERS})