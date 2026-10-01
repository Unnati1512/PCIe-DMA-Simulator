#pragma once

#include "simulator/event.hpp"
#include "simulator/simulator.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace memory {

using Address = std::uint64_t;
using Byte = std::uint8_t;

class Memory {
public:
    Memory(
        Address base_address,
        std::size_t size,
        sim::Cycle access_latency
    );

    void write(Address address, const std::vector<Byte>& data);

    std::vector<Byte> read(Address address, std::size_t length) const;

    void schedule_write(
        sim::Simulator& simulator,
        Address address,
        const std::vector<Byte>& data,
        std::function<void()> completion
    );

    void schedule_read(
        sim::Simulator& simulator,
        Address address,
        std::size_t length,
        std::function<void(std::vector<Byte>)> completion
    );

    bool contains(Address address, std::size_t length) const;

    Address base_address() const;

    std::size_t size() const;

    sim::Cycle access_latency() const;

private:
    Address base_address_;
    std::vector<Byte> data_;
    sim::Cycle access_latency_;
};

} // namespace memory
