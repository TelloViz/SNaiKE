cmake_minimum_required(VERSION 3.16)
project(main LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set a single output directory for all configs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# SFML Configuration
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

# Create executable
add_executable(main 
    src/main.cpp
    src/Game.cpp
    src/Snake.cpp
    src/GameStates.cpp)

# Configure include directories
target_include_directories(main 
    PRIVATE 
        include
        ${SFML_SOURCE_DIR}/include)

# Copy resources
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Link libraries
target_link_libraries(main PRIVATE sfml-graphics)

# Copy resources during build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources"
    COMMENT "Copying resources to bin/resources"
)
