#include "dma/scatter_gather_engine.hpp"

#include <vector>

namespace dma {

ScatterGatherEngine::ScatterGatherEngine(
    const pcie::TlpFragmenter& fragmenter
)
    : fragmenter_(fragmenter) {
}

std::vector<pcie::Tlp>
ScatterGatherEngine::generate_memory_writes(
    pcie::RequestId requester_id,
    const ScatterGatherList& list,
    pcie::Tag starting_tag
) const {
    std::vector<pcie::Tlp> all_fragments;

    pcie::Tag tag = starting_tag;

    for (const auto& segment : list.segments()) {
        std::vector<pcie::Byte> payload(
            segment.length,
            0xAA
        );

        const auto fragments =
            fragmenter_.fragment_memory_write(
                requester_id,
                segment.address,
                payload,
                tag
            );

        for (const auto& tlp : fragments) {
            all_fragments.push_back(tlp);
            ++tag;
        }
    }

    return all_fragments;
}

std::size_t ScatterGatherEngine::total_bytes(
    const ScatterGatherList& list
) const {
    return list.total_length();
}

} // namespace dma
