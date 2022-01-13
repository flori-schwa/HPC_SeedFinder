cmake_minimum_required(VERSION 3.21)
project(MCSeeds CXX C)

find_package(OpenMP 4.5 REQUIRED)
find_package(OpenCL 2.2 REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(MCSeeds ${SRC_FILES})

add_executable(dummy src/GPUSlimeChunkFinder.cl)

set_target_properties(dummy PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(MCSeeds PUBLIC OpenMP::OpenMP_CXX OpenCL::OpenCL)