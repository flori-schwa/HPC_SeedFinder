#include "include/CPUSlimeChunkFinder.hpp"

static const jlong jrand_multiplier = 0x5DEECE66D;
static const jlong jrand_addend = 0xB;
static const jlong jrand_mask = ((jlong) 1 << 48) - 1;


static inline constexpr jlong slime_seed(const jlong world_seed, const jint x, const jint z) {
    return world_seed +
           (jlong) (x * x * 0x4c1906) +
           (jlong) (x * 0x5ac0db) +
           (jlong) (z * z) * (jlong) 0x4307a7 +
           (jlong) (z * 0x5f24f) ^ (jlong) 0x3ad8025f;
}


jint next31(jlong *seed) {
    jlong next = (*seed * jrand_multiplier + jrand_addend) & jrand_mask;
    *seed = next;
    return (jint) ((unsigned) (next >> (48 - 31)));
}

void slime_chunk_check(const int x, const int z, const jlong seed, const jint start_cx, const jint start_cz,
                       SlimeGrid *result) {
    jlong s = slime_seed(seed, x + start_cx, z + start_cz);

    s = (s ^ jrand_multiplier) & jrand_mask;
    jint r = next31(&s);

    const jint bound = 10;
    const jint m = bound - 1;

    jint u = r;

    do {
        r = u % bound;

        // Handle over- and underflows like java
        if (((signed) ((unsigned) u - (unsigned) r + (unsigned) m)) < 0) {
            u = next31(&s);
        } else {
            break;
        }
    } while (true);

    result->set(x, z, r == 0);
}

void CPUSlimeChunkFinder::look_for_slime_chunks(const jlong seed, const jint start_cx, const jint start_cz,
                                                SlimeGrid *result) {
    const int length = result->width * result->height;

#pragma omp parallel for firstprivate(seed, start_cx, start_cz, length) shared(result) default(none) collapse(2)
#ifndef GRID2D_COL_MAJOR
    for (int z = 0; z < result->height; ++z) {
        for (int x = 0; x < result->width; ++x) {
#else
            for (int x = 0; x < result->width; ++x) {
                for (int z = 0; z < result->height; ++z) {
#endif
            slime_chunk_check(x, z, seed, start_cx, start_cz, result);
        }
    }
}

void
CPUSlimeChunkFinder::look_for_slime_chunks_single_thread(const jlong seed, const jint start_cx, const jint start_cz,
                                                         SlimeGrid *result) {
#ifndef GRID2D_COL_MAJOR
    for (int z = 0; z < result->height; ++z) {
        for (int x = 0; x < result->width; ++x) {
#else
            for (int x = 0; x < result->width; ++x) {
                for (int z = 0; z < result->height; ++z) {
#endif
            slime_chunk_check(x, z, seed, start_cx, start_cz, result);
        }
    }
}

bool CPUSlimeChunkFinder::is_slime_chunk(const jlong seed, const jint x, const jint z) {
    jlong s = slime_seed(seed, x, z);

    s = (s ^ jrand_multiplier) & jrand_mask;
    jint r = next31(&s);

    const jint bound = 10;
    const jint m = bound - 1;

    jint u = r;

    do {
        r = u % bound;

        // Handle over- and underflows like java
        if (((signed) ((unsigned) u - (unsigned) r + (unsigned) m)) < 0) {
            u = next31(&s);
        } else {
            break;
        }
    } while (true);

    return r == 0;
}
