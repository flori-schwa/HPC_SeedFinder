//
// Created by flori on 10/01/2022.
//

#ifndef MCSEEDS_CPUSLIMECHUNKFINDER_HPP
#define MCSEEDS_CPUSLIMECHUNKFINDER_HPP

#include "ISlimeChunkFinder.hpp"

class CPUSlimeChunkFinder : public ISlimeChunkFinder {

    void look_for_slime_chunks(const jlong seed, const jint start_cx, const jint start_cz, Grid2D<bool>* result) override;

};


#endif //MCSEEDS_CPUSLIMECHUNKFINDER_HPP
