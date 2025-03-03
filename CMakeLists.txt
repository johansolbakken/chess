cmake_minimum_required(VERSION 3.10)

project(chess)

set(CMAKE_CXX_STANDARD 23)

add_executable(chess main.cpp fen.cpp engine.cpp)
