#include "dma/dma_controller.hpp"
#include "dma/descriptor_ring.hpp"
#include "memory/iommu.hpp"
#include "memory/memory.hpp"
#include "pcie/link.hpp"
#include "pcie/transaction_engine.hpp"
#include "simulator/simulator.hpp"

#include <cstddef>
#include <iostream>
#include <vector>

int main() {
    constexpr memory::Address source_address = 0x1000;
    constexpr memory::Address destination_address = 0x8000;
    constexpr std::size_t transfer_size = 4096;

    sim::Simulator simulator;

    // Simulated host memory, covering both source and destination.
    memory::Memory host_memory(
        0x1000,
        0x10000,
        5
    );

    // Explicit IOVA-to-physical mappings.
    memory::Iommu iommu;
    iommu.map_page(source_address, source_address);
    iommu.map_page(destination_address, destination_address);

    // Give the source a deterministic, non-zero byte pattern.
    std::vector<memory::Byte> source_data(transfer_size);

    for (std::size_t i = 0; i < transfer_size; ++i) {
        source_data[i] =
            static_cast<memory::Byte>((i * 37 + 11) & 0xFF);
    }

    host_memory.write(source_address, source_data);
    host_memory.write(
        destination_address,
        std::vector<memory::Byte>(transfer_size, 0)
    );

    pcie::PcieLink pcie_link(
        simulator,
        pcie::PcieGeneration::Gen4,
        16,
        10
    );

    pcie::TransactionEngine transaction_engine(
        simulator,
        0x0100
    );

    std::cout << "PCIe Link: Gen4 x16\n"
              << "Theoretical bandwidth: "
              << pcie_link.theoretical_bandwidth_gbps()
              << " GB/s\n"
              << "Starting end-to-end DMA transfer\n"
              << "Transfer size: " << transfer_size << " bytes\n\n";

    dma::DescriptorRing descriptor_ring(4);

    const dma::Descriptor descriptor{
        source_address,
        destination_address,
        transfer_size,
        dma::DescriptorControl::Own,
        dma::DescriptorStatus::Free,
        0
    };

    if (!descriptor_ring.submit(descriptor)) {
        std::cerr << "FAIL: could not submit DMA descriptor.\n";
        return 1;
    }

    dma::DmaController dma_controller(
        simulator,
        descriptor_ring,
        host_memory,
        iommu,
        pcie_link,
        transaction_engine
    );

    dma_controller.start();
    simulator.run();

    const auto destination_data =
        host_memory.read(destination_address, transfer_size);

    std::cout << "\n========== VERIFICATION ==========\n"
              << "Simulation finished at cycle: "
              << simulator.now() << '\n'
              << "Descriptors completed: "
              << dma_controller.descriptors_completed() << '\n'
              << "PCIe TLP transmissions: "
              << pcie_link.transmissions() << '\n'
              << "PCIe bytes modelled: "
              << pcie_link.bytes_transmitted() << '\n'
              << "IOMMU TLB hits: " << iommu.tlb_hits() << '\n'
              << "IOMMU TLB misses: " << iommu.tlb_misses() << '\n';

    if (source_data != destination_data) {
        std::cerr << "\nFAIL: source and destination data differ.\n";
        return 1;
    }

    if (dma_controller.descriptors_completed() != 1) {
        std::cerr << "\nFAIL: descriptor did not complete correctly.\n";
        return 1;
    }

    std::cout << "\nPASS: all " << transfer_size
              << " destination bytes match the source.\n";

    return 0;
}
