#include "dma/descriptor_validator.hpp"

#include <cstdint>
#include <limits>

namespace dma {

bool validate_descriptor(const Descriptor& descriptor) {
    // A zero-length transfer is invalid.
    if (descriptor.length == 0) {
        return false;
    }

    // Reject identical starting addresses.
    if (descriptor.source_address ==
        descriptor.destination_address) {
        return false;
    }

    // Ensure the transfer length can be represented as an Address.
    if (descriptor.length >
        static_cast<std::uint64_t>(
            std::numeric_limits<Address>::max())) {
        return false;
    }

    const Address length =
        static_cast<Address>(descriptor.length);

    // Reject ranges whose final byte address would overflow.
    if (length - 1 >
        std::numeric_limits<Address>::max()
            - descriptor.source_address) {
        return false;
    }

    if (length - 1 >
        std::numeric_limits<Address>::max()
            - descriptor.destination_address) {
        return false;
    }

    // Only the currently defined control bits are supported.
    const auto control =
        static_cast<std::uint32_t>(descriptor.control);

    constexpr std::uint32_t allowed_control_bits = 0x3U;

    if ((control & ~allowed_control_bits) != 0) {
        return false;
    }

    return true;
}

} // namespace dma
