#sets up the minimum version of cmake
cmake_minimum_required(VERSION 3.9) # personally I'd set this version as low as required; you don't have to require the cutting edge version

#how the project will be called
project(Extraterrestrial)

#set the C++ standard to be used
set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(RUNNING_WITHOUT_COSPACE)

#set source files
set(SOURCE_FILES
        code/main.cpp
        code/MapData.cpp
        code/Robot.cpp

        code/libs/ColorRecognition.cpp
        code/libs/ConsolePainter.cpp
        code/libs/CoSpaceFunctions.cpp
        code/libs/PVector.cpp)

add_executable(Extraterrestrial ${SOURCE_FILES})