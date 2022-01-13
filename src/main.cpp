#include <iostream>
#include <fstream>
#include <sstream>
#include <omp.h>

#include "include/ISlimeChunkFinder.hpp"
#include "include/CPUSlimeChunkFinder.hpp"
#include "include/SlimeChunkPatternFinder.hpp"
#include "include/perfmeasure.hpp"
#include "include/GPUSlimeChunkFinder.hpp"
#include "include/SeedGeneratorSequential.hpp"

//#define SEARCH_PATTERN

#ifndef SEARCH_PATTERN

#define ALGO_COUNT 2

#endif

int main(int argc, char** argv) {

#ifdef SEARCH_PATTERN

    if (argc != 2) {
        std::cerr << "Syntax: " << argv[0] << " <pattern file>" << std::endl;
        return 0;
    }

    std::vector<std::string> lines;

    {
        std::ifstream in;
        in.open(argv[1]);


        if (in.is_open()) {
            std::string line;

            while (std::getline(in, line)) {
                if (!line.empty()) {
                    lines.push_back(line);
                }
            }
        } else {
            std::cerr << "Failed to open File \"" << argv[1] << "\"" << std::endl;
            return 0;
        }

        in.close();
    }

    int height = (int) lines.size();
    int width = (int) lines[0].size();

    for (const auto &item : lines) {
        if (item.size() != width) {
            std::cerr << "All lines in pattern input file must have the same length" << std::endl;
        }
    }

    SeedGeneratorSequential seqSeedGen;
    CPUSlimeChunkFinder slimeChunkFinder;

    SlimeChunkPatternFinder patternFinder(&seqSeedGen, &slimeChunkFinder, width, height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            SlimeFlag flag;

            switch (lines[y][x]) {
                case '0':
                    flag = SlimeFlag::No;
                    break;
                case '1':
                    flag = SlimeFlag::Yes;
                    break;
                default:
                    std::cerr << "Cannot parse '" << lines[y][x] << "'" << std::endl;
                    return 0;
            }

            patternFinder.desired_pattern.set(x, y, flag);
        }
    }

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
    Grid2D<SlimeFlag> *results[ALGO_COUNT] = {nullptr};

    algos[0] = new CPUSlimeChunkFinder();
    algos[1] = new GPUSlimeChunkFinder();

    const jlong seed = 123L;

    jint start_x = 0;
    jint start_z = 0;

    jint width = 100, height = 100;

    for (int i = 0; i < ALGO_COUNT; i++) {
        results[i] = new Grid2D<SlimeFlag>(width, height);
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

