#include "pcie/transaction_engine.hpp"

#include <iostream>
#include <stdexcept>

namespace pcie {

TransactionEngine::TransactionEngine(
    sim::Simulator& simulator,
    RequestId requester_id,
    std::size_t max_outstanding
)
    : simulator_(simulator),
      requester_id_(requester_id),
      tracker_(max_outstanding) {
}

Tlp TransactionEngine::issue_memory_read(
    Address address,
    std::size_t length,
    Tag tag
) {
    if (!tracker_.can_issue()) {
        throw std::runtime_error(
            "Maximum outstanding request limit reached"
        );
    }

    const Tag allocated_tag = tracker_.allocate_tag();

    if (allocated_tag != tag) {
        tracker_.release_tag(allocated_tag);

        throw std::invalid_argument(
            "Requested PCIe tag is not the next available tag; "
            "use issue_tracked_memory_read() to allocate a tag automatically"
        );
    }

    try {
        Tlp tlp = generator_.create_memory_read(
            requester_id_,
            address,
            length,
            tag
        );

        tracker_.track_request(tag, tlp, simulator_.now());
        ++requests_issued_;

        std::cout << "Cycle " << simulator_.now()
                  << ": PCIe MRd issued address=0x"
                  << std::hex << address << std::dec
                  << " length=" << length
                  << " tag=" << static_cast<unsigned>(tag)
                  << '\n';

        return tlp;
    } catch (...) {
        if (!tracker_.contains(tag)) {
            try {
                tracker_.release_tag(tag);
            } catch (...) {
                // Preserve the original exception.
            }
        }
        throw;
    }
}

Tlp TransactionEngine::issue_memory_write(
    Address address,
    const std::vector<Byte>& payload,
    Tag tag
) {
    if (!tracker_.can_issue()) {
        throw std::runtime_error(
            "Maximum outstanding request limit reached"
        );
    }

    const Tag allocated_tag = tracker_.allocate_tag();

    if (allocated_tag != tag) {
        tracker_.release_tag(allocated_tag);

        throw std::invalid_argument(
            "Requested PCIe tag is not the next available tag; "
            "use issue_tracked_memory_write() to allocate a tag automatically"
        );
    }

    try {
        Tlp tlp = generator_.create_memory_write(
            requester_id_,
            address,
            payload,
            tag
        );

        tracker_.track_request(tag, tlp, simulator_.now());
        ++requests_issued_;

        std::cout << "Cycle " << simulator_.now()
                  << ": PCIe MWr issued address=0x"
                  << std::hex << address << std::dec
                  << " length=" << payload.size()
                  << " tag=" << static_cast<unsigned>(tag)
                  << '\n';

        return tlp;
    } catch (...) {
        if (!tracker_.contains(tag)) {
            try {
                tracker_.release_tag(tag);
            } catch (...) {
                // Preserve the original exception.
            }
        }
        throw;
    }
}

Tlp TransactionEngine::issue_tracked_memory_read(
    Address address,
    std::size_t length
) {
    if (!tracker_.can_issue()) {
        throw std::runtime_error(
            "Maximum outstanding request limit reached"
        );
    }

    const Tag tag = tracker_.allocate_tag();

    try {
        Tlp tlp = generator_.create_memory_read(
            requester_id_,
            address,
            length,
            tag
        );

        tracker_.track_request(tag, tlp, simulator_.now());
        ++requests_issued_;

        std::cout << "Cycle " << simulator_.now()
                  << ": PCIe MRd issued address=0x"
                  << std::hex << address << std::dec
                  << " length=" << length
                  << " tag=" << static_cast<unsigned>(tag)
                  << '\n';

        return tlp;
    } catch (...) {
        if (!tracker_.contains(tag)) {
            try {
                tracker_.release_tag(tag);
            } catch (...) {
                // Preserve the original exception.
            }
        }
        throw;
    }
}

Tlp TransactionEngine::issue_tracked_memory_write(
    Address address,
    const std::vector<Byte>& payload
) {
    if (!tracker_.can_issue()) {
        throw std::runtime_error(
            "Maximum outstanding request limit reached"
        );
    }

    const Tag tag = tracker_.allocate_tag();

    try {
        Tlp tlp = generator_.create_memory_write(
            requester_id_,
            address,
            payload,
            tag
        );

        tracker_.track_request(tag, tlp, simulator_.now());
        ++requests_issued_;

        std::cout << "Cycle " << simulator_.now()
                  << ": PCIe MWr issued address=0x"
                  << std::hex << address << std::dec
                  << " length=" << payload.size()
                  << " tag=" << static_cast<unsigned>(tag)
                  << '\n';

        return tlp;
    } catch (...) {
        if (!tracker_.contains(tag)) {
            try {
                tracker_.release_tag(tag);
            } catch (...) {
                // Preserve the original exception.
            }
        }
        throw;
    }
}

Tlp TransactionEngine::complete_read_request(Tag tag) {
    const Tlp request = tracker_.complete_request(tag);

    if (request.type != TlpType::MemoryRead) {
        throw std::invalid_argument(
            "Read completion received for a non-read request"
        );
    }

    return request;
}

void TransactionEngine::retire_posted_write(Tag tag) {
    const Tlp request = tracker_.complete_request(tag);

    if (request.type != TlpType::MemoryWrite) {
        throw std::invalid_argument(
            "Posted-write retirement used for a non-write request"
        );
    }
}

bool TransactionEngine::has_outstanding_request(Tag tag) const {
    return tracker_.contains(tag);
}

std::size_t TransactionEngine::outstanding_count() const {
    return tracker_.outstanding_count();
}

std::size_t TransactionEngine::requests_issued() const {
    return requests_issued_;
}

std::size_t TransactionEngine::requests_completed() const {
    return tracker_.requests_completed();
}

std::size_t TransactionEngine::unknown_completions() const {
    return tracker_.unknown_completions();
}

} // namespace pcie
