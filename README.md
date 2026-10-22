# PCIe DMA Simulator

An event-driven C++17 architectural simulator for exploring PCIe DMA data transfers, address translation, PCIe transaction handling, and performance trade-offs.

This project models a DMA transfer pipeline and includes component-level experiments for studying PCIe Transaction Layer Packets (TLPs), IOMMU/TLB behavior, outstanding request tracking, and MSI-X interrupt coalescing.

> **Project scope:** This is an architectural simulator for learning and experimentation. It is not a physical PCIe controller, a Linux DMA driver, or a PCIe protocol compliance implementation.

## Project Status

**Status: Architectural prototype — v0.1**

The project includes an end-to-end DMA transfer demonstration and separate models and experiments for selected PCIe DMA subsystem components.

The main demonstration verifies a 4,096-byte transfer through modeled Memory Read and Memory Write transactions. Additional experiments investigate fragmentation, address translation, request tracking, and interrupt coalescing.

The end-to-end demonstration and component-level experiments have different integration levels. Results from standalone experiments should not be interpreted as proof that every component operates together in the main DMA pipeline.

## Architecture

```text
                DMA Descriptor
                      |
                      v
                DMA Controller
                      |
                      v
             IOMMU / TLB Translation
                      |
                      v
             PCIe Transaction Generation
                      |
                      v
             Outstanding Request Tracking
                      |
                      v
               Memory / Completion
                      |
                      v
             Descriptor Status Update
                      |
                      v
                Transfer Complete
```

The main DMA demonstration exercises descriptor processing, address translation, PCIe Memory Read and Memory Write transactions, scheduled memory operations, and descriptor completion.

TLP fragmentation, request-concurrency behavior, and interrupt coalescing are modeled through separate component-level experiments rather than a fully integrated concurrent DMA pipeline.

## Implemented Features

### 1. Event-Driven Simulation

- Cycle-based event scheduling and execution.
- Event ordering and scheduled callbacks.
- Simulation execution up to a specified cycle.

### 2. DMA Controller

- DMA descriptor processing.
- Descriptor validation.
- Finite-state-machine-based transfer control.
- Modeled source and destination address translation.
- Sequential read-then-write transfer flow.
- Descriptor completion and error-related handling.

### 3. PCIe Transaction Modeling

- Modeled Memory Read (MRd) requests.
- Modeled Memory Write (MWr) requests.
- Completion (Cpl) and Completion with Data (CplD) TLP types.
- Simplified PCIe transaction timing.

### 4. TLP Fragmentation

- Maximum Payload Size (MPS)-based write fragmentation.
- Maximum Read Request Size (MRRS)-based read fragmentation.
- 4-KB boundary handling in the fragmentation component.
- Fragment-count experiments across different configured sizes.

### 5. IOMMU and TLB

- Software-modeled address mappings.
- TLB lookup behavior.
- TLB hit and miss accounting.
- Page-walk and translation-fault modeling.

### 6. Outstanding Request Tracking

- PCIe request tag tracking.
- Request allocation and retirement.
- Configurable outstanding-request limits.
- Component-level concurrency experiments.

### 7. Interrupt Modeling

- MSI-X interrupt modeling.
- Interrupt coalescing.
- Experiments comparing interrupt counts and modeled notification latency.

### 8. Performance Analysis

- Simulation counters.
- Standalone component-level experiments.
- Benchmark analysis using Python and Matplotlib.

## End-to-End DMA Demonstration

The main executable demonstrates a 4,096-byte DMA transfer from source address `0x1000` to destination address `0x8000`.

The modeled transfer follows these steps:

1. A DMA descriptor is processed.
2. Source and destination addresses are translated.
3. A PCIe Memory Read request is issued.
4. The modeled read data is returned.
5. A PCIe Memory Write request is issued.
6. The destination memory operation completes.
7. The DMA controller marks the descriptor as complete.
8. The destination data is compared against the source data.

### Demonstrated Results

| Metric | Result |
|---|---:|
| Transfer size | 4,096 bytes |
| Simulation completion cycle | 290 |
| PCIe TLP transmissions | 2 |
| Modeled PCIe payload bytes | 8,192 |
| IOMMU TLB hits | 0 |
| IOMMU TLB misses | 2 |
| Destination data verification | PASS |

The final verification confirms that all 4,096 destination bytes match the source.

These values describe the configured simulation model. They are not measurements of physical PCIe hardware performance.

## Component-Level Experiments

The `benchmarks/` directory contains standalone experiments that investigate selected subsystem behaviors.

