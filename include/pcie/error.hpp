#pragma once

#include "pcie/tlp.hpp"
#include "simulator/event.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <unordered_set>

namespace pcie {

enum class ErrorType {
    CompletionTimeout,
    TranslationFault,
    InvalidDescriptor,
    TlpError
};

struct ErrorEvent {
    ErrorType type;
    sim::Cycle cycle;
    Tag tag;
};

class ErrorInjector {
public:
    void inject_completion_timeout(Tag tag);
    void inject_translation_fault(Tag tag);
    void inject_invalid_descriptor(Tag tag);
    void inject_tlp_error(Tag tag);

    bool has_error(Tag tag) const;
    ErrorType error_type(Tag tag) const;

    void clear(Tag tag);

    std::size_t injected_error_count() const;

private:
    std::unordered_map<Tag, ErrorType> injected_errors_;
};

class ErrorRecoveryManager {
public:
    struct Statistics {
        std::size_t errors_detected{0};
        std::size_t retries{0};
        std::size_t successful_recoveries{0};
        std::size_t aborted_requests{0};
    };

    bool is_retryable(ErrorType type) const;

    void handle_error(
        ErrorType type,
        bool retry_successful
    );

    const Statistics& statistics() const;

private:
    Statistics statistics_;
};

} // namespace pcie
