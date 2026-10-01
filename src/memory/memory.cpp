#include "memory/memory.hpp"

#include <stdexcept>
#include <utility>

namespace memory {

Memory::Memory(
    Address base_address,
    std::size_t size,
    sim::Cycle access_latency
)
    : base_address_(base_address),
      data_(size, 0),
      access_latency_(access_latency) {
}

bool Memory::contains(Address address, std::size_t length) const {
    if (address < base_address_) {
        return false;
    }

    const Address offset = address - base_address_;

    if (offset > data_.size()) {
        return false;
    }

    return length <=
           data_.size() - static_cast<std::size_t>(offset);
}

void Memory::write(
    Address address,
    const std::vector<Byte>& data
) {
    if (!contains(address, data.size())) {
        throw std::out_of_range(
            "Memory write is outside the memory region"
        );
    }

    const std::size_t offset =
        static_cast<std::size_t>(address - base_address_);

    for (std::size_t i = 0; i < data.size(); ++i) {
        data_[offset + i] = data[i];
    }
}

std::vector<Byte> Memory::read(
    Address address,
    std::size_t length
) const {
    if (!contains(address, length)) {
        throw std::out_of_range(
            "Memory read is outside the memory region"
        );
    }

    const std::size_t offset =
        static_cast<std::size_t>(address - base_address_);

    return std::vector<Byte>(
        data_.begin() + offset,
        data_.begin() + offset + length
    );
}

void Memory::schedule_write(
    sim::Simulator& simulator,
    Address address,
    const std::vector<Byte>& data,
    std::function<void()> completion
) {
    if (!contains(address, data.size())) {
        throw std::out_of_range(
            "Memory write is outside the memory region"
        );
    }

    simulator.schedule(
        simulator.now() + access_latency_,
        [this, address, data, completion = std::move(completion)]() {
            write(address, data);

            if (completion) {
                completion();
            }
        }
    );
}

void Memory::schedule_read(
    sim::Simulator& simulator,
    Address address,
    std::size_t length,
    std::function<void(std::vector<Byte>)> completion
) {
    if (!contains(address, length)) {
        throw std::out_of_range(
            "Memory read is outside the memory region"
        );
    }

    simulator.schedule(
        simulator.now() + access_latency_,
        [this, address, length, completion = std::move(completion)]() {
            std::vector<Byte> data = read(address, length);

            if (completion) {
                completion(std::move(data));
            }
        }
    );
}

Address Memory::base_address() const {
    return base_address_;
}

std::size_t Memory::size() const {
    return data_.size();
}

sim::Cycle Memory::access_latency() const {
    return access_latency_;
}

} // namespace memory
