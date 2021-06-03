cmake_minimum_required(VERSION 3.17)
project(testDisasm)

set(CMAKE_CXX_STANDARD 17)

option(ZYDIS_BUILD_TOOLS "" OFF)
option(ZYDIS_BUILD_EXAMPLES "" OFF)
add_subdirectory("deps/zydis")

add_executable(testDisasm main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE "Zydis")