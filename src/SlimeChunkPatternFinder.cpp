#include <iostream>
#include "include/SlimeChunkPatternFinder.hpp"
#include "include/BoyerMoore.hpp"

#define DO_MEASURE

#ifdef DO_MEASURE
#include "include/perfmeasure.hpp"
#else

#define MEASURE_BEGIN
#define MEASURE_END do
#define MEASURE_BEGIN_R
#define MEASURE_END_R
#define MEASURE_PRINT(Prefix)

#endif

//#define NAIVE
#define BOYER_MOORE

bool SlimeChunkPatternFinder::search_seed(jlong seed, jint offset_x, jint offset_z, jint width, jint height,
                                          std::mutex &vec_guard,
                                          std::unordered_set<ChunkLocation> *matches) {
    MEASURE_BEGIN;
    SlimeGrid chunks(width, height);
    MEASURE_END;
    MEASURE_PRINT("Allocate Chunks");

    MEASURE_BEGIN_R;
    this->slime_chunk_finder->look_for_slime_chunks(seed, offset_x, offset_z, &chunks);
    MEASURE_END_R;
    MEASURE_PRINT("Get Slime Chunks");

#ifdef NAIVE

    MEASURE_BEGIN_R;

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
            matches->emplace(seed, offset_x + x, offset_z + z);
            vec_guard.unlock();

            no_match:
            continue;
        }
    }

    MEASURE_END_R;
    MEASURE_PRINT("Search Pattern Naive");

#endif
#ifdef BOYER_MOORE
    const int search_rows = height - desired_pattern.height - 1;

    const int bounding_box_width = desired_pattern.width + 2;
    const int bounding_box_height = desired_pattern.height + 2;

    BoyerMoore<bool, 2> matcher(desired_pattern.row_pointer(0), desired_pattern.width);

    MEASURE_BEGIN_R;

#pragma omp parallel for firstprivate(seed, height, offset_x, offset_z, search_rows, bounding_box_width, bounding_box_height) shared(chunks, vec_guard, matches, matcher) default(none)
    for (int z = 1; z < search_rows; ++z) {
        std::vector<size_t> offsets;
        matcher.find_all_matches(chunks.row_pointer(z), chunks.width, offsets);

        for (int i = 0; i < offsets.size(); ++i) {
            int x = offsets[i];

            if (x == 0 || x >= (chunks.width - desired_pattern.width - 1)) {
                continue;
            }

            bool match = true;

            for (int bb = 0; bb < bounding_box_width; ++bb) {
                if (chunks.get((x - 1) + bb, z - 1) ||
                    chunks.get((x - 1) + bb, z + desired_pattern.height)) {
                    match = false;
                    break;
                }
            }

            if (match) {
                for (int pattern_row = 0; pattern_row < desired_pattern.height; ++pattern_row) {
                    if (
                            !chunks.row_matches(z + pattern_row, x, &desired_pattern, pattern_row)
                            || (chunks.get(x - 1, z + pattern_row) ||
                                chunks.get(x + desired_pattern.width, z + pattern_row))) {
                        match = false;
                        break;
                    }
                }
            }

            if (match) {
                vec_guard.lock();
                matches->emplace(seed, offset_x + x, offset_z + z);
                vec_guard.unlock();
            }
        }
    }

    MEASURE_END_R;
    MEASURE_PRINT("Search Pattern using Boyer-Moore");
#endif

    return !matches->empty();
}

void SlimeChunkPatternFinder::run_until_found(jint offset_x, jint offset_z, jint width, jint height,
                                              std::unordered_set<ChunkLocation> *matches) {
    std::mutex vec_guard;
    volatile bool any_found = false;

    while (!any_found) {
        jlong seed = this->seed_generator->next_seed();

        std::cout << "Checking seed " << seed << "..." << std::endl;

        MEASURE_BEGIN;
        bool result = this->search_seed(seed, offset_x, offset_z, width, height, vec_guard, matches);
        MEASURE_END;

//        MEASURE_PRINT("Check Single Seed");

        if (result) {
            any_found = true;
            break;
        }
    }
}