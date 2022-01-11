#include "SlimeChunkPatternFinder.hpp"



bool SlimeChunkPatternFinder::search_seed(jlong seed, jint offset_x, jint offset_z, jint width, jint height,
                                          std::mutex& vec_guard,
                                          std::vector<ChunkLocation> *matches) {

    Grid2D<bool> chunks(width, height);
    this->slime_chunk_finder->look_for_slime_chunks(seed, offset_x, offset_z, &chunks);

#pragma omp parallel for shared(chunks, vec_guard, matches) firstprivate(seed, offset_x, offset_z, width, height) default(none) collapse(2)
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            if ((x + this->desired_pattern.width) > chunks.width ||
                (z + this->desired_pattern.height) > chunks.height) {
                continue;
            }

            for (int rz = 0; rz < this->desired_pattern.height; ++rz) {
                for (int rx = 0; rx < this->desired_pattern.width; ++rx) {
                    if (chunks.get(x + rx, z + rz) != this->desired_pattern.get(rx, rz)) {
                        goto no_match;
                    }
                }
            }

            vec_guard.lock();
            matches->emplace_back(seed, offset_x + x, offset_z + z);
            vec_guard.unlock();

            no_match:
            continue;
        }
    }

    return !matches->empty();
}

void SlimeChunkPatternFinder::run_until_found(jint offset_x, jint offset_z, jint width, jint height,
                                              std::vector<ChunkLocation> *matches) {
    std::mutex vec_guard;
    volatile bool any_found = false;

//#pragma omp parallel shared(vec_guard, any_found, matches) firstprivate(offset_x, offset_z, width, height) default(none)
//    {
        while (!any_found) {
            jlong seed = this->seed_generator->next_seed();

            if (this->search_seed(seed, offset_x, offset_z, width, height, vec_guard, matches)) {
                any_found = true;
                break;
            }
        }
//    }
}
