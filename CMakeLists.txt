
cmake_minimum_required(VERSION 3.20)

project(nescape
    VERSION 0.5
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dependencies
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

# Source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Executable
add_executable(nescape
    ${SOURCES}
    extern/ini.c
)

# Include directories
target_include_directories(nescape PRIVATE
    include
    src
    extern
)

# Libraries
target_link_libraries(nescape PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_image::SDL2_image
)

