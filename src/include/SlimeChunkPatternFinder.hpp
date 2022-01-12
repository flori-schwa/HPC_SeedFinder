#ifndef MCSEEDS_SLIMECHUNKPATTERNFINDER_HPP
#define MCSEEDS_SLIMECHUNKPATTERNFINDER_HPP

#include <unordered_set>
#include <mutex>

#include "Grid2D.hpp"
#include "ISeedGenerator.hpp"
#include "ISlimeChunkFinder.hpp"
#include "ChunkLocation.hpp"

class SlimeChunkPatternFinder {
private:
    ISeedGenerator *seed_generator;
    ISlimeChunkFinder *slime_chunk_finder;


public:
    Grid2D<SlimeFlag> desired_pattern;

    SlimeChunkPatternFinder(ISeedGenerator *seed_generator, ISlimeChunkFinder *slime_chunk_finder, jint pattern_width,
                            jint pattern_height) :
            seed_generator(seed_generator),
            slime_chunk_finder(slime_chunk_finder),
            desired_pattern(pattern_width, pattern_height) {

    }

    void run_until_found(jint offset_x, jint offset_z, jint width, jint height, std::unordered_set<ChunkLocation>* matches);

    bool search_seed(jlong seed, jint offset_x, jint offset_z, jint width, jint height, std::mutex& vec_guard, std::unordered_set<ChunkLocation>* matches);
};


#endif //MCSEEDS_SLIMECHUNKPATTERNFINDER_HPP
