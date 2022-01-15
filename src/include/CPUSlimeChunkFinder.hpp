#ifndef MCSEEDS_CPUSLIMECHUNKFINDER_HPP
#define MCSEEDS_CPUSLIMECHUNKFINDER_HPP

#include "ISlimeChunkFinder.hpp"

class CPUSlimeChunkFinder : public ISlimeChunkFinder {

public:

    void look_for_slime_chunks(const jlong seed, const jint start_cx, const jint start_cz, SlimeGrid* result) override;

    void look_for_slime_chunks_single_thread(const jlong seed, const jint start_cx, const jint start_cz, SlimeGrid* result);

    bool is_slime_chunk(const jlong seed, const jint x, const jint z);

};


#endif //MCSEEDS_CPUSLIMECHUNKFINDER_HPP
