#include "dma/descriptor_validator.hpp"

namespace dma {

bool validate_descriptor(const Descriptor& descriptor) {
    if (descriptor.length == 0) {
        return false;
    }

    if (descriptor.source_address ==
        descriptor.destination_address) {
        return false;
    }

    return true;
}

} // namespace dma
