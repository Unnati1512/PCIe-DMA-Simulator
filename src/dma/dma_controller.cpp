#include "dma/dma_controller.hpp"

#include <iostream>

namespace dma {

DmaController::DmaController(
    sim::Simulator& simulator,
    DescriptorRing& descriptor_ring
)
    : simulator_(simulator),
      descriptor_ring_(descriptor_ring) {
}

void DmaController::start() {
    transition_to(DmaState::IDLE);
}

DmaState DmaController::state() const {
    return state_;
}

std::size_t DmaController::descriptors_completed() const {
    return descriptors_completed_;
}

void DmaController::transition_to(DmaState next_state) {
    state_ = next_state;

    simulator_.schedule(
        simulator_.now(),
        [this]() {
            switch (state_) {
                case DmaState::IDLE:
                    handle_idle();
                    break;

                case DmaState::FETCH_DESCRIPTOR:
                    handle_fetch_descriptor();
                    break;

                case DmaState::TRANSLATE_ADDRESS:
                    handle_translate_address();
                    break;

                case DmaState::GENERATE_REQUEST:
                    handle_generate_request();
                    break;

                case DmaState::WAIT_COMPLETION:
                    handle_wait_completion();
                    break;

                case DmaState::UPDATE_DESCRIPTOR:
                    handle_update_descriptor();
                    break;

                case DmaState::DONE:
                    handle_done();
                    break;

                case DmaState::ERROR:
                    handle_error();
                    break;
            }
        }
    );
}

void DmaController::handle_idle() {
    std::cout << "Cycle "
              << simulator_.now()
              << ": DMA FSM -> IDLE\n";

    if (!descriptor_ring_.empty()) {
        transition_to(DmaState::FETCH_DESCRIPTOR);
    }
}

void DmaController::handle_fetch_descriptor() {
    std::cout << "Cycle "
              << simulator_.now()
              << ": DMA FSM -> FETCH_DESCRIPTOR\n";

    if (descriptor_ring_.empty()) {
        transition_to(DmaState::IDLE);
        return;
    }

    current_descriptor_ = descriptor_ring_.front();

    descriptor_ring_.front().status =
        DescriptorStatus::InProgress;

    std::cout << "  Fetched descriptor\n";
    std::cout << "  Source: 0x"
              << std::hex
              << current_descriptor_.source_address
              << '\n';

    std::cout << "  Destination: 0x"
              << current_descriptor_.destination_address
              << '\n';

    std::cout << std::dec
              << "  Length: "
              << current_descriptor_.length
              << " bytes\n";

    transition_to(DmaState::TRANSLATE_ADDRESS);
}

void DmaController::handle_translate_address() {
    std::cout << "Cycle "
              << simulator_.now()
              << ": DMA FSM -> TRANSLATE_ADDRESS\n";

    /*
     * Day 4 uses identity translation:
     *
     * IOVA == physical address
     *
     * The real IOMMU/TLB subsystem will be implemented
     * on Day 9.
     */

    std::cout << "  Source address translated: 0x"
              << std::hex
              << current_descriptor_.source_address
              << '\n';

    std::cout << "  Destination address translated: 0x"
              << current_descriptor_.destination_address
              << '\n';

    std::cout << std::dec;

    transition_to(DmaState::GENERATE_REQUEST);
}

void DmaController::handle_generate_request() {
    std::cout << "Cycle "
              << simulator_.now()
              << ": DMA FSM -> GENERATE_REQUEST\n";

    std::cout << "  DMA request generated\n";

    transition_to(DmaState::WAIT_COMPLETION);
}

void DmaController::handle_wait_completion() {
    std::cout << "Cycle "
              << simulator_.now()
              << ": DMA FSM -> WAIT_COMPLETION\n";

    /*
     * Day 4 uses a simulated completion event.
     *
     * Real PCIe completion handling will be implemented
     * when the PCIe transaction model is added.
     */

    simulator_.schedule(
        simulator_.now() + 20,
        [this]() {
            std::cout << "Cycle "
                      << simulator_.now()
                      << ": DMA request completion received\n";

            transition_to(DmaState::UPDATE_DESCRIPTOR);
        }
    );
}

void DmaController::handle_update_descriptor() {
    std::cout << "Cycle "
              << simulator_.now()
              << ": DMA FSM -> UPDATE_DESCRIPTOR\n";

    descriptor_ring_.consume();

    ++descriptors_completed_;

    transition_to(DmaState::DONE);
}

void DmaController::handle_done() {
    std::cout << "Cycle "
              << simulator_.now()
              << ": DMA FSM -> DONE\n";

    transition_to(DmaState::IDLE);
}

void DmaController::handle_error() {
    std::cout << "Cycle "
              << simulator_.now()
              << ": DMA FSM -> ERROR\n";
}

} // namespace dma
