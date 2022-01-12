#include <iostream>

#include "include/SeedGeneratorSequential.hpp"
#include "include/CPUSlimeChunkFinder.hpp"
#include "include/SlimeChunkPatternFinder.hpp"

#include <fstream>
#include <sstream>
#include <omp.h>
#include "include/perfmeasure.hpp"

#define SEARCH_PATTERN

#ifndef SEARCH_PATTERN

#define ALGO_COUNT 1

#endif

int main() {

#ifdef SEARCH_PATTERN
    SeedGeneratorSequential seqSeedGen;
    CPUSlimeChunkFinder slimeChunkFinder;

    SlimeChunkPatternFinder patternFinder(&seqSeedGen, &slimeChunkFinder, 4, 4);

    patternFinder.desired_pattern.set(0, 0, SlimeFlag::No);
    patternFinder.desired_pattern.set(1, 0, SlimeFlag::Yes);
    patternFinder.desired_pattern.set(2, 0, SlimeFlag::Yes);
    patternFinder.desired_pattern.set(3, 0, SlimeFlag::Yes);

    patternFinder.desired_pattern.set(0, 1, SlimeFlag::Yes);
    patternFinder.desired_pattern.set(1, 1, SlimeFlag::Yes);
    patternFinder.desired_pattern.set(2, 1, SlimeFlag::No);
    patternFinder.desired_pattern.set(3, 1, SlimeFlag::No);

    patternFinder.desired_pattern.set(0, 2, SlimeFlag::Yes);
    patternFinder.desired_pattern.set(1, 2, SlimeFlag::Yes);
    patternFinder.desired_pattern.set(2, 2, SlimeFlag::Yes);
    patternFinder.desired_pattern.set(3, 2, SlimeFlag::Yes);

    patternFinder.desired_pattern.set(0, 3, SlimeFlag::No);
    patternFinder.desired_pattern.set(1, 3, SlimeFlag::Yes);
    patternFinder.desired_pattern.set(2, 3, SlimeFlag::No);
    patternFinder.desired_pattern.set(3, 3, SlimeFlag::Yes);

    std::unordered_set<ChunkLocation> results;

    MEASURE_BEGIN;
    patternFinder.run_until_found(-20000, -20000, 40000, 40000, &results);
    MEASURE_END;
    MEASURE_PRINT("Find pattern");

    const int limit = 10;
    int count = 0;

    for (const ChunkLocation &foundLocation: results) {
        if (++count == limit) {
            break;
        }

        std::cout << "Seed: " << foundLocation.world_seed << std::endl
                  << "Chunk X: " << foundLocation.chunk_x << std::endl
                  << "Chunk Z: " << foundLocation.chunk_z << std::endl
                  << "Block X: " << (foundLocation.chunk_x * 16) << std::endl
                  << "Block Z: " << (foundLocation.chunk_z * 16) << std::endl
                  << "================================================" << std::endl;
    }

#else

    ISlimeChunkFinder *algos[ALGO_COUNT] = {nullptr};
    Grid2D<bool> *results[ALGO_COUNT] = {nullptr};

    algos[0] = new CPUSlimeChunkFinder();

    const jlong seed = 123L;

    jint start_x = 0;
    jint start_z = 0;

    jint width = 40000, height = 40000;

    for (int i = 0; i < ALGO_COUNT; i++) {
        results[i] = new Grid2D<bool>(width, height);
    }

    for (int algo_num = 0; algo_num < ALGO_COUNT; algo_num++) {
        if (algos[algo_num] == nullptr) {
            continue;
        }

        MEASURE_BEGIN;
        algos[algo_num]->look_for_slime_chunks(seed, start_x, start_z, results[algo_num]);
        MEASURE_END;

        MEASURE_PRINT("Slime Chunk Finder");
    }

    std::stringstream ss;

    ss << seed << "-" << start_x << "-" << start_z << "-" << width << "-" << height << ".bin";

    std::ifstream input;
    input.open(ss.str());

    input.seekg(0, std::ios::end);
    size_t length = input.tellg();
    input.seekg(0, std::ios::beg);

    if (length != (width * height)) {
        std::cout << "Length does not match (expected: " << (width * height) << ", got: " << length << ")" << std::endl;

        input.close();
        return -1;
    }

    Grid2D<bool> check(width, height);
    input.read(reinterpret_cast<char *>(check.data), length);
    input.close();

    for (int algo_num = 0; algo_num < ALGO_COUNT; algo_num++) {
        bool err_notified = SlimeFlag::No;
        long total_errors = 0;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if ((check.get(x, y) != results[algo_num]->get(x, y))) {
                    ++total_errors;

                    if (!err_notified) {
                        std::cout << "Algorithm #" << algo_num << " produced erroneous output at position ("
                                  << (start_x + x) << ", " << (start_z + y) << ") for seed " << seed << std::endl;
                        std::cout << "Expected " << check.get(x, y) << ", got " << results[algo_num]->get(x, y)
                                  << std::endl;
                        err_notified = true;
                    }
                }
            }
        }

        if (err_notified) {
            std::cout << "Total Errors: " << total_errors << std::endl;
        }
    }

#endif
}

