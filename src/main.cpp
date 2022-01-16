#include <iostream>
#include <fstream>

#include "include/ISlimeChunkFinder.hpp"
#include "include/CPUSlimeChunkFinder.hpp"
#include "include/SlimeChunkPatternFinder.hpp"
#include "include/perfmeasure.hpp"
#include "include/SeedGeneratorSequential.hpp"
#include <omp.h>

#ifdef USE_OPENCL
#include "include/GPUSlimeChunkFinder.hpp"
#endif

#include "include/cxxopts.hpp"

#define SEARCH_PATTERN

#ifndef SEARCH_PATTERN

#define ALGO_COUNT 2

#endif

#define MC_WORLD_WIDTH_BLOCKS (29999872)
#define MC_WORLD_HEIGHT_BLOCKS (29999872)

#define MC_WORLD_WIDTH_CHUNKS (MC_WORLD_WIDTH_BLOCKS / 16)
#define MC_WORLD_HEIGHT_CHUNKS (MC_WORLD_HEIGHT_BLOCKS / 16)

int main(int argc, char** argv) {

#ifdef SEARCH_PATTERN

    cxxopts::Options options("MCSeeds", "Minecraft Seed Pattern Finder");

    options.add_options()
        ("s,seed", "Seed to continue at", cxxopts::value<jlong>()->default_value("0"))
        ("w,width", "Width of Search Grid", cxxopts::value<int>()->default_value("40000"))
        ("h,height", "Height of Search Grid", cxxopts::value<int>()->default_value("40000"))
        ("t,threads", "Number of threads to use", cxxopts::value<int>()->default_value("-1"))
        ("file", "Input File", cxxopts::value<std::string>())
    ;

    options.parse_positional({"file"});

    jlong seed;
    int search_width, search_height, threads;
    std::string file;

    try {
        auto results = options.parse(argc, argv);

        file = results["file"].as<std::string>();
        seed = results["seed"].as<jlong>();
        search_width = results["width"].as<int>();
        search_height = results["height"].as<int>();
        threads = results["threads"].as<int>();
    } catch (cxxopts::OptionException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (search_width <= 0) {
        search_width = MC_WORLD_WIDTH_CHUNKS;
    }

    if (search_height <= 0) {
        search_height = MC_WORLD_HEIGHT_CHUNKS;
    }

    std::vector<std::string> lines;

    {
        std::ifstream in;
        in.open(file);


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
            return 1;
        }
    }

    ISlimeChunkFinder* slimeChunkFinder;

#ifdef USE_OPENCL
    GPUSlimeChunkFinder gpuFinder;
    slimeChunkFinder = &gpuFinder;
#else
    CPUSlimeChunkFinder cpuFinder;
    slimeChunkFinder = &cpuFinder;
#endif

    SeedGeneratorSequential seqSeedGen(seed);

    SlimeChunkPatternFinder patternFinder(&seqSeedGen, slimeChunkFinder, width, height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            bool flag;

            switch (lines[y][x]) {
                case '0':
                    flag = false;
                    break;
                case '1':
                    flag = true;
                    break;
                default:
                    std::cerr << "Cannot parse '" << lines[y][x] << "'" << std::endl;
                    return 0;
            }

            patternFinder.desired_pattern.set(x, y, flag);
        }
    }

    std::unordered_set<ChunkLocation> results;

    if (threads >= 0) {
        omp_set_num_threads(threads);
    }

    MEASURE_BEGIN;
    patternFinder.run_until_found(-(search_width / 2), -(search_height / 2), search_width, search_height, &results);
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
    SlimeGrid *results[ALGO_COUNT] = {nullptr};

    algos[0] = new CPUSlimeChunkFinder();
    algos[1] = new GPUSlimeChunkFinder();

    const jlong seed = 0;

    jint start_x = 0;
    jint start_z = 0;

    jint search_width = 1000, search_height = 1000;

    for (int i = 0; i < ALGO_COUNT; i++) {
        results[i] = new SlimeGrid (search_width, search_height);
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

    ss << seed << "-" << start_x << "-" << start_z << "-" << search_width << "-" << search_height << ".bin";

    std::ifstream input;
    input.open(ss.str());

    input.seekg(0, std::ios::end);
    size_t length = input.tellg();
    input.seekg(0, std::ios::beg);

    if (length != (search_width * search_height)) {
        std::cout << "Length does not match (expected: " << (search_width * search_height) << ", got: " << length << ")" << std::endl;

        input.close();
        return -1;
    }

    SlimeGrid check(search_width, search_height);
    input.read(reinterpret_cast<char *>(check.data), length);
    input.close();

    for (int algo_num = 0; algo_num < ALGO_COUNT; algo_num++) {
        bool err_notified = false;
        long total_errors = 0;

        for (int y = 0; y < search_height; ++y) {
            for (int x = 0; x < search_width; ++x) {
                if ((check.get(x, y) != results[algo_num]->get(x, y))) {
                    ++total_errors;

                    if (!err_notified) {
                        std::cout << "Algorithm #" << algo_num << " produced erroneous output at position ("
                                  << (start_x + x) << ", " << (start_z + y) << ") for seed " << seed << std::endl;
                        printf("Expected %x, got %x\n", check.get(x, y), results[algo_num]->get(x, y));
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

