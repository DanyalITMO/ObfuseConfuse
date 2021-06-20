cmake_minimum_required(VERSION 3.17)
project(testDisasm)

set(CMAKE_CXX_STANDARD 17)

option(ZYDIS_BUILD_TOOLS "" OFF)
option(ZYDIS_BUILD_EXAMPLES "" OFF)
add_subdirectory("deps/zydis")

add_executable(testDisasm main.cpp Obfuscators/Text/AbstractTextObfuscator.cpp Obfuscators/Text/AbstractTextObfuscator.h Obfuscators/Text/Jumper.cpp Obfuscators/Text/Jumper.h Obfuscators/Text/Shuffler.cpp Obfuscators/Text/Shuffler.h Obfuscators/Text/Junk.cpp Obfuscators/Text/Junk.h Obfuscators/Bin/AbstractBinObfuscator.cpp Obfuscators/Bin/AbstractBinObfuscator.h Obfuscators/Bin/TwoXor.cpp Obfuscators/Bin/TwoXor.h utils.cpp utils.h)
target_link_libraries(${PROJECT_NAME} PRIVATE "Zydis")