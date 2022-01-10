#ifndef MCSEEDS_ISLIMECHUNKFINDER_HPP
#define MCSEEDS_ISLIMECHUNKFINDER_HPP

#include "javatypes.hpp"
#include "Grid2D.hpp"

class ISlimeChunkFinder {
public:
    virtual void look_for_slime_chunks(jlong seed, jint start_cx, jint start_cz, Grid2D<bool>& result) = 0;
};


#endif //MCSEEDS_ISLIMECHUNKFINDER_HPP
