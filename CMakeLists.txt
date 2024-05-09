cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME "LogReader")
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
)

file(GLOB_RECURSE ${PROJECT_NAME}_SRC "src/*.cpp" "src/*.cxx" "src/*.c")

add_executable(${PROJECT_NAME} main.cpp
    ${${PROJECT_NAME}_SRC}
    ${${PROJECT_NAME}_HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE "include")

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core)
