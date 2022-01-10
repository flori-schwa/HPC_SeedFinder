//
// Created by florian on 10.01.22.
//

#ifndef MCSEEDS_DEBUG_H
#define MCSEEDS_DEBUG_H

#include <cstdio>

#define dbg_printf(fmt, ...) do { printf("[%s:%d] " fmt, __FILE__, __LINE__, __VA_ARGS__); } while (0);

#endif //MCSEEDS_DEBUG_H
