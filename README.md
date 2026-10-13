# PCIe DMA Subsystem Simulator

An event-driven, cycle-based **C++17 simulator** that models a PCIe DMA data-transfer pipeline, including descriptor processing, address translation, PCIe Memory Read/Write transactions, outstanding-request tracking, and transfer verification.

The project explores how a DMA subsystem coordinates memory transfers through a simplified PCIe transaction model.

> **Scope:** This is a software-based architectural simulator, not a physical PCIe controller, hardware DMA engine, or PCIe compliance implementation.

## Features

- **Event-driven simulation:** Schedule and execute operations at simulated clock cycles.
- **DMA controller:** Process descriptors through a state-machine-driven transfer pipeline.
- **Descriptor management:** Implement a descriptor ring and descriptor validation.
- **PCIe transaction modelling:** Generate Memory Read (MRd) and Memory Write (MWr) Transaction Layer Packets (TLPs).
- **Request tracking:** Manage outstanding requests, PCIe tags, completion, and request retirement.
- **Address translation:** Integrate IOMMU address translation and TLB statistics.
- **Simulated memory:** Schedule source-memory reads and destination-memory writes.
- **Verification:** Compare source and destination data after a DMA transfer.
- **Regression tests:** Test descriptor-ring and descriptor-validation behaviour.

## Architecture

```text
        DMA Descriptor
              |
              v
     Descriptor Ring
     + Validation
              |
              v
       DMA Controller
        (State Machine)
              |
              v
      IOMMU / TLB
   Address Translation
              |
              v
    PCIe Transaction Engine
       MRd / MWr TLPs
              |
              v
       PCIe Link Model
              |
              v
      Simulated Memory
       Read / Write
              |
              v
      Data Verification
```

## How the Transfer Works

The demonstrated transfer follows this sequence:

1. Fetch and validate the DMA descriptor.
2. Translate the source and destination addresses.
3. Generate and transmit a PCIe Memory Read request.
4. Schedule a simulated source-memory read.
5. Generate and transmit a Memory Write request containing the returned data.
6. Schedule the destination-memory write.
7. Update the descriptor state and return the DMA controller to idle.
8. Compare the destination data with the source data.

The current end-to-end demonstration processes one descriptor and transfers 4096 bytes.

## Technology Stack

- **Language:** C++17
- **Build system:** CMake
- **Testing:** CTest
- **Development environment:** Linux / Ubuntu
- **Version control:** Git

## Project Structure

```text
PCIe-DMA-Simulator/
├── include/       # Component headers
├── src/           # Simulator implementation
├── tests/         # Regression tests
├── docs/          # Learning, experiment, and verification notes
├── CMakeLists.txt
└── README.md
```

## Build and Run

### Prerequisites

- C++17-compatible compiler
- CMake
- Build tools

On Ubuntu:

```bash
sudo apt update
sudo apt install build-essential cmake git
```

### Build

```bash
cmake -S . -B build
cmake --build build -j2
```

### Run the simulator

```bash
./build/pcie_dma_simulator
```

### Run regression tests

```bash
ctest --test-dir build --output-on-failure
```

## Results

The verified end-to-end simulation used the following configuration:

| Parameter | Result |
|---|---|
| PCIe configuration modelled | Gen4 x16 |
| Source address | `0x1000` |
| Destination address | `0x8000` |
| Transfer size | 4096 bytes |
| Simulation completion cycle | 290 |
| Descriptors completed | 1 |
| PCIe TLP transmissions | 2 |
| PCIe bytes modelled | 8192 |
| IOMMU TLB hits | 0 |
| IOMMU TLB misses | 2 |
| Data-integrity verification | PASS |

**Verification result:** All 4096 destination bytes matched the source data.

The automated regression suite also passed both existing tests:

- `descriptor_ring_regression`
- `descriptor_validator_regression`

These results describe the current simulation and test suite. The reported cycle count and bandwidth figures are model outputs, not measurements of physical PCIe hardware.

## Limitations

- The simulator does not implement a physical PCIe endpoint or DMA engine.
- PCIe electrical, PHY, and hardware-level behaviour are outside its scope.
- It is not a complete PCIe protocol-compliance implementation.
- The demonstrated end-to-end transfer covers one descriptor and one 4096-byte transfer.
- The current regression suite covers descriptor-ring and descriptor-validation behaviour; it does not exhaustively verify every transaction-tracking or DMA error scenario.
- Simulation timing and performance results should not be interpreted as real-hardware benchmarks.

## Future Improvements

- Add dedicated tests for outstanding-tag exhaustion, reuse, and invalid completions.
- Test DMA error recovery and address-translation failures.
- Extend verification to multiple descriptors and outstanding transactions.
- Compare simulated performance across transfer sizes and outstanding-request limits.
- Explore RTL implementation and FPGA deployment as a separate hardware-oriented extension.

## Documentation

- [`docs/learning-log.md`](docs/learning-log.md) — Development and learning notes
- [`docs/experiment-summary.md`](docs/experiment-summary.md) — Experiment results and observations
- [`docs/verification.md`](docs/verification.md) — Verification approach and results

## Author

**Unnati Chaturvedi**
