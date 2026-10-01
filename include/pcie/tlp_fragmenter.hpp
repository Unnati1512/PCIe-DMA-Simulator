#pragma once

#include "pcie/tlp.hpp"

#include <cstddef>
#include <vector>

namespace pcie {

class TlpFragmenter {
public:
    TlpFragmenter(
        std::size_t max_payload_size,
        std::size_t max_read_request_size
    );

    std::vector<Tlp> fragment_memory_write(
        RequestId requester_id,
        Address address,
        const std::vector<Byte>& payload,
        Tag starting_tag = 0
    ) const;

    std::vector<Tlp> fragment_memory_read(
        RequestId requester_id,
        Address address,
        std::size_t length,
        Tag starting_tag = 0
    ) const;

    std::size_t max_payload_size() const;

    std::size_t max_read_request_size() const;

private:
    std::size_t max_payload_size_;
    std::size_t max_read_request_size_;

    static std::size_t bytes_until_4k_boundary(
        Address address
    );

    static std::size_t fragment_size(
        Address address,
        std::size_t remaining,
        std::size_t maximum_size
    );
};

} // namespace pcie
