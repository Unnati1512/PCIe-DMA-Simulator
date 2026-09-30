# PCIe DMA Simulator

A cycle/event-based PCIe DMA subsystem simulator implemented in C++17.

## Objective 

Model the major software-visible and hardware-oriented components of a PCIe DMA subsystem, including:

- DMA descriptor processing
- Scatter-gather transfers
- PCIe Memory Read/Write TLPs
- Completion handling
- IOVA/IOMMU translation
- TLB behavior
- Outstanding DMA requests
- MSI-X interrupts
- Error injection and recovery
- Performance counters and benchmarking

## Technology
- C++17
- CMake
- Git
- Python
- NumPy
- Matplotlib

## Status

Project setup complete

Implementation begins with the simulation framework and memory subsystem
