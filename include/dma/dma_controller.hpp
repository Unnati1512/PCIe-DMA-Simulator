#pragma once

#include "dma/descriptor_ring.hpp"
#include "memory/iommu.hpp"
#include "memory/memory.hpp"
#include "pcie/link.hpp"
#include "pcie/transaction_engine.hpp"
#include "simulator/simulator.hpp"

#include <cstddef>
#include <cstdint>

namespace dma {

enum class DmaState {
    IDLE,
    FETCH_DESCRIPTOR,
    TRANSLATE_ADDRESS,
    GENERATE_REQUEST,
    WAIT_COMPLETION,
    UPDATE_DESCRIPTOR,
    DONE,
    ERROR
};

class DmaController {
public:
    DmaController(
        sim::Simulator& simulator,
        DescriptorRing& descriptor_ring,
        memory::Memory& host_memory,
        memory::Iommu& iommu,
        pcie::PcieLink& pcie_link,
        pcie::TransactionEngine& transaction_engine
    );

    void start();

    DmaState state() const;

    std::size_t descriptors_completed() const;

private:
    void transition_to(DmaState next_state);

    void handle_idle();
    void handle_fetch_descriptor();
    void handle_translate_address();
    void handle_generate_request();
    void handle_wait_completion();
    void handle_update_descriptor();
    void handle_done();
    void handle_error();

    sim::Simulator& simulator_;
    DescriptorRing& descriptor_ring_;
    memory::Memory& host_memory_;
    memory::Iommu& iommu_;
    pcie::PcieLink& pcie_link_;
    pcie::TransactionEngine& transaction_engine_;

    DmaState state_{DmaState::IDLE};
    std::size_t descriptors_completed_{0};

    Descriptor current_descriptor_{};

    memory::Address source_physical_address_{0};
    memory::Address destination_physical_address_{0};
};

} // namespace dma
