cmake_minimum_required(VERSION 3.16)

project(pcie_dma_simulator
	VERSION 0.1.0
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(pcie_dma_simulator
    src/main.cpp
    src/simulator/simulator.cpp
    src/simulator/performance_counters.cpp
    src/simulator/benchmark.cpp
    src/memory/memory.cpp
    src/dma/descriptor_ring.cpp
    src/dma/descriptor_validator.cpp
    src/dma/dma_controller.cpp
    src/pcie/tlp_utils.cpp
    src/pcie/outstanding_requests.cpp
    src/pcie/tlp_generator.cpp
    src/pcie/interrupt_coalescer.cpp
    src/pcie/error.cpp
    src/pcie/tlp_printer.cpp
    src/pcie/transaction_engine.cpp
    src/pcie/link.cpp
    src/pcie/msix.cpp
    src/pcie/tlp_fragmenter.cpp
    src/dma/scatter_gather.cpp
    src/dma/scatter_gather_engine.cpp
    src/memory/iommu.cpp
)

target_include_directories(pcie_dma_simulator
    PRIVATE
        include
)

include(CTest)

if(BUILD_TESTING)
    add_executable(test_descriptor_ring
        tests/test_descriptor_ring.cpp
        src/dma/descriptor_ring.cpp
        src/dma/descriptor_validator.cpp
    )

    target_include_directories(test_descriptor_ring
        PRIVATE
            include
    )

    add_test(
        NAME descriptor_ring_regression
        COMMAND test_descriptor_ring
    )
    add_executable(test_descriptor_validator
        tests/test_descriptor_validator.cpp
        src/dma/descriptor_validator.cpp
    )

    target_include_directories(test_descriptor_validator
        PRIVATE
            include
    )

    add_test(
        NAME descriptor_validator_regression
        COMMAND test_descriptor_validator
    )
endif()
