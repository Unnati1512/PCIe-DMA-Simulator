#include <iostream>

#include "simulator/simulator.hpp"

int main() {
    sim::Simulator simulator;

    std::cout << "Starting PCIe DMA simulation\n";

    simulator.schedule(10, [&]() {
        std::cout << "Cycle " << simulator.now()
                  << ": DMA request issued\n";
    });

    simulator.schedule(15, [&]() {
        std::cout << "Cycle " << simulator.now()
                  << ": PCIe request generated\n";
    });

    simulator.schedule(35, [&]() {
        std::cout << "Cycle " << simulator.now()
                  << ": Completion received\n";
    });

    simulator.schedule(40, [&]() {
        std::cout << "Cycle " << simulator.now()
                  << ": DMA transfer completed\n";
    });
    simulator.run();

    std::cout << "Simulation finished at cycle "
              << simulator.now() << '\n';

    return 0;
}
