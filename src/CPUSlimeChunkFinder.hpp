//
// Created by flori on 10/01/2022.
//

#ifndef MCSEEDS_CPUSLIMECHUNKFINDER_HPP
#define MCSEEDS_CPUSLIMECHUNKFINDER_HPP

#include "ISlimeChunkFinder.hpp"

class CPUSlimeChunkFinder : public ISlimeChunkFinder {

    void look_for_slime_chunks(jlong seed, jint start_cx, jint start_cz, Grid2D<bool> &result) override;

};


#endif //MCSEEDS_CPUSLIMECHUNKFINDER_HPP
