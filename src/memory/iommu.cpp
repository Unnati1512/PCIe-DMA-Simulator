#include "memory/iommu.hpp"

#include <stdexcept>

namespace memory {

Iommu::Iommu() = default;

Address Iommu::page_number(Address address) {
    return address / PageSize;
}

Address Iommu::page_offset(Address address) {
    return address % PageSize;
}

void Iommu::map_page(
    Address iova,
    Address physical_address
) {
    if (iova % PageSize != 0) {
        throw std::invalid_argument(
            "IOVA must be page aligned"
        );
    }

    if (physical_address % PageSize != 0) {
        throw std::invalid_argument(
            "Physical address must be page aligned"
        );
    }

    page_table_[page_number(iova)] =
        physical_address / PageSize;

    // Remove an old cached translation if the mapping
    // is changed.
    tlb_.erase(page_number(iova));
}

void Iommu::unmap_page(Address iova) {
    page_table_.erase(page_number(iova));
    tlb_.erase(page_number(iova));
}

Iommu::TranslationResult
Iommu::translate(Address iova) {
    const Address iova_page =
        page_number(iova);

    const Address offset =
        page_offset(iova);

    // --------------------------------------------------------
    // TLB lookup
    // --------------------------------------------------------

    const auto tlb_it =
        tlb_.find(iova_page);

    if (tlb_it != tlb_.end()) {
        ++tlb_hits_;

        const Address physical_address =
            tlb_it->second.physical_page * PageSize
            + offset;

        return TranslationResult{
            true,
            physical_address,
            true,
            false
        };
    }

    // --------------------------------------------------------
    // TLB miss
    // --------------------------------------------------------

    ++tlb_misses_;

    // Simulate a page-table walk.
    ++page_walks_;

    const auto page_it =
        page_table_.find(iova_page);

    if (page_it == page_table_.end()) {
        ++translation_faults_;

        return TranslationResult{
            false,
            0,
            false,
            true
        };
    }

    const Address physical_page =
        page_it->second;

    // Fill TLB.
    tlb_[iova_page] =
        TlbEntry{
            physical_page
        };

    const Address physical_address =
        physical_page * PageSize
        + offset;

    return TranslationResult{
        true,
        physical_address,
        false,
        true
    };
}

void Iommu::clear_tlb() {
    tlb_.clear();
}

std::size_t Iommu::tlb_hits() const {
    return tlb_hits_;
}

std::size_t Iommu::tlb_misses() const {
    return tlb_misses_;
}

std::size_t Iommu::page_walks() const {
    return page_walks_;
}

std::size_t Iommu::translation_faults() const {
    return translation_faults_;
}

} // namespace memory
