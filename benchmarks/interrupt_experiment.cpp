#include "pcie/interrupt_coalescer.hpp"
#include "pcie/msix.hpp"
#include "simulator/simulator.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <vector>

int main() {
    constexpr std::size_t completion_count = 16;

    const std::vector<std::size_t> thresholds{
        1, 2, 4, 8
    };

    std::ofstream csv(
        "benchmarks/data/interrupts.csv"
    );

    if (!csv) {
        std::cerr
            << "Failed to open interrupts.csv\n";
        return 1;
    }

    csv << "threshold,completions,interrupts,"
           "total_notification_latency,"
           "max_notification_latency\n";

    for (const std::size_t threshold : thresholds) {

        sim::Simulator simulator;

        pcie::MsixController msix(1);

        msix.register_handler(
            0,
            [](pcie::VectorId, sim::Cycle) {}
        );

        pcie::InterruptCoalescer coalescer(
            simulator,
            msix,
            0,
            threshold,
            0
        );

        for (std::size_t i = 0;
             i < completion_count;
             ++i) {

            const sim::Cycle completion_cycle =
                static_cast<sim::Cycle>(i * 10);

            coalescer.completion(
                completion_cycle
            );
        }

        coalescer.flush();

        const auto& statistics =
            coalescer.statistics();

        csv << threshold << ','
            << statistics.completions << ','
            << statistics.interrupts << ','
            << statistics.total_notification_latency
            << ','
            << statistics.max_notification_latency
            << '\n';

        std::cout
            << "Threshold " << threshold
            << ": completions="
            << statistics.completions
            << ", interrupts="
            << statistics.interrupts
            << ", total_latency="
            << statistics.total_notification_latency
            << ", max_latency="
            << statistics.max_notification_latency
            << '\n';
    }

    return 0;
}
