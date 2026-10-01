#pragma once

#include "pcie/tlp.hpp"

#include <cstddef>
#include <vector>

namespace pcie {

class TlpGenerator {
public:
    Tlp create_memory_read(
        RequestId requester_id,
        Address address,
        std::size_t length,
        Tag tag
    ) const;

    Tlp create_memory_write(
        RequestId requester_id,
        Address address,
        const std::vector<Byte>& payload,
        Tag tag
    ) const;

    Tlp create_completion(
        RequestId requester_id,
        Tag tag
    ) const;

    Tlp create_completion_with_data(
        RequestId requester_id,
        Tag tag,
        const std::vector<Byte>& payload
    ) const;
};

} // namespace pcie
