cmake_minimum_required(VERSION 3.14)
project(Discrete_Event_Simulation)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Discrete_Event_Simulation
        file.cpp
        file.h
        globals.cpp
        globals.h
        main.cpp cpu.cpp cpu.h disk.cpp disk.h)
