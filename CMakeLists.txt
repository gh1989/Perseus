cmake_minimum_required(VERSION 3.0)

project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

# Define build types
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

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
    src/test.h
    src/types.h
    src/geometry.cpp
    src/move.cpp
    src/square.cpp
    src/state.cpp
    src/test.cpp
    src/move_generation.cpp
    src/string_transforms.cpp
    src/main.cpp
)

# Create the executable targets
add_executable(ChessEngine ${SOURCES})

# Add the include directories
target_include_directories(ChessEngine PUBLIC ${PROJECT_SOURCE_DIR}/src)

# Add Catch2
find_package(Catch2 REQUIRED)
include_directories(${CATCH2_INCLUDE_DIRS})

# Add tests executable
set(TEST_SOURCES
    tests/test_bitboard.cpp
)

add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests Catch2::Catch2)

# Add a custom test target
include(Catch)
catch_discover_tests(tests)
