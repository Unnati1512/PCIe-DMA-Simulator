#pragma once

#include "pcie/tlp.hpp"

#include <ostream>

namespace pcie {

void print_tlp(
    const Tlp& tlp,
    std::ostream& output
);

} // namespace pcie
