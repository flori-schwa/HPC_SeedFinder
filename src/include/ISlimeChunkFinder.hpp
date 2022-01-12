#ifndef MCSEEDS_ISLIMECHUNKFINDER_HPP
#define MCSEEDS_ISLIMECHUNKFINDER_HPP

#include "javatypes.hpp"
#include "slimes.hpp"
#include "Grid2D.hpp"

class ISlimeChunkFinder {
public:
    virtual void look_for_slime_chunks(const jlong seed, const jint start_cx, const jint start_cz, Grid2D<SlimeFlag>* result) = 0;
};


#endif //MCSEEDS_ISLIMECHUNKFINDER_HPP
