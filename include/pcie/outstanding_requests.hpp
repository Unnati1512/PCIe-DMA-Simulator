#pragma once

#include "pcie/tlp.hpp"
#include "simulator/event.hpp"
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace pcie {

struct OutstandingRequest {
    Tag tag;
    Tlp request;
    sim::Cycle issue_cycle;
};

class OutstandingRequestTracker {
public:
    explicit OutstandingRequestTracker(
        std::size_t max_outstanding
    );

    bool can_issue() const;

    Tag allocate_tag();
    void release_tag(Tag tag);

    void track_request(
        Tag tag,
        const Tlp& request,
        sim::Cycle issue_cycle
    );

    Tlp complete_request(Tag tag);

    bool contains(Tag tag) const;

    std::size_t outstanding_count() const;

    std::size_t max_outstanding() const;

    std::size_t requests_issued() const;

    std::size_t requests_completed() const;

    std::size_t unknown_completions() const;

private:
    std::size_t max_outstanding_;

    std::unordered_map<Tag, OutstandingRequest> requests_;

    std::vector<bool> tag_in_use_;

    std::size_t next_tag_{0};

    std::size_t requests_issued_{0};

    std::size_t requests_completed_{0};

    std::size_t unknown_completions_{0};
};

} // namespace pcie
