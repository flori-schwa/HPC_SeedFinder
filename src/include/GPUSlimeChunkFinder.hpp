#ifndef MCSEEDS_GPUSLIMECHUNKFINDER_HPP
#define MCSEEDS_GPUSLIMECHUNKFINDER_HPP

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 220
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

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
public:
    GPUSlimeChunkFinder();

    ~GPUSlimeChunkFinder();

    void look_for_slime_chunks(jlong seed, jint start_cx, jint start_cz, Grid2D<SlimeFlag>* result) override;
};


#endif //MCSEEDS_GPUSLIMECHUNKFINDER_HPP
