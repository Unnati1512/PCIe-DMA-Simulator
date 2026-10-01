#include <iostream>

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

    if (ring.submit(descriptor)) {
        std::cout << "Cycle "
                  << simulator.now()
                  << ": DMA descriptor submitted\n";
    }

    simulator.schedule(10, [&]() {
        const dma::Descriptor& current = ring.front();

        std::cout << "Cycle "
                  << simulator.now()
                  << ": DMA descriptor ready\n";

        std::cout << "  Source: 0x"
                  << std::hex
                  << current.source_address
                  << '\n';

        std::cout << "  Destination: 0x"
                  << current.destination_address
                  << '\n';

        std::cout << std::dec
                  << "  Length: "
                  << current.length
                  << " bytes\n";
    });

    simulator.schedule(20, [&]() {
        ring.consume();

        std::cout << "Cycle "
                  << simulator.now()
                  << ": DMA descriptor completed\n";
    });

    simulator.run();

    std::cout << "Simulation finished at cycle "
              << simulator.now()
              << '\n';

    return 0;
}
