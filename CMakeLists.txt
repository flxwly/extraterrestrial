#sets up the minimum version of cmake
cmake_minimum_required(VERSION 3.9) # personally I'd set this version as low as required; you don't have to require the cutting edge version

#how the project will be called
project (Extraterrestrial)

#set the C++ standard to be used
set (CMAKE_CXX_STANDARD 20)

#set source files
set (SOURCE_FILES
        code/main.cpp
        code/MapData.cpp
        code/libs/ColorRecognition.cpp
        code/libs/CoSpaceFunctions.cpp
        code/libs/PVector.cpp
        code/Robot.cpp code/Robot.hpp code/Pathfinding.cpp code/Pathfinding.hpp)

add_executable(Extraterrestrial ${SOURCE_FILES})

set(SFML_DIR ENV{SFML32_HOME})
find_package(SFML REQUIRED system window graphics network audio)
target_link_libraries(Extraterrestrial sfml-graphics sfml-audio)