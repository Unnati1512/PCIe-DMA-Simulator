#include "pcie/tlp_fragmenter.hpp"

#include <stdexcept>

namespace pcie {

TlpFragmenter::TlpFragmenter(
    std::size_t max_payload_size,
    std::size_t max_read_request_size
)
    : max_payload_size_(max_payload_size),
      max_read_request_size_(max_read_request_size) {

    if (max_payload_size_ == 0) {
        throw std::invalid_argument(
            "MPS must be greater than zero"
        );
    }

    if (max_read_request_size_ == 0) {
        throw std::invalid_argument(
            "MRRS must be greater than zero"
        );
    }
}

std::size_t TlpFragmenter::bytes_until_4k_boundary(
    Address address
) {
    constexpr std::size_t boundary = 4096;

    const std::size_t offset =
        static_cast<std::size_t>(
            address % boundary
        );

    return boundary - offset;
}

std::size_t TlpFragmenter::fragment_size(
    Address address,
    std::size_t remaining,
    std::size_t maximum_size
) {
    const std::size_t boundary_remaining =
        bytes_until_4k_boundary(address);

    if (remaining < maximum_size &&
        remaining < boundary_remaining) {
        return remaining;
    }

    if (maximum_size < boundary_remaining) {
        return maximum_size;
    }

    return boundary_remaining;
}

std::vector<Tlp> TlpFragmenter::fragment_memory_write(
    RequestId requester_id,
    Address address,
    const std::vector<Byte>& payload,
    Tag starting_tag
) const {
    if (payload.empty()) {
        throw std::invalid_argument(
            "Cannot fragment an empty Memory Write"
        );
    }

    std::vector<Tlp> fragments;

    std::size_t offset = 0;
    Tag tag = starting_tag;

    while (offset < payload.size()) {
        const std::size_t remaining =
            payload.size() - offset;

        const std::size_t length =
            fragment_size(
                address,
                remaining,
                max_payload_size_
            );

        std::vector<Byte> fragment_payload(
            payload.begin() + offset,
            payload.begin() + offset + length
        );

        fragments.push_back(
            Tlp{
                TlpType::MemoryWrite,
                requester_id,
                address,
                length,
                tag,
                0xF,
                0xF,
                fragment_payload
            }
        );

        address += length;
        offset += length;
        ++tag;
    }

    return fragments;
}

std::vector<Tlp> TlpFragmenter::fragment_memory_read(
    RequestId requester_id,
    Address address,
    std::size_t length,
    Tag starting_tag
) const {
    if (length == 0) {
        throw std::invalid_argument(
            "Cannot fragment a zero-length Memory Read"
        );
    }

    std::vector<Tlp> fragments;

    std::size_t remaining = length;
    Tag tag = starting_tag;

    while (remaining > 0) {
        const std::size_t fragment_length =
            fragment_size(
                address,
                remaining,
                max_read_request_size_
            );

        fragments.push_back(
            Tlp{
                TlpType::MemoryRead,
                requester_id,
                address,
                fragment_length,
                tag,
                0xF,
                0xF,
                {}
            }
        );

        address += fragment_length;
        remaining -= fragment_length;
        ++tag;
    }

    return fragments;
}

std::size_t TlpFragmenter::max_payload_size() const {
    return max_payload_size_;
}

std::size_t TlpFragmenter::max_read_request_size() const {
    return max_read_request_size_;
}

} // namespace pcie
