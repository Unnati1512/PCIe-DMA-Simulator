#pragma once

#include "pcie/tlp.hpp"
#include "simulator/simulator.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>

namespace pcie {

enum class PcieGeneration {
    Gen1,
    Gen2,
    Gen3,
    Gen4,
    Gen5
};

class PcieLink {
public:
    PcieLink(
        sim::Simulator& simulator,
        PcieGeneration generation,
        std::size_t lanes,
        sim::Cycle fixed_latency = 10
    );

    double raw_rate_gtps() const;

    double encoding_efficiency() const;

    double theoretical_bandwidth_gbps() const;

    std::size_t transmission_cycles(
        std::size_t payload_bytes
    ) const;

    void transmit(
        const Tlp& tlp,
        std::function<void()> completion
    );

    PcieGeneration generation() const;

    std::size_t lanes() const;

    sim::Cycle fixed_latency() const;

    std::size_t transmissions() const;

    std::size_t bytes_transmitted() const;

    double utilization() const;

private:
    sim::Simulator& simulator_;

    PcieGeneration generation_;

    std::size_t lanes_;

    sim::Cycle fixed_latency_;

    std::size_t transmissions_{0};

    std::size_t bytes_transmitted_{0};

    sim::Cycle busy_cycles_{0};

    sim::Cycle last_busy_start_{0};
};

} // namespace pcie
