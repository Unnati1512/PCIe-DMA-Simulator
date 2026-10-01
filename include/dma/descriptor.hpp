#pragma once

#include <cstddef>
#include <cstdint>

namespace dma {

using Address = std::uint64_t;

enum class DescriptorControl : std::uint32_t {
    None = 0,
    Own = 1u << 0,
    Interrupt = 1u << 1
};

enum class DescriptorStatus : std::uint32_t {
    Free = 0,
    Ready,
    InProgress,
    Completed,
    Error
};

struct Descriptor {
    Address source_address;
    Address destination_address;

    std::size_t length;

    DescriptorControl control;
    DescriptorStatus status;

    std::size_t next;
};

} // namespace dma
