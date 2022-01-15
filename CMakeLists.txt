cmake_minimum_required(VERSION 3.21)
project(MCSeeds CXX C)

find_package(OpenMP 4.5 REQUIRED)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

if (GPU)
    add_compile_definitions(USE_OPENCL)
    find_package(OpenCL 2.2 REQUIRED)
    set(CMAKE_C_STANDARD 99)
    add_executable(dummy src/GPUSlimeChunkFinder.cl)
    set_target_properties(dummy PROPERTIES LINKER_LANGUAGE C)
endif()

add_executable(MCSeeds ${SRC_FILES})

if (GPU)
    target_link_libraries(MCSeeds PRIVATE OpenMP::OpenMP_CXX OpenCL::OpenCL)
else()
    target_link_libraries(MCSeeds PRIVATE OpenMP::OpenMP_CXX)
endif()