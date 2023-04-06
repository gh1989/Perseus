cmake_minimum_required(VERSION 3.0)

project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

# Define build types
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Create the directories for the build configurations
make_directory(Debug)
make_directory(Release)

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
    tests/test_bitboard.cpp
)

# Create the executable targets for the debug and release builds
add_executable(ChessEngine_Debug ${SOURCES})
set_target_properties(ChessEngine_Debug PROPERTIES OUTPUT_NAME "Debug/ChessEngine")

add_executable(ChessEngine_Release ${SOURCES})
set_target_properties(ChessEngine_Release PROPERTIES OUTPUT_NAME "Release/ChessEngine")

# Link against any necessary libraries for the tests
# (in this example, we assume that the main ChessEngine library
# is already included in the SOURCES list)
set(TEST_SOURCES
    tests/test_bitboard.cpp
)

# Create the test executable target
add_executable(TestChessEngine ${TEST_SOURCES})

# Link against the main ChessEngine library and any other necessary libraries
target_link_libraries(TestChessEngine ChessEngine)

# Add the test target to the "test" CMake target
add_test(NAME MyTest COMMAND TestChessEngine)
