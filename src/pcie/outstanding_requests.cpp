#include "pcie/outstanding_requests.hpp"

#include <stdexcept>
#include <string>

namespace pcie {

namespace {
constexpr std::size_t TAG_SPACE_SIZE = 256;
}

// ============================================================
// Constructor
// ============================================================

OutstandingRequestTracker::OutstandingRequestTracker(
    std::size_t max_outstanding)
    : max_outstanding_(max_outstanding),
      tag_in_use_(TAG_SPACE_SIZE, false) {

    if (max_outstanding_ == 0) {
        throw std::invalid_argument(
            "Maximum outstanding requests must be greater than zero.");
    }

    if (max_outstanding_ > TAG_SPACE_SIZE) {
        throw std::invalid_argument(
            "Maximum outstanding requests cannot exceed 256.");
    }
}

// ============================================================
// Check whether another request can be issued
// ============================================================

bool OutstandingRequestTracker::can_issue() const {
    return requests_.size() < max_outstanding_;
}

// ============================================================
// Allocate an unused PCIe tag
// ============================================================

Tag OutstandingRequestTracker::allocate_tag() {
    if (!can_issue()) {
        throw std::runtime_error(
            "Cannot allocate tag: maximum outstanding requests reached.");
    }

    for (std::size_t i = 0; i < TAG_SPACE_SIZE; ++i) {
        const std::size_t candidate =
            (next_tag_ + i) % TAG_SPACE_SIZE;

        if (!tag_in_use_[candidate]) {
            tag_in_use_[candidate] = true;

            next_tag_ = (candidate + 1) % TAG_SPACE_SIZE;

            return static_cast<Tag>(candidate);
        }
    }

    throw std::runtime_error(
        "Cannot allocate tag: no free PCIe tags available.");
}

// ============================================================
// Release an allocated PCIe tag
// ============================================================

void OutstandingRequestTracker::release_tag(Tag tag) {
    const std::size_t index = static_cast<std::size_t>(tag);

    if (index >= tag_in_use_.size()) {
        throw std::out_of_range(
            "Cannot release tag: PCIe tag is out of range.");
    }

    if (!tag_in_use_[index]) {
        throw std::logic_error(
            "Cannot release tag: tag is not currently allocated.");
    }

    if (requests_.find(tag) != requests_.end()) {
        throw std::logic_error(
            "Cannot release tag: request is still outstanding.");
    }

    tag_in_use_[index] = false;
}

// ============================================================
// Track an issued request
// ============================================================

void OutstandingRequestTracker::track_request(
    Tag tag,
    const Tlp& request,
    sim::Cycle issue_cycle) {

    const std::size_t index = static_cast<std::size_t>(tag);

    if (index >= tag_in_use_.size()) {
        throw std::out_of_range(
            "Cannot track request: PCIe tag is out of range.");
    }

    if (!can_issue()) {
        throw std::runtime_error(
            "Cannot track request: maximum outstanding requests reached.");
    }

    if (!tag_in_use_[index]) {
        throw std::logic_error(
            "Cannot track request: tag has not been allocated.");
    }

    if (requests_.find(tag) != requests_.end()) {
        throw std::logic_error(
            "Cannot track request: tag already has an outstanding request.");
    }

    requests_.emplace(
        tag,
        OutstandingRequest{tag, request, issue_cycle});

    ++requests_issued_;
}

// ============================================================
// Complete an outstanding request
// ============================================================

Tlp OutstandingRequestTracker::complete_request(Tag tag) {
    auto it = requests_.find(tag);

    if (it == requests_.end()) {
        ++unknown_completions_;

        throw std::runtime_error(
            "Cannot complete request: unknown PCIe tag " +
            std::to_string(static_cast<unsigned int>(tag)) + ".");
    }

    Tlp original_request = it->second.request;

    requests_.erase(it);

    tag_in_use_[static_cast<std::size_t>(tag)] = false;

    ++requests_completed_;

    return original_request;
}

// ============================================================
// Check whether a request is outstanding
// ============================================================

bool OutstandingRequestTracker::contains(Tag tag) const {
    return requests_.find(tag) != requests_.end();
}

// ============================================================
// Number of outstanding requests
// ============================================================

std::size_t OutstandingRequestTracker::outstanding_count() const {
    return requests_.size();
}

// ============================================================
// Maximum permitted outstanding requests
// ============================================================

std::size_t OutstandingRequestTracker::max_outstanding() const {
    return max_outstanding_;
}

// ============================================================
// Total requests issued
// ============================================================

std::size_t OutstandingRequestTracker::requests_issued() const {
    return requests_issued_;
}

// ============================================================
// Total successfully completed requests
// ============================================================

std::size_t OutstandingRequestTracker::requests_completed() const {
    return requests_completed_;
}

// ============================================================
// Total attempts to complete unknown requests
// ============================================================

std::size_t OutstandingRequestTracker::unknown_completions() const {
    return unknown_completions_;
}

} // namespace pcie
