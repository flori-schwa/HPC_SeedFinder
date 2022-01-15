#ifndef MCSEEDS_GPUSLIMECHUNKFINDER_HPP
#define MCSEEDS_GPUSLIMECHUNKFINDER_HPP

#ifdef USE_OPENCL

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 220
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <mutex>

#include "ISlimeChunkFinder.hpp"
#include "javatypes.hpp"
#include "slimes.hpp"


class GPUSlimeChunkFinder : public ISlimeChunkFinder {
private:
    cl::Device* device;
    cl::Context context;
    cl::CommandQueue queue;
    cl::Program program;
    cl::Kernel kernel;
    bool init_ok = false;
    unsigned long long max_memory;
    std::mutex kernel_guard;

    void get_slime_chunks_1d(const jlong seed, const jint offset_x, const jint offset_z, SlimeGrid* result);
public:
    GPUSlimeChunkFinder();

    ~GPUSlimeChunkFinder();

    void look_for_slime_chunks(jlong seed, jint start_cx, jint start_cz, SlimeGrid* result) override;

};

#endif

#endif //MCSEEDS_GPUSLIMECHUNKFINDER_HPP
