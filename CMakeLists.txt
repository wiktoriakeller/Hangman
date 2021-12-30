# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Hangman")

find_package(Threads)
link_libraries(${CMAKE_THREAD_LIBS_INIT})

# Include sub-projects.
add_subdirectory ("HangmanServer")
