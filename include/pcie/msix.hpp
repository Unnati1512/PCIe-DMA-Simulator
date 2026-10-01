#pragma once

#include "simulator/event.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace pcie {

using VectorId = std::uint16_t;

struct MsixVector {
    VectorId id;
    std::size_t interrupt_count{0};
};

class MsixController {
public:
    explicit MsixController(std::size_t vector_count);

    void register_handler(
        VectorId vector,
        std::function<void(VectorId, sim::Cycle)> handler
    );

    void trigger(
        VectorId vector,
        sim::Cycle cycle
    );

    std::size_t vector_count() const;

    std::size_t interrupt_count() const;

    std::size_t vector_interrupt_count(
        VectorId vector
    ) const;

private:
    std::vector<MsixVector> vectors_;

    std::vector<
        std::function<void(VectorId, sim::Cycle)>
    > handlers_;

    std::size_t total_interrupts_{0};
};

}
