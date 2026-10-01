#pragma once

#include "pcie/tlp.hpp"

#include <string>

namespace pcie {

std::string to_string(TlpType type);

bool is_read_request(const Tlp& tlp);

bool is_write_request(const Tlp& tlp);

bool is_completion(const Tlp& tlp);

bool has_payload(const Tlp& tlp);

} // namespace pcie
