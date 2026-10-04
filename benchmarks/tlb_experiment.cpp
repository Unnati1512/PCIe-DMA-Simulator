#include "memory/iommu.hpp"

#include <fstream>
#include <iostream>
#include <vector>

int main() {
    std::ofstream csv(
        "benchmarks/data/tlb.csv"
    );

    if (!csv) {
        std::cerr
            << "Failed to open tlb.csv\n";
        return 1;
    }

    csv << "accesses,tlb_hits,tlb_misses,"
           "page_walks,translation_faults\n";

    const std::vector<std::size_t> access_counts{
        1, 2, 4, 8, 16, 32, 64
    };

    for (const std::size_t accesses :
         access_counts) {

        memory::Iommu iommu;

        for (std::size_t page = 0;
             page < 16;
             ++page) {

            iommu.map_page(
                0x100000 +
                    page * memory::Iommu::PageSize,
                0x800000 +
                    page * memory::Iommu::PageSize
            );
        }

        for (std::size_t i = 0;
             i < accesses;
             ++i) {

            const memory::Address address =
                0x100000 +
                (i % 4) * memory::Iommu::PageSize +
                128;

            iommu.translate(address);
        }

        const std::size_t hits =
            iommu.tlb_hits();

        const std::size_t misses =
            iommu.tlb_misses();

        const std::size_t walks =
            iommu.page_walks();

        const std::size_t faults =
            iommu.translation_faults();

        csv << accesses << ','
            << hits << ','
            << misses << ','
            << walks << ','
            << faults << '\n';

        std::cout
            << "Accesses " << accesses
            << ": hits=" << hits
            << ", misses=" << misses
            << ", walks=" << walks
            << ", faults=" << faults
            << '\n';
    }

    return 0;
}
