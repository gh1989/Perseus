cmake_minimum_required(VERSION 3.0)
project(ChessNetwork)
set(CMAKE_CXX_STANDARD 23)

# Include directories for your headers
include_directories(src)

# Add the source files to your project
set(SOURCES
    src/bitboard.h
    src/geometry.h
    src/move.h
    src/square.h
    src/state.h
    src/move_generation.h
    src/random.h
    src/string_transforms.h
    src/types.h
    src/geometry.cpp
    src/move.cpp
    src/square.cpp
    src/state.cpp
    src/move_generation.cpp
    src/string_transforms.cpp
)

add_executable(perseus src/main.cpp ${SOURCES})
set_property(TARGET perseus PROPERTY CXX_STANDARD 23)