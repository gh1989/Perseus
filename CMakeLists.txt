cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0057 NEW)
project(ChessNetwork)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

# Find the PyTorch package
find_package(Torch REQUIRED)
message(STATUS "Found PyTorch: ${TORCH_LIBRARIES}")

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
    src/move.cpp
    src/square.cpp
    src/state.cpp
    src/move_generation.cpp
    src/string_transforms.cpp
)

#set(LIBKINETO_PATH "/Users/gregg/Code/Perseus/kineto/libkineto/build/libkineto.a") 

add_executable(perseus src/main.cpp ${SOURCES})
target_link_libraries(perseus PRIVATE ${TORCH_LIBRARIES}) # ${LIBKINETO_PATH})

# Add Catch2
find_package(Catch2 REQUIRED)

# Separate source files for tests
set(TEST_SOURCES
    tests/test_all.cpp
    # Add other test source files here
)

add_executable(tests ${TEST_SOURCES} ${SOURCES})
target_link_libraries(tests PRIVATE Catch2::Catch2 ${TORCH_LIBRARIES}) # ${LIBKINETO_PATH})

# Add a custom test target
include(Catch)
catch_discover_tests(tests)
