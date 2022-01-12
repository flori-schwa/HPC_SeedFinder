#ifndef MCSEEDS_CPUSLIMECHUNKFINDER_HPP
#define MCSEEDS_CPUSLIMECHUNKFINDER_HPP

#include "ISlimeChunkFinder.hpp"

class CPUSlimeChunkFinder : public ISlimeChunkFinder {

    void look_for_slime_chunks(const jlong seed, const jint start_cx, const jint start_cz, Grid2D<SlimeFlag>* result) override;

};


#endif //MCSEEDS_CPUSLIMECHUNKFINDER_HPP
