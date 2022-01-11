#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "ISlimeChunkFinder.hpp"
#include "CPUSlimeChunkFinder.hpp"
#include "Grid2D.hpp"
#include "perfmeasure.hpp"

#define ALGO_COUNT 1

int main() {
    ISlimeChunkFinder* algos[ALGO_COUNT];
    Grid2D<bool>* results[ALGO_COUNT];

    std::fill_n(algos, ALGO_COUNT, nullptr);
    std::fill_n(results, ALGO_COUNT, nullptr);

    algos[0] = new CPUSlimeChunkFinder();

    const jlong seed = 123L;

    jint start_x = 0;
    jint start_z = 0;

    jint width = 40000, height = 40000;

//    jint start_x = 363;
//    jint start_z = 0;
//
//    jint width = 1, height = 1000000;

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
        bool err_notified = false;
        long total_errors = 0;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if ((check.at(x, y) != results[algo_num]->at(x, y))) {
                    ++total_errors;

                    if (!err_notified) {
                        std::cout << "Algorithm #" << algo_num << " produced erroneous output at position ("
                                  << (start_x + x) << ", " << (start_z + y) << ") for seed " << seed << std::endl;
                        std::cout << "Expected " << check.at(x, y) << ", got " << results[algo_num]->at(x, y)
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
}

