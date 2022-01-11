#ifndef MCSEEDS_ISEEDGENERATOR_HPP
#define MCSEEDS_ISEEDGENERATOR_HPP

#include "javatypes.hpp"

class ISeedGenerator {
public:
    virtual jlong next_seed() = 0;
};


#endif //MCSEEDS_ISEEDGENERATOR_HPP
