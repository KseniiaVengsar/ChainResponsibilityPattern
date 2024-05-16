cmake_minimum_required(VERSION 3.22.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
project(MyProject)

add_executable(MyProjectEXE 
    ./ChainResponsibilityPattern/Source.cpp 
)