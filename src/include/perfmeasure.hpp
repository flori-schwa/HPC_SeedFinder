#ifndef MCSEEDS_PERFMEASURE_HPP
#define MCSEEDS_PERFMEASURE_HPP

#include <chrono>
#include <iostream>

#define NS_PER_SEC (1000000000)

#define NOW(Name) std::chrono::steady_clock::time_point Name = std::chrono::steady_clock::now()

#define MEASURE_BEGIN NOW(__begin)
#define MEASURE_END NOW(__end)

#define MEASURE_PRINT(Prefix) do { std::cout << std::fixed << "[" Prefix "] " << "Duration: " << (std::chrono::duration_cast<std::chrono::nanoseconds>(__end - __begin).count() / (double) NS_PER_SEC) << " [s]" << std::endl; } while (0)

#endif //MCSEEDS_PERFMEASURE_HPP
