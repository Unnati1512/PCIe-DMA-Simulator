#include "simulator/benchmark.hpp"

#include <cassert>
#include <iostream>

int main() {
    std::cout
        << "=== Day 13 Benchmark Harness Test ===\n\n";

    sim::BenchmarkConfig config{
        "dma_4k_concurrency_4",
        4096,
        4
    };

    const auto result =
        sim::Benchmark::run(config);

    assert(result.config.name ==
           "dma_4k_concurrency_4");

    assert(result.config.transfer_size == 4096);

    assert(result.config.concurrency == 4);

    assert(result.counters.dma_operations == 4);

    assert(result.counters.dma_completed == 4);

    assert(result.counters.requests_issued == 4);

    assert(result.counters.requests_completed == 4);

    assert(result.counters.bytes_transferred ==
           4 * 4096);

    assert(result.counters.tlps_generated == 4);

    assert(result.simulated_cycles == 13);

    std::cout << "Benchmark: "
              << result.config.name << '\n';

    std::cout << "Transfer size: "
              << result.config.transfer_size
              << " bytes\n";

    std::cout << "Concurrency: "
              << result.config.concurrency << '\n';

    std::cout << "DMA operations: "
              << result.counters.dma_operations << '\n';

    std::cout << "Bytes transferred: "
              << result.counters.bytes_transferred
              << '\n';

    std::cout << "Simulated cycles: "
              << result.simulated_cycles << '\n';

    std::cout
        << "\nBenchmark harness test passed.\n";

    return 0;
}
