#include "include/CPUSlimeChunkFinder.hpp"

static const jlong jrand_multiplier = 0x5DEECE66D;
static const jlong jrand_addend = 0xB;
static const jlong jrand_mask = ((jlong) 1 << 48) - 1;


static jlong slime_seed(jlong world_seed, jint x, jint z) {
    return world_seed +
           (jlong) (x * x * 0x4c1906) +
           (jlong) (x * 0x5ac0db) +
           (jlong) (z * z) * (jlong) 0x4307a7 +
           (jlong) (z * 0x5f24f) ^ (jlong) 0x3ad8025f;
}


jint next31(jlong *seed) {
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

void CPUSlimeChunkFinder::look_for_slime_chunks(const jlong seed, const jint start_cx, const jint start_cz,
                                                Grid2D<bool> *result) {
    const int length = result->width * result->height;

#pragma omp parallel for firstprivate(seed, start_cx, start_cz, length) shared(result) default(none)
    for (jint i = 0; i < length; ++i) {
        jint x = i % result->width;
        jint z = i / result->width;

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
}
