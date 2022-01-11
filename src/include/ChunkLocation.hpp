#ifndef MCSEEDS_CHUNKLOCATION_HPP
#define MCSEEDS_CHUNKLOCATION_HPP

#include "javatypes.hpp"

struct ChunkLocation {
    jlong world_seed;

    jint chunk_x;
    jint chunk_z;

    ChunkLocation(jlong worldSeed, jint chunkX, jint chunkZ) : world_seed(worldSeed), chunk_x(chunkX),
                                                               chunk_z(chunkZ) {}
};

#endif //MCSEEDS_CHUNKLOCATION_HPP