| Experiment | Objective |
|---|---|
| Outstanding request concurrency | Examine request-tracking capacity and PCIe tag usage |
| IOMMU/TLB behavior | Examine translation hits, misses, and page walks |
| MPS fragmentation | Examine Memory Write fragment counts |
| MRRS fragmentation | Examine Memory Read request counts |
| Interrupt coalescing | Examine interrupt counts and modeled notification latency |

### MPS and MRRS Fragmentation

For a 16-KiB transfer, the standalone fragmentation experiments report the following fragment counts:

| Configured fragment/request size | Number of fragments |
|---:|---:|
| 128 bytes | 128 |
| 256 bytes | 64 |
| 512 bytes | 32 |
| 1,024 bytes | 16 |
| 2,048 bytes | 8 |
| 4,096 bytes | 4 |

These results illustrate how configured fragment sizes affect the number of modeled transactions for the tested transfer.

### IOMMU/TLB Experiment

The standalone TLB experiment accesses mapped pages and records translation behavior. One recorded experiment reports:

| Metric | Result |
|---|---:|
| Accesses | 64 |
| TLB hits | 60 |
| TLB misses | 4 |
| Page walks | 4 |
| Translation faults | 0 |

These results belong to the standalone experiment, not the end-to-end DMA demonstration.

### Outstanding Request Concurrency

The concurrency experiment evaluates request tracking at configured concurrency levels of 1, 2, 4, 8, 16, 32, and 64.

Its completion-cycle values are synthetic values assigned by the experiment. They are not measured PCIe latencies or evidence of physical link throughput.

### MSI-X Interrupt Coalescing

The interrupt experiment models the effect of different coalescing thresholds on interrupt counts and notification latency.

The results reflect the experiment's configured completion schedule and timing assumptions. They should not be interpreted as hardware measurements.

## Project Structure

```text
pcie-dma-simulator/
├── include/
│   ├── dma/
│   ├── memory/
│   ├── pcie/
│   └── simulator/
├── src/
│   ├── dma/
│   ├── memory/
│   ├── pcie/
│   ├── simulator/
│   └── main.cpp
├── tests/
├── benchmarks/
│   ├── concurrency_experiment.cpp
│   ├── tlb_experiment.cpp
│   ├── mps_experiment.cpp
│   ├── mrrs_experiment.cpp
│   ├── interrupt_experiment.cpp
│   └── analyze.py
├── docs/
│   ├── learning-log.md
│   ├── verification.md
│   └── experiment-summary.md
├── CMakeLists.txt
└── README.md
```

## Requirements

- C++17-compatible compiler
- CMake 3.16 or later
- Python 3
- Matplotlib for benchmark analysis

## Build and Run

### Configure and Build

Run the following commands from the project root:

```bash
cmake -S . -B build
cmake --build build -j2
```

### Run the Simulator

```bash
./build/pcie_dma_simulator
```

The executable runs the end-to-end DMA demonstration and reports the simulated transfer progress and verification result.

### Run Regression Tests

```bash
ctest --test-dir build --output-on-failure
```

The current CTest configuration registers the descriptor-ring and descriptor-validation regression tests.

Additional test source files may exist in `tests/`, but files that are not registered with CTest are not automatically executed by this command.

### Run Benchmark Analysis

```bash
python3 benchmarks/analyze.py
```

The analysis script processes the benchmark data available to it. The standalone experiment source files are not automatically compiled or executed by the standard CMake build unless corresponding build targets have been configured.

## Limitations

- The simulator does not implement a physical PCIe controller, electrical signaling, or PHY behavior.
- PCIe transaction behavior is simplified and is not intended for protocol compliance testing.
- The IOMMU is software-modeled and is not connected to a real operating-system or hardware IOMMU.
- The main DMA demonstration uses a simplified, sequential read-then-write flow.
- TLP fragmentation is modeled separately and is not integrated into the main DMA controller's transfer path.
- The main DMA path does not demonstrate multiple concurrent outstanding DMA requests.
- The main controller rejects transfers that cross the relevant 4-KB IOMMU page boundary rather than automatically splitting the transfer across translated pages.
- Request tracking, fragmentation, and interrupt-coalescing experiments are component-level models rather than a fully integrated concurrent DMA implementation.
- Simulation timing and bandwidth calculations depend on the implemented assumptions.
- Theoretical link bandwidth is not equivalent to measured application throughput.
- Benchmark results must not be interpreted as measurements of real PCIe hardware.

## Technologies

- C++17
- CMake
- Python
- Matplotlib
- Event-driven simulation
- PCIe DMA architectural modeling

## Documentation

- [`docs/learning-log.md`](docs/learning-log.md) — Development and learning notes
- [`docs/experiment-summary.md`](docs/experiment-summary.md) — Experiment results and observations
- [`docs/verification.md`](docs/verification.md) — Verification approach and results

## Author

**Unnati Chaturvedi**
