#include <iostream>
#include "ISlimeChunkFinder.hpp"
#include "BaseSlimeChunkFinder.hpp"
#include "CPUSlimeChunkFinder.hpp"
#include "Grid2D.hpp"
#include "perfmeasure.hpp"

#define ALGO_COUNT 2

int main() {
    ISlimeChunkFinder* algos[ALGO_COUNT] = {nullptr};
    Grid2D<bool>* results[ALGO_COUNT] = {nullptr};

    algos[0] = new BaseSlimeChunkFinder();
    algos[1] = new CPUSlimeChunkFinder();

    const jlong seed = 123L;

//    jint start_x = -9;
//    jint start_z = -5990;
//
//    jint width = 1, height = 1;

    jint start_x = -10;
    jint start_z = -10;

    jint width = 10000, height = 10000;

    for (int i = 0; i < ALGO_COUNT; i++) {
        results[i] = new Grid2D<bool>(width, height);
    }

    for (int algo_num = 0; algo_num < ALGO_COUNT; algo_num++) {
        if (algos[algo_num] == nullptr) {
            continue;
        }

        MEASURE_BEGIN;
        algos[algo_num]->look_for_slime_chunks(seed, start_x, start_z, *results[algo_num]);
        MEASURE_END;

        MEASURE_PRINT("Slime Chunk Finder");
    }

    Grid2D<bool> check(width, height);

    for (int algo_num = 0; algo_num < ALGO_COUNT; algo_num++) {
        bool err_notified = false;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (algo_num == 0) {
                    check.at(x, y) = results[algo_num]->at(x, y);
//                    std::cout << results[algo_num]->at(x, y) << " ";
                } else {
                    if ((check.at(x, y) ^= results[algo_num]->at(x, y)) && !err_notified) {
                        std::cout << "Algorithm #" << algo_num << " produced erroneous output at position (" << (start_x + x) << ", " << (start_z + y) << ") for seed " << seed << std::endl;
                        err_notified = true;
                    }
                }
            }

//            if (algo_num == 0) {
//                std::cout << std::endl;
//            }
        }
    }
}

