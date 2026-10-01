#include "simulator/simulator.hpp"

#include <stdexcept>

namespace sim {

void Simulator::schedule(Cycle time, std::function<void()> callback) {
    if (time < current_cycle_) {
        throw std::invalid_argument(
            "Cannot schedule an event in the past"
        );
    }

    events_.push(Event{
        time,
        next_sequence_++,
        std::move(callback)
    });
}
void Simulator::run() {
    while (!events_.empty()) {
        Event event = events_.top();
        events_.pop();

        current_cycle_ = event.time;

        event.callback();
    }
}

void Simulator::run_until(Cycle end_time) {
    if (end_time < current_cycle_) {
        throw std::invalid_argument(
            "Cannot run backwards in simulation time"
        );
    }
    while (!events_.empty() &&
           events_.top().time <= end_time) {

        Event event = events_.top();
        events_.pop();

        current_cycle_ = event.time;

        event.callback();
    }

    current_cycle_ = end_time;
}

Cycle Simulator::now() const {
    return current_cycle_;
}

std::size_t Simulator::pending_events() const {
    return events_.size();
}

} // namespace sim
