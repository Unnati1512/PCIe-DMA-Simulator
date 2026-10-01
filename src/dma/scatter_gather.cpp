#include "dma/scatter_gather.hpp"

#include <stdexcept>

namespace dma {

void ScatterGatherList::add_segment(
    Address address,
    std::size_t length
) {
    if (length == 0) {
        throw std::invalid_argument(
            "Scatter-gather segment length must be greater than zero"
        );
    }

    segments_.push_back(
        ScatterGatherSegment{
            address,
            length
        }
    );
}

const std::vector<ScatterGatherSegment>&
ScatterGatherList::segments() const {
    return segments_;
}

std::size_t ScatterGatherList::total_length() const {
    std::size_t total = 0;

    for (const auto& segment : segments_) {
        total += segment.length;
    }

    return total;
}

std::size_t ScatterGatherList::segment_count() const {
    return segments_.size();
}

bool ScatterGatherList::empty() const {
    return segments_.empty();
}

} // namespace dma
