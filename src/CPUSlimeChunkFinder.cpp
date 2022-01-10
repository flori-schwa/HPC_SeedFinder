#include "CPUSlimeChunkFinder.hpp"

#include <iostream>

static const jlong jrand_multiplier = 0x5DEECE66D;
static const jlong jrand_addend = 0xB;
static const jlong jrand_mask = ((jlong) 1 << 48) - 1;

static inline constexpr jlong slime_seed(const jlong world_seed, const jint x, const jint z) {
    return world_seed +
           (jint) (x * x * 0x4c1906) +
           (jint) (x * 0x5ac0db) +
           (jint) (z * z * 0x4307a7L) +
           (jint) (z * 0x5f24f) ^ 0x3ad8025fL;
}

static jint next31(jlong* seed) {
    jlong old, next;

    do {
        old = *seed;
        next = (old * jrand_multiplier + jrand_addend) & jrand_mask;

        if (*seed == old) {
            *seed = next;
            break;
        }
    } while (true);

    return (jint) ((unsigned) (next >> (48 - 31)));
}

void CPUSlimeChunkFinder::look_for_slime_chunks(jlong seed, jint start_cx, jint start_cz, Grid2D<bool> &result) {
    bool* data = result.data;
    const int width = result.width, height = result.height;

#pragma omp parallel for firstprivate(seed, start_cx, start_cz, width, height) shared(data) default(none) collapse(2)
    for (jint z = 0; z < height; ++z) {
        for (jint x = 0; x < width; ++x) {
            jlong s = slime_seed(seed, start_cx + x, start_cz + z);
            s = (s ^ jrand_multiplier) & jrand_mask;

            jint r = next31(&s);
            const jint bound = 10;
            const jint m = bound - 1;

            for (jint u = r; u - (r = u % bound) + m < 0; u = next31(&s)) {
                // nop
            }

            data[z * height + x] = r == 0;
        }
    }
}
