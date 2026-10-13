#include "dma/dma_controller.hpp"

#include <iostream>

namespace dma {

DmaController::DmaController(
    sim::Simulator& simulator,
    DescriptorRing& descriptor_ring,
    memory::Memory& host_memory,
    memory::Iommu& iommu,
    pcie::PcieLink& pcie_link,
    pcie::TransactionEngine& transaction_engine
)
    : simulator_(simulator),
      descriptor_ring_(descriptor_ring),
      host_memory_(host_memory),
      iommu_(iommu),
      pcie_link_(pcie_link),
      transaction_engine_(transaction_engine) {
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
        [this, next_state]() {
            switch (next_state) {
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
    std::cout << "Cycle " << simulator_.now()
              << ": DMA FSM -> IDLE\n";

    if (!descriptor_ring_.empty()) {
        transition_to(DmaState::FETCH_DESCRIPTOR);
    }
}

void DmaController::handle_fetch_descriptor() {
    if (descriptor_ring_.empty()) {
        transition_to(DmaState::IDLE);
        return;
    }

    current_descriptor_ = descriptor_ring_.front();

    descriptor_ring_.front().status =
        DescriptorStatus::InProgress;

    std::cout << "Cycle " << simulator_.now()
              << ": DMA FSM -> FETCH_DESCRIPTOR\n"
              << "  Source: 0x" << std::hex
              << current_descriptor_.source_address
              << "\n  Destination: 0x"
              << current_descriptor_.destination_address
              << std::dec
              << "\n  Length: "
              << current_descriptor_.length
              << " bytes\n";

    transition_to(DmaState::TRANSLATE_ADDRESS);
}

void DmaController::handle_translate_address() {
    std::cout << "Cycle " << simulator_.now()
              << ": DMA FSM -> TRANSLATE_ADDRESS\n";

    const std::size_t length = current_descriptor_.length;

    // This first integration milestone supports descriptors contained
    // within one 4 KiB IOMMU page.
    const bool source_crosses_page =
        current_descriptor_.source_address % memory::Iommu::PageSize
            + length > memory::Iommu::PageSize;

    const bool destination_crosses_page =
        current_descriptor_.destination_address % memory::Iommu::PageSize
            + length > memory::Iommu::PageSize;

    if (length == 0 || source_crosses_page ||
        destination_crosses_page) {
        std::cerr << "DMA ERROR: invalid length or descriptor crosses "
                     "a 4 KiB page boundary.\n";
        transition_to(DmaState::ERROR);
        return;
    }

    const auto source_result =
        iommu_.translate(current_descriptor_.source_address);

    const auto destination_result =
        iommu_.translate(current_descriptor_.destination_address);

    if (!source_result.success || !destination_result.success) {
        std::cerr << "DMA ERROR: IOMMU translation fault.\n";
        transition_to(DmaState::ERROR);
        return;
    }

    source_physical_address_ = source_result.physical_address;
    destination_physical_address_ =
        destination_result.physical_address;

    if (!host_memory_.contains(source_physical_address_, length) ||
        !host_memory_.contains(destination_physical_address_, length)) {
        std::cerr << "DMA ERROR: translated address is outside memory.\n";
        transition_to(DmaState::ERROR);
        return;
    }

    std::cout << "  Source IOVA translated to physical address: 0x"
              << std::hex << source_physical_address_
              << "\n  Destination IOVA translated to physical address: 0x"
              << destination_physical_address_
              << std::dec << '\n';

    transition_to(DmaState::GENERATE_REQUEST);
}

void DmaController::handle_generate_request() {
    std::cout << "Cycle " << simulator_.now()
              << ": DMA FSM -> GENERATE_REQUEST\n";

    std::cout << "  Generating PCIe Memory Read request.\n";

    transition_to(DmaState::WAIT_COMPLETION);
}

void DmaController::handle_wait_completion() {
    std::cout << "Cycle " << simulator_.now()
              << ": DMA FSM -> WAIT_COMPLETION\n";

    const auto read_request =
        transaction_engine_.issue_memory_read(
            source_physical_address_,
            current_descriptor_.length,
            0
        );

    pcie_link_.transmit(
        read_request,
        [this]() {
            host_memory_.schedule_read(
                simulator_,
                source_physical_address_,
                current_descriptor_.length,
                [this](std::vector<memory::Byte> payload) {
                    // Retire the read request when its data returns.
                    transaction_engine_.complete_read_request(0);

                    std::cout << "Cycle " << simulator_.now()
                              << ": Read data returned: "
                              << payload.size() << " bytes\n";

                    const auto write_request =
                        transaction_engine_.issue_memory_write(
                            destination_physical_address_,
                            payload,
                            1
                        );

                    pcie_link_.transmit(
                        write_request,
                        [this, payload = std::move(payload)]() {
                            host_memory_.schedule_write(
                                simulator_,
                                destination_physical_address_,
                                payload,
                                [this]() {
                                    // Retire the posted write after the
                                    // destination memory write completes.
                                    transaction_engine_.retire_posted_write(1);

                                    std::cout << "Cycle " << simulator_.now()
                                              << ": Destination memory write "
                                                 "completed.\n";

                                    transition_to(
                                        DmaState::UPDATE_DESCRIPTOR
                                    );
                                }
                            );
                        }
                    );
                }
            );
        }
    );
}

void DmaController::handle_update_descriptor() {
    std::cout << "Cycle " << simulator_.now()
              << ": DMA FSM -> UPDATE_DESCRIPTOR\n";

    if (descriptor_ring_.empty()) {
        transition_to(DmaState::ERROR);
        return;
    }

    descriptor_ring_.front().status = DescriptorStatus::Completed;
    descriptor_ring_.consume();

    ++descriptors_completed_;

    transition_to(DmaState::DONE);
}

void DmaController::handle_done() {
    std::cout << "Cycle " << simulator_.now()
              << ": DMA FSM -> DONE\n";

    transition_to(DmaState::IDLE);
}

void DmaController::handle_error() {
    std::cerr << "Cycle " << simulator_.now()
              << ": DMA FSM -> ERROR\n";

    if (!descriptor_ring_.empty()) {
        descriptor_ring_.front().status = DescriptorStatus::Error;
        descriptor_ring_.consume();
    }

    transition_to(DmaState::IDLE);
}

} // namespace dma
