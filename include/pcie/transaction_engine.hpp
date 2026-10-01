#pragma once

#include "pcie/tlp.hpp"
#include "pcie/tlp_generator.hpp"
#include "simulator/simulator.hpp"

#include <cstddef>

namespace pcie {

class TransactionEngine {
public:
    TransactionEngine(
        sim::Simulator& simulator,
        RequestId requester_id
    );

    Tlp issue_memory_read(
        Address address,
        std::size_t length,
        Tag tag
    );

    Tlp issue_memory_write(
        Address address,
        const std::vector<Byte>& payload,
        Tag tag
    );

    std::size_t requests_issued() const;

private:
    sim::Simulator& simulator_;

    RequestId requester_id_;

    TlpGenerator generator_;

    std::size_t requests_issued_{0};
};

} // namespace pcie
