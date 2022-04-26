cmake_minimum_required(VERSION 3.14)
project(hackaton3 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-O2")
include_directories(/usr/local/include)
add_executable(${PROJECT_NAME}
	src/Job_System.cpp
	src/Graph.cpp
	src/Composer.cpp
	src/Layout.cpp
	src/main.cpp
)
