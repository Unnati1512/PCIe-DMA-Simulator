#include "pcie/outstanding_requests.hpp"

#include <stdexcept>

namespace pcie {

namespace {

constexpr std::size_t TagCount = 256;

} // namespace

OutstandingRequestTracker::OutstandingRequestTracker(
    std::size_t max_outstanding
)
    : max_outstanding_(max_outstanding),
      tag_in_use_(TagCount, false) {

    if (max_outstanding_ == 0) {
        throw std::invalid_argument(
            "Maximum outstanding requests must be greater than zero"
        );
    }

    if (max_outstanding_ > TagCount) {
        throw std::invalid_argument(
            "Maximum outstanding requests exceeds available tags"
        );
    }
}

bool OutstandingRequestTracker::can_issue() const {
    return requests_.size() < max_outstanding_;
}

Tag OutstandingRequestTracker::allocate_tag() {
    if (!can_issue()) {
        throw std::runtime_error(
            "Maximum outstanding request limit reached"
        );
    }

    for (std::size_t i = 0; i < TagCount; ++i) {
        const std::size_t candidate =
            (next_tag_ + i) % TagCount;

        if (!tag_in_use_[candidate]) {
            tag_in_use_[candidate] = true;
            next_tag_ = (candidate + 1) % TagCount;

            return static_cast<Tag>(candidate);
        }
    }

    throw std::runtime_error(
        "No PCIe tags available"
    );
}

void OutstandingRequestTracker::track_request(
    Tag tag,
    const Tlp& request,
    sim::Cycle issue_cycle
) {
    const std::size_t tag_index =
        static_cast<std::size_t>(tag);

    if (tag_index >= TagCount) {
        throw std::invalid_argument(
            "Invalid PCIe tag"
        );
    }

    if (!tag_in_use_[tag_index]) {
        throw std::invalid_argument(
            "Tag was not allocated"
        );
    }

    if (requests_.find(tag) != requests_.end()) {
        throw std::invalid_argument(
            "Request already tracked for tag"
        );
    }

    requests_.emplace(
        tag,
        OutstandingRequest{
            tag,
            request,
            issue_cycle
        }
    );

    ++requests_issued_;
}

Tlp OutstandingRequestTracker::complete_request(
    Tag tag
) {
    const auto it = requests_.find(tag);

    if (it == requests_.end()) {
        ++unknown_completions_;

        throw std::runtime_error(
            "Completion received for unknown tag"
        );
    }

    Tlp request = it->second.request;

    const std::size_t tag_index =
        static_cast<std::size_t>(tag);

    tag_in_use_[tag_index] = false;

    requests_.erase(it);

    ++requests_completed_;

    return request;
}

bool OutstandingRequestTracker::contains(
    Tag tag
) const {
    return requests_.find(tag) != requests_.end();
}

std::size_t OutstandingRequestTracker::outstanding_count() const {
    return requests_.size();
}

std::size_t OutstandingRequestTracker::max_outstanding() const {
    return max_outstanding_;
}

std::size_t OutstandingRequestTracker::requests_issued() const {
    return requests_issued_;
}

std::size_t OutstandingRequestTracker::requests_completed() const {
    return requests_completed_;
}

std::size_t OutstandingRequestTracker::unknown_completions() const {
    return unknown_completions_;
}

} // namespace pcie
