#ifndef MCSEEDS_CHUNKLOCATION_HPP
#define MCSEEDS_CHUNKLOCATION_HPP

#include "javatypes.hpp"
#include <functional>

struct ChunkLocation {
    jlong world_seed;

    jint chunk_x;
    jint chunk_z;

    ChunkLocation(jlong worldSeed, jint chunkX, jint chunkZ) : world_seed(worldSeed), chunk_x(chunkX),
                                                               chunk_z(chunkZ) {}

    bool operator==(const ChunkLocation& other) const;
};

namespace std {
    template<>
    struct hash<ChunkLocation> {
        std::size_t operator()(const ChunkLocation &pos) const;
    };
}

#endif //MCSEEDS_CHUNKLOCATION_HPP
