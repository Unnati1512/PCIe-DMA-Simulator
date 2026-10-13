#pragma once

#include "pcie/outstanding_requests.hpp"
#include "pcie/tlp.hpp"
#include "pcie/tlp_generator.hpp"
#include "simulator/simulator.hpp"

#include <cstddef>

namespace pcie {

class TransactionEngine {
public:
    TransactionEngine(
        sim::Simulator& simulator,
        RequestId requester_id,
        std::size_t max_outstanding = 32
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

    Tlp issue_tracked_memory_read(
        Address address,
        std::size_t length
    );

    Tlp issue_tracked_memory_write(
        Address address,
        const std::vector<Byte>& payload
    );

    Tlp complete_read_request(Tag tag);

    void retire_posted_write(Tag tag);

    bool has_outstanding_request(Tag tag) const;

    std::size_t outstanding_count() const;

    std::size_t requests_issued() const;

    std::size_t requests_completed() const;

    std::size_t unknown_completions() const;

private:
    sim::Simulator& simulator_;

    RequestId requester_id_;

    TlpGenerator generator_;

    OutstandingRequestTracker tracker_;

    std::size_t requests_issued_{0};
};

} // namespace pcie
