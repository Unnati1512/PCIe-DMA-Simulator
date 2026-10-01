#include "pcie/tlp_generator.hpp"

#include <stdexcept>

namespace pcie {

Tlp TlpGenerator::create_memory_read(
    RequestId requester_id,
    Address address,
    std::size_t length,
    Tag tag
) const {
    if (length == 0) {
        throw std::invalid_argument(
            "Memory read length must be greater than zero"
        );
    }

    return Tlp{
        TlpType::MemoryRead,
        requester_id,
        address,
        length,
        tag,
        0xF,
        0xF,
        {}
    };
}

Tlp TlpGenerator::create_memory_write(
    RequestId requester_id,
    Address address,
    const std::vector<Byte>& payload,
    Tag tag
) const {
    if (payload.empty()) {
        throw std::invalid_argument(
            "Memory write payload must not be empty"
        );
    }

    return Tlp{
        TlpType::MemoryWrite,
        requester_id,
        address,
        payload.size(),
        tag,
        0xF,
        0xF,
        payload
    };
}

Tlp TlpGenerator::create_completion(
    RequestId requester_id,
    Tag tag
) const {
    return Tlp{
        TlpType::Completion,
        requester_id,
        0,
        0,
        tag,
        0,
        0,
        {}
    };
}

Tlp TlpGenerator::create_completion_with_data(
    RequestId requester_id,
    Tag tag,
    const std::vector<Byte>& payload
) const {
    if (payload.empty()) {
        throw std::invalid_argument(
            "Completion data must not be empty"
        );
    }

    return Tlp{
        TlpType::CompletionWithData,
        requester_id,
        0,
        payload.size(),
        tag,
        0,
        0,
        payload
    };
}

} // namespace pcie
