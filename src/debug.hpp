#ifndef MCSEEDS_DEBUG_HPP
#define MCSEEDS_DEBUG_HPP

#include <cstdio>

#define dbg_printf(fmt, ...) do { printf("[%s:%d] " fmt, __FILE__, __LINE__, __VA_ARGS__); } while (0);

#endif //MCSEEDS_DEBUG_HPP
