# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Hangman")

# Include sub-projects.
include_directories(HangmanServer)
file(GLOB SOURCES "HangmanServer/*.cpp")

add_executable(server ${SOURCES})
target_compile_options(server PRIVATE -Wall -Wextra)