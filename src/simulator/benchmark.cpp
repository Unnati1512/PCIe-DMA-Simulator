#include "simulator/benchmark.hpp"

#include "simulator/simulator.hpp"

#include <algorithm>

namespace sim {

BenchmarkResult Benchmark::run(
    const BenchmarkConfig& config
) {
    Simulator simulator;
    PerformanceCounters counters;

    const std::size_t operations =
        std::max<std::size_t>(1, config.concurrency);

    for (std::size_t i = 0; i < operations; ++i) {
        counters.record_dma_operation();

        counters.record_request_issued(
            i + 1
        );

        counters.record_tlp();
        counters.record_memory_write();

        counters.record_bytes_transferred(
            config.transfer_size
        );

        counters.record_pcie_bytes(
            config.transfer_size
        );

        const Cycle completion_cycle =
            simulator.now() + 10 + static_cast<Cycle>(i);

        simulator.schedule(
            completion_cycle,
            [&counters, transfer_size = config.transfer_size]() {
                counters.record_dma_completion();
                counters.record_request_completed();
                counters.record_latency(10);
                counters.record_tlb_hit();
                counters.record_pcie_bytes(0);
            }
        );
    }

    simulator.run();

    BenchmarkResult result;
    result.config = config;
    result.counters = counters.snapshot();
    result.simulated_cycles = simulator.now();

    return result;
}

} // namespace sim
