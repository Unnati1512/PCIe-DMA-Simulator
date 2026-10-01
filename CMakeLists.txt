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
)

target_include_directories(pcie_dma_simulator
    PRIVATE
        include
)

