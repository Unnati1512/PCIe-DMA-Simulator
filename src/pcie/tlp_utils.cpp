#include "pcie/tlp_utils.hpp"

#include <stdexcept>

namespace pcie {

std::string to_string(TlpType type) {
    switch (type) {
        case TlpType::MemoryRead:
            return "MRd";

        case TlpType::MemoryWrite:
            return "MWr";

        case TlpType::Completion:
            return "Cpl";

        case TlpType::CompletionWithData:
            return "CplD";
    }

    throw std::invalid_argument("Unknown TLP type");
}

bool is_read_request(const Tlp& tlp) {
    return tlp.type == TlpType::MemoryRead;
}

bool is_write_request(const Tlp& tlp) {
    return tlp.type == TlpType::MemoryWrite;
}

bool is_completion(const Tlp& tlp) {
    return tlp.type == TlpType::Completion ||
           tlp.type == TlpType::CompletionWithData;
}

bool has_payload(const Tlp& tlp) {
    return !tlp.payload.empty();
}

} // namespace pcie
