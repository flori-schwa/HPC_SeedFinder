#ifndef MCSEEDS_SEEDGENERATORSEQUENTIAL_HPP
#define MCSEEDS_SEEDGENERATORSEQUENTIAL_HPP

#include <atomic>

#include "ISeedGenerator.hpp"

class SeedGeneratorSequential : public ISeedGenerator {
private:
    std::atomic<jlong> next;

public:
    SeedGeneratorSequential() : next(0) {}

    explicit SeedGeneratorSequential(jlong next) : next(next) {}

    jlong next_seed() override;
};

#endif //MCSEEDS_SEEDGENERATORSEQUENTIAL_HPP
