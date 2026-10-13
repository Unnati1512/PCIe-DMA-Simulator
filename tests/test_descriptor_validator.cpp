#include "dma/descriptor_validator.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>

int main() {
    using namespace dma;

    Descriptor descriptor{
        0x1000,
        0x8000,
        4096,
        DescriptorControl::Own,
        DescriptorStatus::Free,
        0
    };

    assert(validate_descriptor(descriptor));

    auto invalid = descriptor;
    invalid.length = 0;
    assert(!validate_descriptor(invalid));

    invalid = descriptor;
    invalid.destination_address = invalid.source_address;
    assert(!validate_descriptor(invalid));

    invalid = descriptor;
    invalid.source_address =
        std::numeric_limits<Address>::max() - 2;
    invalid.length = 8;
    assert(!validate_descriptor(invalid));

    invalid = descriptor;
    invalid.destination_address =
        std::numeric_limits<Address>::max() - 2;
    invalid.length = 8;
    assert(!validate_descriptor(invalid));

    invalid = descriptor;
    invalid.control = static_cast<DescriptorControl>(0x4U);
    assert(!validate_descriptor(invalid));

    std::cout << "Descriptor validator tests passed.\n";
    return 0;
}
