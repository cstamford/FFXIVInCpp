cmake_minimum_required(VERSION 3.13)
project(FFXIVInCpp VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(src)

add_subdirectory(ext/minhook)
link_libraries(minhook)

add_subdirectory(src/Project)
add_subdirectory(src/LandingPad)
add_subdirectory(src/Injector)
add_subdirectory(src/Offsets)

