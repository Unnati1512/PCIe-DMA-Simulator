#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace pcie {

using Address = std::uint64_t;
using RequestId = std::uint16_t;
using Tag = std::uint8_t;
using Byte = std::uint8_t;

enum class TlpType {
    MemoryRead,
    MemoryWrite,
    Completion,
    CompletionWithData
};

struct Tlp {
    TlpType type;

    RequestId requester_id;

    Address address;

    std::size_t length;

    Tag tag;

    std::uint8_t first_byte_enable;

    std::uint8_t last_byte_enable;

    std::vector<Byte> payload;
};

} // namespace pcie
