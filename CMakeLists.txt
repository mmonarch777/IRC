cmake_minimum_required(VERSION 3.21)
project(kamil_irk)

file(GLOB
        FC "src/*.cpp")

set(CMAKE_CXX_STANDARD 98)

add_executable(kamil_irk ${FC})
