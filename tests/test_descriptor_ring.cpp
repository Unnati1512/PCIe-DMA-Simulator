#include "dma/descriptor_ring.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>

int main() {
    dma::DescriptorRing ring(2);

    dma::Descriptor descriptor{
        0x1000,
        0x8000,
        4096,
        dma::DescriptorControl::Own,
        dma::DescriptorStatus::Free,
        0
    };

    // A submitted descriptor should become Ready.
    assert(ring.submit(descriptor));
    assert(!ring.empty());
    assert(ring.front().status == dma::DescriptorStatus::Ready);

    // Consuming an errored descriptor must preserve its Error status.
    ring.front().status = dma::DescriptorStatus::Error;
    ring.consume();

    assert(ring.empty());
    assert(ring.at(0).status == dma::DescriptorStatus::Error);

    // The ring should still support successful descriptors.
    assert(ring.submit(descriptor));
    assert(ring.front().status == dma::DescriptorStatus::Ready);

    ring.front().status = dma::DescriptorStatus::Completed;
    ring.consume();

    assert(ring.empty());
    assert(ring.at(1).status == dma::DescriptorStatus::Completed);

    // Consuming an empty ring must still throw.
    bool threw = false;

    try {
        ring.consume();
    } catch (const std::out_of_range&) {
        threw = true;
    }

    assert(threw);

    std::cout << "Descriptor ring regression tests passed.\n";
    return 0;
}
