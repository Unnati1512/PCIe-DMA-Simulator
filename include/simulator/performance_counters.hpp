#pragma once

#include "simulator/event.hpp"

#include <cstddef>

namespace sim {

class PerformanceCounters {
public:
    struct Snapshot {
        std::size_t dma_operations{0};
        std::size_t dma_completed{0};
        std::size_t dma_failed{0};
        std::size_t bytes_transferred{0};

        std::size_t tlps_generated{0};
        std::size_t memory_reads{0};
        std::size_t memory_writes{0};
        std::size_t completion_tlps{0};
        std::size_t pcie_bytes{0};

        std::size_t requests_issued{0};
        std::size_t requests_completed{0};
        std::size_t maximum_outstanding{0};

        std::size_t interrupts{0};

        std::size_t tlb_hits{0};
        std::size_t tlb_misses{0};
        std::size_t page_walks{0};
        std::size_t translation_faults{0};

        std::size_t errors{0};
        std::size_t retries{0};
        std::size_t aborted_requests{0};

        Cycle total_latency{0};
        Cycle maximum_latency{0};
    };

    void record_dma_operation();
    void record_dma_completion();
    void record_dma_failure();
    void record_bytes_transferred(std::size_t bytes);

    void record_tlp();
    void record_memory_read();
    void record_memory_write();
    void record_completion_tlp();
    void record_pcie_bytes(std::size_t bytes);

    void record_request_issued(std::size_t outstanding);
    void record_request_completed();

    void record_interrupt();

    void record_tlb_hit();
    void record_tlb_miss();
    void record_page_walk();
    void record_translation_fault();

    void record_error();
    void record_retry();
    void record_abort();

    void record_latency(Cycle latency);

    Snapshot snapshot() const;
    void reset();

private:
    Snapshot counters_;
};

} // namespace sim
