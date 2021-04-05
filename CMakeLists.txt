#sets up the minimum version of cmake
cmake_minimum_required(VERSION 3.9) # personally I'd set this version as low as required; you don't have to require the cutting edge version

#how the project will be called
project (CsBot-Extraterrestrial)

#set the C++ standard to be used
set (CMAKE_CXX_STANDARD 20)

#set source files
set (SOURCE_FILES code/main.cpp
        code/MapData.cpp
        code/Pathfinder.cpp
        code/Robot.cpp
        code/SFMLWindow.cpp
        code/libs/ColorRecognition.cpp
        code/libs/CommonFunctions.cpp
        code/libs/CoSpaceFunctions.cpp code/libs/PVector.cpp code/libs/PVector.hpp)

add_executable(CsBot ${SOURCE_FILES})

set(SFML_DIR "C:/MinGW/SFML-2.5.1/lib/cmake/SFML")
find_package(SFML REQUIRED system window graphics network audio)
target_link_libraries(CsBot sfml-graphics sfml-audio)