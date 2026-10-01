#pragma once

#include "dma/descriptor.hpp"
#include "dma/descriptor_validator.hpp"

#include <cstddef>
#include <vector>

namespace dma {

class DescriptorRing {
public:
    explicit DescriptorRing(std::size_t capacity);

    std::size_t capacity() const;

    std::size_t head() const;

    std::size_t tail() const;

    bool empty() const;

    bool full() const;

    bool submit(const Descriptor& descriptor);

    Descriptor& front();
    const Descriptor& front() const;

    void consume();

    Descriptor& at(std::size_t index);

    const Descriptor& at(std::size_t index) const;

private:
    std::vector<Descriptor> descriptors_;

    std::size_t head_{0};
    std::size_t tail_{0};
    std::size_t count_{0};
};

} // namespace dma
