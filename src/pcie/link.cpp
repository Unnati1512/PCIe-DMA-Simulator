#include "pcie/link.hpp"
#include "pcie/tlp_utils.hpp"

#include <cmath>
#include <stdexcept>

namespace pcie {

namespace {

double generation_rate(PcieGeneration generation) {
    switch (generation) {
        case PcieGeneration::Gen1:
            return 2.5;

        case PcieGeneration::Gen2:
            return 5.0;

        case PcieGeneration::Gen3:
            return 8.0;

        case PcieGeneration::Gen4:
            return 16.0;

        case PcieGeneration::Gen5:
            return 32.0;
    }

    throw std::invalid_argument(
        "Unknown PCIe generation"
    );
}

double generation_efficiency(PcieGeneration generation) {
    switch (generation) {
        case PcieGeneration::Gen1:
        case PcieGeneration::Gen2:
            return 8.0 / 10.0;

        case PcieGeneration::Gen3:
        case PcieGeneration::Gen4:
        case PcieGeneration::Gen5:
            return 128.0 / 130.0;
    }

    throw std::invalid_argument(
        "Unknown PCIe generation"
    );
}

std::size_t payload_bytes(const Tlp& tlp) {
    if (is_write_request(tlp) ||
        tlp.type == TlpType::CompletionWithData) {
        return tlp.payload.size();
    }

    return tlp.length;
}

} // namespace

PcieLink::PcieLink(
    sim::Simulator& simulator,
    PcieGeneration generation,
    std::size_t lanes,
    sim::Cycle fixed_latency
)
    : simulator_(simulator),
      generation_(generation),
      lanes_(lanes),
      fixed_latency_(fixed_latency) {

    if (lanes_ == 0) {
        throw std::invalid_argument(
            "PCIe link must have at least one lane"
        );
    }
}

double PcieLink::raw_rate_gtps() const {
    return generation_rate(generation_);
}

double PcieLink::encoding_efficiency() const {
    return generation_efficiency(generation_);
}

double PcieLink::theoretical_bandwidth_gbps() const {
    /*
     * GT/s represents transfers per second.
     *
     * We multiply:
     *
     * transfer rate
     * × number of lanes
     * × encoding efficiency
     *
     * and convert bits to bytes.
     */
    return raw_rate_gtps()
           * static_cast<double>(lanes_)
           * encoding_efficiency()
           / 8.0;
}

std::size_t PcieLink::transmission_cycles(
    std::size_t payload_bytes
) const {

    if (payload_bytes == 0) {
        return 0;
    }

    /*
     * The simulator uses 1 GHz simulation time:
     *
     * 1 cycle = 1 ns
     *
     * Therefore:
     *
     * bytes_per_second / 1e9
     * = bytes_per_simulation_cycle.
     */

    const double bytes_per_cycle =
        theoretical_bandwidth_gbps();

    return static_cast<std::size_t>(
        std::ceil(
            static_cast<double>(payload_bytes)
            / bytes_per_cycle
        )
    );
}

void PcieLink::transmit(
    const Tlp& tlp,
    std::function<void()> completion
) {
    const std::size_t bytes = payload_bytes(tlp);

    const std::size_t serialization_cycles =
        transmission_cycles(bytes);

    const sim::Cycle start_cycle =
        simulator_.now();

    const sim::Cycle total_latency =
        fixed_latency_
        + serialization_cycles;

    ++transmissions_;
    bytes_transmitted_ += bytes;
    busy_cycles_ += serialization_cycles;

    last_busy_start_ = start_cycle;

    simulator_.schedule(
        start_cycle + total_latency,
        std::move(completion)
    );
}

PcieGeneration PcieLink::generation() const {
    return generation_;
}

std::size_t PcieLink::lanes() const {
    return lanes_;
}

sim::Cycle PcieLink::fixed_latency() const {
    return fixed_latency_;
}

std::size_t PcieLink::transmissions() const {
    return transmissions_;
}

std::size_t PcieLink::bytes_transmitted() const {
    return bytes_transmitted_;
}

double PcieLink::utilization() const {
    const sim::Cycle elapsed =
        simulator_.now();

    if (elapsed == 0) {
        return 0.0;
    }

    return static_cast<double>(busy_cycles_)
           / static_cast<double>(elapsed);
}

} // namespace pcie
