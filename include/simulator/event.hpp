#pragma once

#include <cstdint>
#include <functional>

namespace sim {

using Cycle = std::uint64_t;

struct Event {
    Cycle time;
    std::uint64_t sequence;
    std::function<void()> callback;
};

struct EventCompare {
    bool operator()(const Event& a, const Event& b) const {
        if (a.time != b.time) {
            return a.time > b.time;
        }

        return a.sequence > b.sequence;
    }
};

} // namespace sim
