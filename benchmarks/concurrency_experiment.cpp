#include "pcie/outstanding_requests.hpp"
#include "pcie/tlp_generator.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <vector>

int main() {
    const std::vector<std::size_t> concurrency_levels{
        1, 2, 4, 8, 16, 32, 64
    };

    std::ofstream csv(
        "benchmarks/data/concurrency.csv"
    );

    if (!csv) {
        std::cerr
            << "Failed to open concurrency.csv\n";
        return 1;
    }

    csv << "concurrency,requests_issued,"
           "requests_completed,maximum_outstanding,"
           "completion_cycles\n";

    pcie::TlpGenerator generator;

    for (const std::size_t concurrency :
         concurrency_levels) {

        pcie::OutstandingRequestTracker tracker(
            concurrency
        );

        std::vector<pcie::Tag> tags;

        for (std::size_t i = 0;
             i < concurrency;
             ++i) {

            if (!tracker.can_issue()) {
                break;
            }

            const pcie::Tag tag =
                tracker.allocate_tag();

            const auto request =
                generator.create_memory_read(
                    0x1000 + i * 4096,
                    0x800000 + i * 4096,
                    4096,
                    tag
                );

            tracker.track_request(
                tag,
                request,
                static_cast<sim::Cycle>(i)
            );

            tags.push_back(tag);
        }

        const std::size_t issued =
            tracker.requests_issued();

        const std::size_t maximum =
            tracker.outstanding_count();

        for (const pcie::Tag tag : tags) {
            tracker.complete_request(tag);
        }

        const std::size_t completed =
            tracker.requests_completed();

        const std::size_t completion_cycles =
            concurrency;

        csv << concurrency << ','
            << issued << ','
            << completed << ','
            << maximum << ','
            << completion_cycles << '\n';

        std::cout
            << "Concurrency "
            << concurrency
            << ": issued=" << issued
            << ", completed=" << completed
            << ", maximum_outstanding="
            << maximum
            << '\n';
    }

    return 0;
}
