cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0057 NEW)
project(ChessNetwork)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

# Find Python
find_package(Python REQUIRED COMPONENTS Interpreter Development)
message("Python executable: ${PYTHON_EXECUTABLE}")
 
# Find the PyTorch package
find_package(Torch REQUIRED)

# Find the pybind11 package
find_package(pybind11 REQUIRED)

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

# Create the Python module
pybind11_add_module(ChessNetwork MODULE ${SOURCES})

# Set the path to Python include directory
set(PYTHON_INCLUDE_DIRS "/opt/homebrew/Cellar/python@3.11/3.11.4_1/Frameworks/Python.framework/Versions/3.11/include/python3.11")

# Add the Python include directory to the include directories
include_directories(${PYTHON_INCLUDE_DIRS})

# Add the necessary include directories
#execute_process(
#    COMMAND python3 -c "from distutils.sysconfig import get_python_inc; print(get_python_inc())"
#    OUTPUT_VARIABLE PYTHON_INCLUDE_DIRS
#    OUTPUT_STRIP_TRAILING_WHITESPACE
#)

target_include_directories(ChessNetwork PRIVATE ${PYTHON_INCLUDE_DIRS} ${TORCH_INCLUDE_DIRS})
message("PYTHON_INCLUDE_DIRS: ${PYTHON_INCLUDE_DIRS}")
message("TORCH_INCLUDE_DIRS: ${TORCH_INCLUDE_DIRS}")

# Link the required libraries
target_link_libraries(ChessNetwork PRIVATE ${TORCH_LIBRARIES})

# Add Catch2
find_package(Catch2 REQUIRED)
include_directories(${CATCH2_INCLUDE_DIRS})

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
