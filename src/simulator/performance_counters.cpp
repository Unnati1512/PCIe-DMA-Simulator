#include "simulator/performance_counters.hpp"

namespace sim {

void PerformanceCounters::record_dma_operation() {
    ++counters_.dma_operations;
}

void PerformanceCounters::record_dma_completion() {
    ++counters_.dma_completed;
}

void PerformanceCounters::record_dma_failure() {
    ++counters_.dma_failed;
}

void PerformanceCounters::record_bytes_transferred(
    std::size_t bytes
) {
    counters_.bytes_transferred += bytes;
}

void PerformanceCounters::record_tlp() {
    ++counters_.tlps_generated;
}

void PerformanceCounters::record_memory_read() {
    ++counters_.memory_reads;
}

void PerformanceCounters::record_memory_write() {
    ++counters_.memory_writes;
}

void PerformanceCounters::record_completion_tlp() {
    ++counters_.completion_tlps;
}

void PerformanceCounters::record_pcie_bytes(
    std::size_t bytes
) {
    counters_.pcie_bytes += bytes;
}

void PerformanceCounters::record_request_issued(
    std::size_t outstanding
) {
    ++counters_.requests_issued;

    if (outstanding > counters_.maximum_outstanding) {
        counters_.maximum_outstanding = outstanding;
    }
}

void PerformanceCounters::record_request_completed() {
    ++counters_.requests_completed;
}

void PerformanceCounters::record_interrupt() {
    ++counters_.interrupts;
}

void PerformanceCounters::record_tlb_hit() {
    ++counters_.tlb_hits;
}

void PerformanceCounters::record_tlb_miss() {
    ++counters_.tlb_misses;
}

void PerformanceCounters::record_page_walk() {
    ++counters_.page_walks;
}

void PerformanceCounters::record_translation_fault() {
    ++counters_.translation_faults;
}

void PerformanceCounters::record_error() {
    ++counters_.errors;
}

void PerformanceCounters::record_retry() {
    ++counters_.retries;
}

void PerformanceCounters::record_abort() {
    ++counters_.aborted_requests;
}

void PerformanceCounters::record_latency(Cycle latency) {
    counters_.total_latency += latency;

    if (latency > counters_.maximum_latency) {
        counters_.maximum_latency = latency;
    }
}

PerformanceCounters::Snapshot
PerformanceCounters::snapshot() const {
    return counters_;
}

void PerformanceCounters::reset() {
    counters_ = Snapshot{};
}

} // namespace sim
