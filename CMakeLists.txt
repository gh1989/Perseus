cmake_minimum_required(VERSION 3.0)

project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

set(SOURCE_FILES
  bitboard.h
  geometry.h
  move.h
  square.h
  state.h
  move_generation.h
  random.h
  string_transforms.h
  test.h
  types.h
  geometry.cpp
  move.cpp
  square.cpp
  state.cpp
  test.cpp
  move_generation.cpp
  string_transforms.cpp
  main.cpp)

add_executable(ChessEngine ${SOURCE_FILES})