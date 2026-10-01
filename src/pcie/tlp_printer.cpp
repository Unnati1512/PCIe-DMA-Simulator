#include "pcie/tlp_printer.hpp"

#include "pcie/tlp_utils.hpp"

#include <iomanip>

namespace pcie {

void print_tlp(
    const Tlp& tlp,
    std::ostream& output
) {
    output << "TLP\n";

    output << "  Type: "
           << to_string(tlp.type)
           << '\n';

    output << "  Requester ID: "
           << tlp.requester_id
           << '\n';

    output << "  Address: 0x"
           << std::hex
           << tlp.address
           << '\n';

    output << std::dec
           << "  Length: "
           << tlp.length
           << " bytes\n";

    output << "  Tag: "
           << static_cast<unsigned>(tlp.tag)
           << '\n';

    output << "  First BE: 0x"
           << std::hex
           << static_cast<unsigned>(tlp.first_byte_enable)
           << '\n';

    output << "  Last BE: 0x"
           << static_cast<unsigned>(tlp.last_byte_enable)
           << '\n';

    output << std::dec;

    output << "  Payload bytes: "
           << tlp.payload.size()
           << '\n';
}

} // namespace pcie
