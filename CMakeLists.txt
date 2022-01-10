cmake_minimum_required(VERSION 3.21)
project(MCSeeds)

find_package(OpenMP)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(MCSeeds ${SRC_FILES})

if (OpenMP_CXX_FOUND)
    target_link_libraries(MCSeeds PUBLIC OpenMP::OpenMP_CXX)
endif()