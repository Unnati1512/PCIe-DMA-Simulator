#pragma once

#include "simulator/event.hpp"

#include <cstddef>
#include <cstdint>
#include <queue>
#include <vector>

namespace sim {
class Simulator {
public:
    void schedule(Cycle time, std::function<void()> callback);

    void run();

    void run_until(Cycle end_time);

    Cycle now() const;

    std::size_t pending_events() const;

private:
    Cycle current_cycle_{0};

    std::uint64_t next_sequence_{0};

    std::priority_queue<
        Event,
        std::vector<Event>,
        EventCompare
    > events_;
};

} // namespace sim
