#ifndef MCSEEDS_GPUSLIMECHUNKFINDER_HPP
#define MCSEEDS_GPUSLIMECHUNKFINDER_HPP

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <cstdint>

#include "ISlimeChunkFinder.hpp"
#include "javatypes.hpp"
#include "slimes.hpp"


#define EXPECTED_PLATFORMS 3
#define EXPECTED_DEVICES 3

class GPUSlimeChunkFinder : public ISlimeChunkFinder {
private:
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    bool init_ok = false;

    void UseDevice(cl_device_id dev);
public:
    GPUSlimeChunkFinder();

    void look_for_slime_chunks(jlong seed, jint start_cx, jint start_cz, Grid2D<SlimeFlag>* result) override;
};


#endif //MCSEEDS_GPUSLIMECHUNKFINDER_HPP
