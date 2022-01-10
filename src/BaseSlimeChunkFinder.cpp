#include "BaseSlimeChunkFinder.hpp"

#include <iostream>

// https://github.com/openjdk/jdk/blob/master/src/java.base/share/classes/java/util/Random.java
class JRandom {
private:
    static const jlong multiplier = 0x5DEECE66D;
    static const jlong addend = 0xB;
    static const jlong mask = ((jlong) 1 << 48) - 1;

    static bool compare_and_set(jlong *write_to, jlong expected, jlong update) {
        if (*write_to == expected) {
            *write_to = update;
            return true;
        }

        return false;
    }

    static constexpr jlong initial_scramble(jlong seed) {
        return (seed ^ multiplier) & mask;
    }

public:
    jlong seed;

    explicit JRandom(jlong seed) : seed(initial_scramble(seed)) {}

    jint next(int bits) {
        jlong oldseed, nextseed;

        do {
            oldseed = this->seed;
            nextseed = (oldseed * multiplier + addend) & mask;
        } while (!JRandom::compare_and_set(&this->seed, oldseed, nextseed));

        return (jint) ((unsigned) (nextseed >> (48 - bits)));
    }

    jint nextInt() {
        return next(32);
    }

    jint nextInt(jint bound) {
        jint r = next(31);
        jint m = bound - 1;

        if ((bound & m) == 0) { // power of 2
            r = (jint) ((bound * (jlong) r) >> 31);
        } else {
            for (jint u = r; u - (r = u % bound) + m < 0; u = next(31)) {
                // nothing
            }
        }

        return r;
    }
};

// https://minecraft.fandom.com/wiki/Slime?so=search#Java_Edition
bool is_slime_chunk(jlong seed, jint x, jint z) {
    return JRandom(
            seed +
            (jint) (x * x * 0x4c1906) +
            (jint) (x * 0x5ac0db) +
            (jint) (z * z * 0x4307a7L) +
            (jint) (z * 0x5f24f) ^ 0x3ad8025fL
    ).nextInt(10) == 0;
}

/*
 * Single threaded 1000x1000: Duration: 0.007748 [s]
 * Multi threaded 1000x1000: Duration:  0.001737 [s]
 */
void BaseSlimeChunkFinder::look_for_slime_chunks(jlong seed, jint start_cx, jint start_cz, Grid2D<bool>& result) {
    for (jint z = 0; z < result.height; ++z) {
        for (jint x = 0; x < result.width; ++x) {
            result.at(x, z) = is_slime_chunk(seed, start_cx + x, start_cz + z);
        }
    }
}
