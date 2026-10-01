#include "dma/descriptor_ring.hpp"

#include <stdexcept>

namespace dma {

DescriptorRing::DescriptorRing(std::size_t capacity)
    : descriptors_(capacity) {
    if (capacity == 0) {
        throw std::invalid_argument(
            "Descriptor ring capacity must be greater than zero"
        );
    }

    for (std::size_t i = 0; i < capacity; ++i) {
        descriptors_[i].source_address = 0;
        descriptors_[i].destination_address = 0;
        descriptors_[i].length = 0;
        descriptors_[i].control = DescriptorControl::None;
        descriptors_[i].status = DescriptorStatus::Free;
        descriptors_[i].next = (i + 1) % capacity;
    }
}

std::size_t DescriptorRing::capacity() const {
    return descriptors_.size();
}

std::size_t DescriptorRing::head() const {
    return head_;
}

std::size_t DescriptorRing::tail() const {
    return tail_;
}

bool DescriptorRing::empty() const {
    return count_ == 0;
}

bool DescriptorRing::full() const {
    return count_ == descriptors_.size();
}

bool DescriptorRing::submit(const Descriptor& descriptor) {
    if (full()) {
        return false;
    }
    if (!validate_descriptor(descriptor)) {
        return false;
    }

    Descriptor submitted = descriptor;

    submitted.status = DescriptorStatus::Ready;
    submitted.next = (tail_ + 1) % descriptors_.size();

    descriptors_[tail_] = submitted;

    tail_ = (tail_ + 1) % descriptors_.size();
    ++count_;

    return true;
}

Descriptor& DescriptorRing::front() {
    if (empty()) {
        throw std::out_of_range(
            "Cannot access front of an empty descriptor ring"
        );
    }

    return descriptors_[head_];
}

const Descriptor& DescriptorRing::front() const {
    if (empty()) {
        throw std::out_of_range(
            "Cannot access front of an empty descriptor ring"
        );
    }

    return descriptors_[head_];
}

void DescriptorRing::consume() {
    if (empty()) {
        throw std::out_of_range(
            "Cannot consume from an empty descriptor ring"
        );
    }

    descriptors_[head_].status = DescriptorStatus::Completed;

    head_ = (head_ + 1) % descriptors_.size();
    --count_;
}

Descriptor& DescriptorRing::at(std::size_t index) {
    if (index >= descriptors_.size()) {
        throw std::out_of_range(
            "Descriptor index is outside the ring"
        );
    }

    return descriptors_[index];
}

const Descriptor& DescriptorRing::at(std::size_t index) const {
    if (index >= descriptors_.size()) {
        throw std::out_of_range(
            "Descriptor index is outside the ring"
        );
    }

    return descriptors_[index];
}

} // namespace dma
