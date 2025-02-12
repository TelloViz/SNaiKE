cmake_minimum_required(VERSION 3.16)
project(main LANGUAGES CXX)

# Set C++20 as the standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_20)
target_link_libraries(main PRIVATE sfml-graphics)
