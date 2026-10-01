#include <iostream>

#include "dma/dma_controller.hpp"
#include "dma/descriptor_ring.hpp"
#include "simulator/simulator.hpp"

int main() {
    sim::Simulator simulator;

    dma::DescriptorRing ring(4);

    dma::Descriptor descriptor{
        0x1000,
        0x8000,
        4096,
        dma::DescriptorControl::Own,
        dma::DescriptorStatus::Free,
        0
    };

    std::cout << "Starting PCIe DMA simulation\n";

    if (!ring.submit(descriptor)) {
        std::cerr << "Failed to submit DMA descriptor\n";
        return 1;
    }

    std::cout << "Descriptor submitted to DMA ring\n";

    dma::DmaController controller(
        simulator,
        ring
    );

    controller.start();

    simulator.run();

    std::cout << "\nSimulation finished at cycle "
              << simulator.now()
              << '\n';

    std::cout << "Descriptors completed: "
              << controller.descriptors_completed()
              << '\n';

    return 0;
}
