#pragma once

#include "dma/scatter_gather.hpp"
#include "pcie/tlp.hpp"
#include "pcie/tlp_fragmenter.hpp"

#include <cstddef>
#include <vector>

namespace dma {

class ScatterGatherEngine {
public:
    explicit ScatterGatherEngine(
        const pcie::TlpFragmenter& fragmenter
    );

    std::vector<pcie::Tlp> generate_memory_writes(
        pcie::RequestId requester_id,
        const ScatterGatherList& list,
        pcie::Tag starting_tag = 0
    ) const;

    std::size_t total_bytes(
        const ScatterGatherList& list
    ) const;

private:
    const pcie::TlpFragmenter& fragmenter_;
};

} // namespace dma
