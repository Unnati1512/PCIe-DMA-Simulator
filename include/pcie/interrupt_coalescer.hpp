#pragma once

#include "pcie/msix.hpp"
#include "simulator/simulator.hpp"

#include <cstddef>
#include <cstdint>

namespace pcie {

class InterruptCoalescer {
public:
    struct Statistics {
        std::size_t completions{0};
        std::size_t interrupts{0};
        sim::Cycle total_notification_latency{0};
        sim::Cycle max_notification_latency{0};
    };

    InterruptCoalescer(
        sim::Simulator& simulator,
        MsixController& msix,
        VectorId vector,
        std::size_t completion_threshold,
        sim::Cycle timer
    );

    void completion(
        sim::Cycle completion_cycle
    );

    void flush();

    const Statistics& statistics() const;

private:
    void generate_interrupt(
        sim::Cycle interrupt_cycle
    );

    sim::Simulator& simulator_;
    MsixController& msix_;
    VectorId vector_;

    std::size_t completion_threshold_;
    sim::Cycle timer_;

    std::size_t pending_completions_{0};

    bool timer_pending_{false};

    sim::Cycle first_pending_cycle_{0};

    Statistics statistics_;
};

}
