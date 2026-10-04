#pragma once

#include "simulator/performance_counters.hpp"

#include <cstddef>
#include <string>

namespace sim {

struct BenchmarkConfig {
    std::string name;
    std::size_t transfer_size{0};
    std::size_t concurrency{1};
};

struct BenchmarkResult {
    BenchmarkConfig config;
    PerformanceCounters::Snapshot counters;
    Cycle simulated_cycles{0};
};

class Benchmark {
public:
    static BenchmarkResult run(
        const BenchmarkConfig& config
    );
};

} // namespace sim
