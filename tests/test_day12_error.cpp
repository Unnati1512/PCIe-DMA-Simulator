#include "pcie/error.hpp"

#include <cassert>
#include <iostream>

int main() {
    std::cout << "=== Day 12 Error Injection and Recovery Test ===\n\n";

    pcie::ErrorInjector injector;

    injector.inject_completion_timeout(1);
    injector.inject_translation_fault(2);
    injector.inject_invalid_descriptor(3);
    injector.inject_tlp_error(4);

    assert(injector.injected_error_count() == 4);

    assert(injector.has_error(1));
    assert(injector.has_error(2));
    assert(injector.has_error(3));
    assert(injector.has_error(4));

    assert(
        injector.error_type(1) ==
        pcie::ErrorType::CompletionTimeout
    );

    assert(
        injector.error_type(2) ==
        pcie::ErrorType::TranslationFault
    );

    assert(
        injector.error_type(3) ==
        pcie::ErrorType::InvalidDescriptor
    );

    assert(
        injector.error_type(4) ==
        pcie::ErrorType::TlpError
    );

    injector.clear(4);

    assert(!injector.has_error(4));
    assert(injector.injected_error_count() == 3);

    pcie::ErrorRecoveryManager recovery;

    assert(
        recovery.is_retryable(
            pcie::ErrorType::CompletionTimeout
        )
    );

    assert(
        recovery.is_retryable(
            pcie::ErrorType::TlpError
        )
    );

    assert(
        !recovery.is_retryable(
            pcie::ErrorType::TranslationFault
        )
    );

    assert(
        !recovery.is_retryable(
            pcie::ErrorType::InvalidDescriptor
        )
    );

    recovery.handle_error(
        pcie::ErrorType::CompletionTimeout,
        true
    );

    recovery.handle_error(
        pcie::ErrorType::TlpError,
        false
    );

    recovery.handle_error(
        pcie::ErrorType::TranslationFault,
        false
    );

    recovery.handle_error(
        pcie::ErrorType::InvalidDescriptor,
        false
    );

    const auto& stats = recovery.statistics();

    assert(stats.errors_detected == 4);
    assert(stats.retries == 2);
    assert(stats.successful_recoveries == 1);
    assert(stats.aborted_requests == 3);

    std::cout << "Injected errors: 4\n";
    std::cout << "Errors detected: "
              << stats.errors_detected << '\n';

    std::cout << "Retries: "
              << stats.retries << '\n';

    std::cout << "Successful recoveries: "
              << stats.successful_recoveries << '\n';

    std::cout << "Aborted requests: "
              << stats.aborted_requests << '\n';

    std::cout << "\nError injection and recovery test passed.\n";

    return 0;
}
