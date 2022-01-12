cmake_minimum_required(VERSION 3.21)
project(MCSeeds)

find_package(OpenMP 4.5 REQUIRED)
find_package(OpenCL 2.2 REQUIRED)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(MCSeeds ${SRC_FILES})

target_link_libraries(MCSeeds PUBLIC OpenMP::OpenMP_CXX OpenCL::OpenCL)