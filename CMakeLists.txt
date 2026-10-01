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
    src/memory/memory.cpp
    src/dma/descriptor_ring.cpp
    src/dma/descriptor_validator.cpp
    src/dma/dma_controller.cpp
    src/pcie/tlp_utils.cpp
    src/pcie/outstanding_requests.cpp
    src/pcie/tlp_generator.cpp
    src/pcie/tlp_printer.cpp
    src/pcie/transaction_engine.cpp
    src/pcie/link.cpp
    src/pcie/tlp_fragmenter.cpp
    src/dma/scatter_gather.cpp
    src/dma/scatter_gather_engine.cpp
    src/memory/iommu.cpp
)

target_include_directories(pcie_dma_simulator
    PRIVATE
        include
)

