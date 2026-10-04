#include "pcie/tlp_fragmenter.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <vector>

int main() {
    constexpr std::size_t transfer_size = 16384;

    const std::vector<std::size_t> mrrs_values{
        128, 256, 512, 1024, 2048, 4096
    };

    std::ofstream csv(
        "benchmarks/data/mrrs.csv"
    );

    if (!csv) {
        std::cerr
            << "Failed to open mrrs.csv\n";
        return 1;
    }

    csv << "mrrs,transfer_bytes,tlp_count,"
           "requested_bytes\n";

    for (const std::size_t mrrs : mrrs_values) {

        pcie::TlpFragmenter fragmenter(
            4096,
            mrrs
        );

        const auto tlps =
            fragmenter.fragment_memory_read(
                1,
                0x20000,
                transfer_size,
                0
            );

        std::size_t requested_bytes = 0;

        for (const auto& tlp : tlps) {
            requested_bytes += tlp.length;
        }

        csv << mrrs << ','
            << transfer_size << ','
            << tlps.size() << ','
            << requested_bytes << '\n';

        std::cout
            << "MRRS " << mrrs
            << ": TLPs=" << tlps.size()
            << ", requested_bytes="
            << requested_bytes
            << '\n';
    }

    return 0;
}
