#include "pcie/interrupt_coalescer.hpp"

#include <stdexcept>

namespace pcie {

InterruptCoalescer::InterruptCoalescer(
    sim::Simulator& simulator,
    MsixController& msix,
    VectorId vector,
    std::size_t completion_threshold,
    sim::Cycle timer
)
    : simulator_(simulator),
      msix_(msix),
      vector_(vector),
      completion_threshold_(completion_threshold),
      timer_(timer) {

    if (completion_threshold_ == 0 && timer_ == 0) {
        throw std::invalid_argument(
            "At least one coalescing mechanism must be enabled"
        );
    }

    if (static_cast<std::size_t>(vector_) >=
        msix_.vector_count()) {

        throw std::out_of_range(
            "Invalid MSI-X vector"
        );
    }
}

void InterruptCoalescer::completion(
    sim::Cycle completion_cycle
) {
    ++pending_completions_;
    ++statistics_.completions;

    if (pending_completions_ == 1) {
        first_pending_cycle_ =
            completion_cycle;
    }

    // --------------------------------------------------------
    // Threshold-based interrupt
    // --------------------------------------------------------

    if (completion_threshold_ > 0 &&
        pending_completions_ >=
            completion_threshold_) {

        generate_interrupt(
            completion_cycle
        );

        return;
    }

    // --------------------------------------------------------
    // Timer-based interrupt
    // --------------------------------------------------------

    if (timer_ > 0 && !timer_pending_) {
        timer_pending_ = true;

        const sim::Cycle interrupt_cycle =
            completion_cycle + timer_;

        simulator_.schedule(
            interrupt_cycle,
            [this, interrupt_cycle]() {

                timer_pending_ = false;

                if (pending_completions_ > 0) {
                    generate_interrupt(
                        interrupt_cycle
                    );
                }
            }
        );
    }
}

void InterruptCoalescer::flush() {
    if (pending_completions_ > 0) {
        generate_interrupt(
            simulator_.now()
        );
    }

    timer_pending_ = false;
}

void InterruptCoalescer::generate_interrupt(
    sim::Cycle interrupt_cycle
) {
    if (pending_completions_ == 0) {
        return;
    }

    const sim::Cycle latency =
        interrupt_cycle -
        first_pending_cycle_;

    statistics_.total_notification_latency +=
        latency;

    if (latency >
        statistics_.max_notification_latency) {

        statistics_.max_notification_latency =
            latency;
    }

    msix_.trigger(
        vector_,
        interrupt_cycle
    );

    ++statistics_.interrupts;

    pending_completions_ = 0;
}

const InterruptCoalescer::Statistics&
InterruptCoalescer::statistics() const {
    return statistics_;
}

}
