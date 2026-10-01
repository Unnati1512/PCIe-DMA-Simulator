#include "pcie/error.hpp"

#include <stdexcept>

namespace pcie {

void ErrorInjector::inject_completion_timeout(Tag tag) {
    injected_errors_[tag] = ErrorType::CompletionTimeout;
}

void ErrorInjector::inject_translation_fault(Tag tag) {
    injected_errors_[tag] = ErrorType::TranslationFault;
}

void ErrorInjector::inject_invalid_descriptor(Tag tag) {
    injected_errors_[tag] = ErrorType::InvalidDescriptor;
}

void ErrorInjector::inject_tlp_error(Tag tag) {
    injected_errors_[tag] = ErrorType::TlpError;
}

bool ErrorInjector::has_error(Tag tag) const {
    return injected_errors_.find(tag) != injected_errors_.end();
}

ErrorType ErrorInjector::error_type(Tag tag) const {
    const auto it = injected_errors_.find(tag);

    if (it == injected_errors_.end()) {
        throw std::out_of_range(
            "No injected error exists for tag"
        );
    }

    return it->second;
}

void ErrorInjector::clear(Tag tag) {
    injected_errors_.erase(tag);
}

std::size_t ErrorInjector::injected_error_count() const {
    return injected_errors_.size();
}

bool ErrorRecoveryManager::is_retryable(ErrorType type) const {
    switch (type) {
        case ErrorType::CompletionTimeout:
            return true;

        case ErrorType::TlpError:
            return true;

        case ErrorType::TranslationFault:
            return false;

        case ErrorType::InvalidDescriptor:
            return false;
    }

    return false;
}

void ErrorRecoveryManager::handle_error(
    ErrorType type,
    bool retry_successful
) {
    ++statistics_.errors_detected;

    if (!is_retryable(type)) {
        ++statistics_.aborted_requests;
        return;
    }

    ++statistics_.retries;

    if (retry_successful) {
        ++statistics_.successful_recoveries;
    } else {
        ++statistics_.aborted_requests;
    }
}

const ErrorRecoveryManager::Statistics&
ErrorRecoveryManager::statistics() const {
    return statistics_;
}

} // namespace pcie
