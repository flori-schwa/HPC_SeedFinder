#ifndef MCSEEDS_BASESLIMECHUNKFINDER_HPP
#define MCSEEDS_BASESLIMECHUNKFINDER_HPP

#include "ISlimeChunkFinder.hpp"

class BaseSlimeChunkFinder : public ISlimeChunkFinder {
public:
    void
    look_for_slime_chunks(const jlong seed, const jint start_cx, const jint start_cz, Grid2D<bool>* result) override;
};


#endif //MCSEEDS_BASESLIMECHUNKFINDER_HPP
