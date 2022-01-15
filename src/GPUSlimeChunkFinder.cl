const long jrand_multiplier = 0x5DEECE66D;
const long jrand_addend = 0xB;
const long jrand_mask = ((long) 1 << 48) - 1;

int next31(long *seed) {
    long next = (*seed * jrand_multiplier + jrand_addend) & jrand_mask;
    *seed = next;
    return (int) ((unsigned) (next >> (48 - 31)));
}

long slime_seed(long world_seed, int x, int z) {
    long a = (signed) (((unsigned) x * (unsigned) x) * (unsigned) 0x4c1906);
    long b = (long) ((int) x * (int) 0x5ac0db);
    long c = ((long) ((int) (z * z))) * ((long) 0x4307a7);
    long d = (long) ((int) z * (int) 0x5f24f);
    long e = 0x3ad8025f;
    long f = (long) ((ulong) world_seed + (ulong) a + (ulong) b + (ulong) c + (ulong) d);

    return (long) ((ulong) f ^ (ulong) e);
}

__kernel void look_for_slime_chunks(long seed, int offset_x, int offset_z, long width, int dimensions, __global unsigned char* buffer) {
    int x, z;

    if (dimensions == 1) {
        x = get_global_id(0);
        z = 0;
    } else {
        z = get_global_id(0);
        x = get_global_id(1);
    }

    long s = slime_seed(seed, x + offset_x, z + offset_z);
    s = (s ^ jrand_multiplier) & jrand_mask;
    int r = next31(&s);

    const int bound = 10;
    const int m = bound - 1;

    int u = r;

    do {
        r = u % bound;

        if ((u - r +  m) < 0) {
            u = next31(&s);
        } else {
            break;
        }
    } while(true);

    //printf("(%u, %u): %u\n", x, z, r == 0);
    buffer[z * width + x] = r == 0;
}


/*
const long jrand_multiplier = 0x5DEECE66D;
const long jrand_addend = 0xB;
const long jrand_mask = ((long) 1 << 48) - 1;

long slime_seed(long world_seed, int x, int z) {
    return world_seed +
           (long) (x * x * 0x4c1906) +
           (long) (x * 0x5ac0db) +
           (long) (z * z) * (long) 0x4307a7 +
           (long) (z * 0x5f24f) ^ (long) 0x3ad8025f;
}


int next31(long *seed) {
    long old, next;

    do {
        old = *seed;
        next = (old * jrand_multiplier + jrand_addend) & jrand_mask;

        if (*seed == old) {
            *seed = next;
            break;
        }
    } while (1);

    return (int) ((unsigned) (next >> (48 - 31)));
}

kernel void look_for_slime_chunks(constant long seed, constant int start_cx, constant int start_cz) {
    const int length = result->width * result->height;

    for (int z = 0; z < result->height; ++z) {
        for (int x = 0; x < result->width; ++x) {

            long s = slime_seed(seed, x + start_cx, z + start_cz);

            s = (s ^ jrand_multiplier) & jrand_mask;
            int r = next31(&s);

            const int bound = 10;
            const int m = bound - 1;

            int u = r;

            do {
                r = u % bound;

                // Handle over- and underflows like java
                if (((signed) ((unsigned) u - (unsigned) r + (unsigned) m)) < 0) {
                    u = next31(&s);
                } else {
                    break;
                }
            } while (1);

            result->set(x, z, r == 0 ? SlimeFlag::Yes : SlimeFlag::No);
        }
    }
}
*/

