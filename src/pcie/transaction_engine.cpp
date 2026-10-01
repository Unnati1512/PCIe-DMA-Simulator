#include "pcie/transaction_engine.hpp"

#include <iostream>

namespace pcie {

TransactionEngine::TransactionEngine(
    sim::Simulator& simulator,
    RequestId requester_id
)
    : simulator_(simulator),
      requester_id_(requester_id) {
}

Tlp TransactionEngine::issue_memory_read(
    Address address,
    std::size_t length,
    Tag tag
) {
    Tlp tlp = generator_.create_memory_read(
        requester_id_,
        address,
        length,
        tag
    );

    ++requests_issued_;

    std::cout << "Cycle "
              << simulator_.now()
              << ": PCIe MRd issued"
              << " address=0x"
              << std::hex
              << address
              << std::dec
              << " length="
              << length
              << " tag="
              << static_cast<unsigned>(tag)
              << '\n';

    return tlp;
}

Tlp TransactionEngine::issue_memory_write(
    Address address,
    const std::vector<Byte>& payload,
    Tag tag
) {
    Tlp tlp = generator_.create_memory_write(
        requester_id_,
        address,
        payload,
        tag
    );

    ++requests_issued_;

    std::cout << "Cycle "
              << simulator_.now()
              << ": PCIe MWr issued"
              << " address=0x"
              << std::hex
              << address
              << std::dec
              << " length="
              << payload.size()
              << " tag="
              << static_cast<unsigned>(tag)
              << '\n';

    return tlp;
}

std::size_t TransactionEngine::requests_issued() const {
    return requests_issued_;
}

} // namespace pcie
