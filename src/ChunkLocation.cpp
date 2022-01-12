#include "include/ChunkLocation.hpp"

bool ChunkLocation::operator==(const ChunkLocation &other) const {
    return this->world_seed == other.world_seed &&
            this->chunk_x == other.chunk_x &&
            this->chunk_z == other.chunk_z;
}

// https://stackoverflow.com/a/2595226
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

std::size_t std::hash<ChunkLocation>::operator()(const ChunkLocation& pos) const {
    std::size_t hash = 0;

    hash_combine(hash, pos.world_seed);
    hash_combine(hash, pos.chunk_x);
    hash_combine(hash, pos.chunk_z);

    return hash;
}