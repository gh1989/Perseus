cmake_minimum_required(VERSION 3.0)

project(ChessNetwork)
set(CMAKE_CXX_STANDARD 23)
# Find the PyTorch package
find_package(Torch REQUIRED)

# Add the source files to your project
set(SOURCES
    src/bitboard.h
    src/geometry.h
    src/move.h
    src/square.h
    src/state.h
    src/move_generation.h
    src/random.h
    src/network.h
    src/string_transforms.h
    src/types.h
    src/geometry.cpp
    src/network.cpp
    src/move.cpp
    src/square.cpp
    src/state.cpp
    src/move_generation.cpp
    src/string_transforms.cpp
)

# Create the shared library target
add_library(ChessNetwork SHARED ${SOURCES})

# Add Catch2
find_package(Catch2 REQUIRED)
include_directories(${CATCH2_INCLUDE_DIRS} ${TORCH_INCLUDE_DIRS})

# Add tests executable
set(TEST_SOURCES
    src/string_transforms.h
    src/string_transforms.cpp
    tests/test_all.cpp
)

add_executable(tests ${SOURCES} ${TEST_SOURCES})
target_link_libraries(tests Catch2::Catch2 ${TORCH_LIBRARIES})

# Add a custom test target
include(Catch)
catch_discover_tests(tests)