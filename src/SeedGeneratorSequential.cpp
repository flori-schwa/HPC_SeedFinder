#include "SeedGeneratorSequential.hpp"

jlong SeedGeneratorSequential::next_seed() {
    return this->next++;
}