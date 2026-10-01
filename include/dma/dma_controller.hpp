#pragma once

#include "dma/descriptor_ring.hpp"
#include "simulator/simulator.hpp"

#include <cstddef>
#include <cstdint>

namespace dma {

enum class DmaState {
    IDLE,
    FETCH_DESCRIPTOR,
    TRANSLATE_ADDRESS,
    GENERATE_REQUEST,
    WAIT_COMPLETION,
    UPDATE_DESCRIPTOR,
    DONE,
    ERROR
};

class DmaController {
public:
    DmaController(
        sim::Simulator& simulator,
        DescriptorRing& descriptor_ring
    );

    void start();

    DmaState state() const;

    std::size_t descriptors_completed() const;

private:
    void transition_to(DmaState next_state);

    void handle_idle();

    void handle_fetch_descriptor();

    void handle_translate_address();

    void handle_generate_request();

    void handle_wait_completion();

    void handle_update_descriptor();

    void handle_done();

    void handle_error();

    sim::Simulator& simulator_;

    DescriptorRing& descriptor_ring_;

    DmaState state_{DmaState::IDLE};

    std::size_t descriptors_completed_{0};

    Descriptor current_descriptor_{};
};

} // namespace dma
