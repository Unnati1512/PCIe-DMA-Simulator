#pragma once

#include "memory/memory.hpp"

#include <cstddef>
#include <cstdint>
#include <unordered_map>

namespace memory {

class Iommu {
public:
    static constexpr std::size_t PageSize = 4096;

    struct TranslationResult {
        bool success;
        Address physical_address;
        bool tlb_hit;
        bool page_walk;
    };

    Iommu();

    void map_page(
        Address iova,
        Address physical_address
    );

    void unmap_page(
        Address iova
    );

    TranslationResult translate(
        Address iova
    );

    void clear_tlb();

    std::size_t tlb_hits() const;

    std::size_t tlb_misses() const;

    std::size_t page_walks() const;

    std::size_t translation_faults() const;

private:
    struct TlbEntry {
        Address physical_page;
    };

    static Address page_number(Address address);

    static Address page_offset(Address address);

    std::unordered_map<Address, Address> page_table_;

    std::unordered_map<Address, TlbEntry> tlb_;

    std::size_t tlb_hits_{0};
    std::size_t tlb_misses_{0};
    std::size_t page_walks_{0};
    std::size_t translation_faults_{0};
};

} // namespace memory
