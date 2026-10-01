#pragma once

#include "dma/descriptor.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace dma {

struct ScatterGatherSegment {
    Address address;
    std::size_t length;
};

class ScatterGatherList {
public:
    void add_segment(
        Address address,
        std::size_t length
    );

    const std::vector<ScatterGatherSegment>& segments() const;

    std::size_t total_length() const;

    std::size_t segment_count() const;

    bool empty() const;

private:
    std::vector<ScatterGatherSegment> segments_;
};

} // namespace dma
