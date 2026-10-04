#include "pcie/tlp_fragmenter.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <vector>

int main() {
    constexpr std::size_t transfer_size = 16384;

    const std::vector<std::size_t> mps_values{
        128, 256, 512, 1024, 2048, 4096
    };

    std::ofstream csv(
        "benchmarks/data/mps.csv"
    );

    if (!csv) {
        std::cerr
            << "Failed to open mps.csv\n";
        return 1;
    }

    csv << "mps,transfer_bytes,tlp_count,"
           "payload_bytes\n";

    std::vector<pcie::Byte> payload(
        transfer_size,
        0xAA
    );

    for (const std::size_t mps : mps_values) {

        pcie::TlpFragmenter fragmenter(
            mps,
            4096
        );

        const auto tlps =
            fragmenter.fragment_memory_write(
                1,
                0x10000,
                payload,
                0
            );

        std::size_t payload_bytes = 0;

        for (const auto& tlp : tlps) {
            payload_bytes += tlp.length;
        }

        csv << mps << ','
            << transfer_size << ','
            << tlps.size() << ','
            << payload_bytes << '\n';

        std::cout
            << "MPS " << mps
            << ": TLPs=" << tlps.size()
            << ", payload_bytes="
            << payload_bytes
            << '\n';
    }

    return 0;
}
