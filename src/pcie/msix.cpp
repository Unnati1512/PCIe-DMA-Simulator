#include "pcie/msix.hpp"

#include <stdexcept>

namespace pcie {

MsixController::MsixController(
    std::size_t vector_count
)
    : vectors_(vector_count),
      handlers_(vector_count) {

    if (vector_count == 0) {
        throw std::invalid_argument(
            "MSI-X must contain at least one vector"
        );
    }

    for (std::size_t i = 0; i < vector_count; ++i) {
        vectors_[i].id =
            static_cast<VectorId>(i);
    }
}

void MsixController::register_handler(
    VectorId vector,
    std::function<void(VectorId, sim::Cycle)> handler
) {
    const std::size_t index =
        static_cast<std::size_t>(vector);

    if (index >= vectors_.size()) {
        throw std::out_of_range(
            "Invalid MSI-X vector"
        );
    }

    handlers_[index] = std::move(handler);
}

void MsixController::trigger(
    VectorId vector,
    sim::Cycle cycle
) {
    const std::size_t index =
        static_cast<std::size_t>(vector);

    if (index >= vectors_.size()) {
        throw std::out_of_range(
            "Invalid MSI-X vector"
        );
    }

    ++vectors_[index].interrupt_count;
    ++total_interrupts_;

    if (handlers_[index]) {
        handlers_[index](vector, cycle);
    }
}

std::size_t MsixController::vector_count() const {
    return vectors_.size();
}

std::size_t MsixController::interrupt_count() const {
    return total_interrupts_;
}

std::size_t MsixController::vector_interrupt_count(
    VectorId vector
) const {
    const std::size_t index =
        static_cast<std::size_t>(vector);

    if (index >= vectors_.size()) {
        throw std::out_of_range(
            "Invalid MSI-X vector"
        );
    }

    return vectors_[index].interrupt_count;
}

}
