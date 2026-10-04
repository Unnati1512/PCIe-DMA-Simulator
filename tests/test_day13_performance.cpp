#include "simulator/benchmark.hpp"
#include "simulator/performance_counters.hpp"

#include <cassert>
#include <iostream>

int main() {
    std::cout
        << "=== Day 13 Performance Counter Test ===\n\n";

    sim::PerformanceCounters counters;

    counters.record_dma_operation();
    counters.record_dma_operation();

    counters.record_dma_completion();
    counters.record_bytes_transferred(4096);

    counters.record_tlp();
    counters.record_memory_read();
    counters.record_memory_write();
    counters.record_completion_tlp();
    counters.record_pcie_bytes(4352);

    counters.record_request_issued(1);
    counters.record_request_issued(2);
    counters.record_request_completed();

    counters.record_interrupt();

    counters.record_tlb_hit();
    counters.record_tlb_miss();
    counters.record_page_walk();
    counters.record_translation_fault();

    counters.record_error();
    counters.record_retry();
    counters.record_abort();

    counters.record_latency(20);
    counters.record_latency(35);

    const auto snapshot = counters.snapshot();

    assert(snapshot.dma_operations == 2);
    assert(snapshot.dma_completed == 1);
    assert(snapshot.bytes_transferred == 4096);

    assert(snapshot.tlps_generated == 1);
    assert(snapshot.memory_reads == 1);
    assert(snapshot.memory_writes == 1);
    assert(snapshot.completion_tlps == 1);
    assert(snapshot.pcie_bytes == 4352);

    assert(snapshot.requests_issued == 2);
    assert(snapshot.requests_completed == 1);
    assert(snapshot.maximum_outstanding == 2);

    assert(snapshot.interrupts == 1);

    assert(snapshot.tlb_hits == 1);
    assert(snapshot.tlb_misses == 1);
    assert(snapshot.page_walks == 1);
    assert(snapshot.translation_faults == 1);

    assert(snapshot.errors == 1);
    assert(snapshot.retries == 1);
    assert(snapshot.aborted_requests == 1);

    assert(snapshot.total_latency == 55);
    assert(snapshot.maximum_latency == 35);

    counters.reset();

    const auto reset_snapshot = counters.snapshot();

    assert(reset_snapshot.dma_operations == 0);
    assert(reset_snapshot.bytes_transferred == 0);
    assert(reset_snapshot.tlps_generated == 0);
    assert(reset_snapshot.requests_issued == 0);
    assert(reset_snapshot.interrupts == 0);
    assert(reset_snapshot.errors == 0);
    assert(reset_snapshot.total_latency == 0);

    std::cout << "DMA operations: "
              << snapshot.dma_operations << '\n';

    std::cout << "Bytes transferred: "
              << snapshot.bytes_transferred << '\n';

    std::cout << "TLPs generated: "
              << snapshot.tlps_generated << '\n';

    std::cout << "Requests issued: "
              << snapshot.requests_issued << '\n';

    std::cout << "Maximum outstanding: "
              << snapshot.maximum_outstanding << '\n';

    std::cout << "Interrupts: "
              << snapshot.interrupts << '\n';

    std::cout << "TLB hits: "
              << snapshot.tlb_hits << '\n';

    std::cout << "TLB misses: "
              << snapshot.tlb_misses << '\n';

    std::cout << "Errors: "
              << snapshot.errors << '\n';

    std::cout << "Retries: "
              << snapshot.retries << '\n';

    std::cout << "Aborted requests: "
              << snapshot.aborted_requests << '\n';

    std::cout << "Total latency: "
              << snapshot.total_latency << '\n';

    std::cout << "Maximum latency: "
              << snapshot.maximum_latency << '\n';

    std::cout
        << "\nPerformance counter test passed.\n";

    return 0;
}
