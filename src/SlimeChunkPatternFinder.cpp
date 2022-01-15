#include <iostream>
#include "include/SlimeChunkPatternFinder.hpp"
#include "include/CPUSlimeChunkFinder.hpp"
#include "include/BoyerMoore.hpp"

//#define NAIVE
//#define BOYER_MOORE
#define LINE_BY_LINE

bool SlimeChunkPatternFinder::search_seed(jlong seed, int offset_x, int offset_z, int width, int height,
                                          std::mutex &vec_guard,
                                          std::unordered_set<ChunkLocation> *matches) {

#ifndef LINE_BY_LINE
    MEASURE_BEGIN;
    SlimeGrid chunks(width, height);
    MEASURE_END;
    MEASURE_PRINT("Allocate Chunks");

    MEASURE_BEGIN_R;
    this->slime_chunk_finder->look_for_slime_chunks(seed, offset_x, offset_z, &chunks);
    MEASURE_END_R;
    MEASURE_PRINT("Get Slime Chunks");
#endif

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

    BoyerMoore<SlimeFlagType, 2> matcher(desired_pattern.row_pointer(0), desired_pattern.width);

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
#ifdef LINE_BY_LINE

    SlimeGrid patternWithPadding(this->desired_pattern.width + 2, this->desired_pattern.height + 2);

    for (int x = 0; x < patternWithPadding.width; ++x) {
        patternWithPadding.set(x, 0, 0);
        patternWithPadding.set(x, patternWithPadding.height - 1, 0);
    }

    for (int z = 1; z < patternWithPadding.height - 1; ++z) {
        for (int x = 0; x < patternWithPadding.width; ++x) {
            if (x == 0 || x == patternWithPadding.width - 1) {
                patternWithPadding.set(x, z, 0);
            } else {
                patternWithPadding.set(x, z, this->desired_pattern.get(x - 1, z - 1));
            }
        }
    }

    struct HeaderMatch {
        int origin_x;
        int origin_z;

        HeaderMatch(int originX, int originZ) : origin_x(originX), origin_z(originZ) {}
    };

    std::vector<HeaderMatch> matching_patterns;
    std::mutex matches_guard;

    BoyerMoore<SlimeFlagType, 2> matcher(desired_pattern.row_pointer(0), desired_pattern.width);

    CPUSlimeChunkFinder cpuFinder;

#pragma omp parallel for firstprivate(width, height, seed, offset_x, offset_z) shared(matcher, matches_guard, matching_patterns, cpuFinder, patternWithPadding, matches) default(none)
    for (int z = 0; z < height; ++z) {
        SlimeGrid check(this->desired_pattern.width + 2, this->desired_pattern.height + 2);

        std::vector<size_t> first_line_matches;

        matcher.find_all_matches([&cpuFinder, seed, offset_x, offset_z, z] (size_t x) {
            return cpuFinder.is_slime_chunk(seed, offset_x + x, offset_z + z) ? 1 : 0;
        }, width, first_line_matches);

//#pragma omp parallel for shared(first_line_matches, cpuFinder, check, patternWithPadding, matches_guard, matches) firstprivate(seed, offset_x, offset_z, z) default(none)
        for (size_t match_offset : first_line_matches) {
            int x = (int) match_offset;

            cpuFinder.look_for_slime_chunks_single_thread(seed, offset_x + x - 1,
                                                          offset_z + z - 1, &check);

            for (int row = 0; row < patternWithPadding.height; ++row) {
                if (!check.row_matches(row, 0, &patternWithPadding, row)) {
                    goto no_match;
                }
            }

            matches_guard.lock();
            matches->emplace(seed, offset_x + x, offset_z + z);
            matches_guard.unlock();

            no_match:
            continue;
        }
    }
#endif

    return !matches->empty();

}

void SlimeChunkPatternFinder::run_until_found(int offset_x, int offset_z, int width, int height,
                                              std::unordered_set<ChunkLocation> *matches) {
    std::mutex vec_guard;
    volatile bool any_found = false;

    std::cout << "Search Grid dimensions: " << width << " * " << height << std::endl;

    while (!any_found) {
        jlong seed = this->seed_generator->next_seed();

        std::cout << "Checking seed " << seed << "..." << std::endl;

        bool result = this->search_seed(seed, offset_x, offset_z, width, height, vec_guard, matches);

        if (result) {
            any_found = true;
        }
    }
}