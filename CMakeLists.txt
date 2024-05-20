cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME "LogReader")
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Qt
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
)

# third party libs
set(spdlog_DIR "${CMAKE_SOURCE_DIR}/lib/spdlog/lib/cmake/spdlog")
find_package(spdlog REQUIRED)

file(GLOB_RECURSE ${PROJECT_NAME}_SRC "src/*.cpp" "src/*.cxx" "src/*.c")
file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS "include/*.h" "include/*.hpp")

add_executable(${PROJECT_NAME} main.cpp
    ${${PROJECT_NAME}_SRC}
    ${${PROJECT_NAME}_HEADERS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    spdlog::spdlog
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/spdlog/include
    ${CMAKE_SOURCE_DIR}/include
)
