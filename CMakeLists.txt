cmake_minimum_required(VERSION 3.14.5)
project(extraterrestrial)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,10000000")

add_executable(extraterrestrialCPP
        code/libs/CommonFunctions.cpp
        code/CoSpaceFunctions.cpp
        code/main.cpp
        code/ColorRecognition.cpp
        code/Pathfinder.cpp
        code/MapData.cpp
        code/libs/DebugTools.cpp
        code/Robot.cpp
        code/main.cpp code/libs/DebugTools.hpp)