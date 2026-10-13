# PCIe DMA Simulator

An event-driven C++17 architectural simulator for studying the behavior of a PCIe DMA subsystem.

The project models the major components involved in a DMA data-transfer pipeline, including descriptor processing, address translation, PCIe transaction generation, outstanding request tracking, and memory transfers. It is designed to help explore how DMA engines interact with PCIe transactions, memory, and supporting hardware components.

> **Note:** This is an architectural simulator, not a physical PCIe controller, Linux driver, or PCIe compliance implementation. Simulation results represent the implemented model and must not be interpreted as measurements of real PCIe hardware.

---

## Project Status

**Status: v0.1 architectural prototype — verification and integration ongoing**

The project contains an event-driven, cycle-based simulation framework implemented in C++17.

The current verified demonstration completes a simulated 4096-byte DMA transfer. The simulator generates a PCIe Memory Read request, retrieves the source data, generates a PCIe Memory Write request, and writes the data to the destination memory.

The final verification compares all 4096 destination bytes against the source data, and the current simulation reports a successful match.

The current regression test suite includes passing tests for:

- DMA descriptor ring behavior
- DMA descriptor validation

Further integration and verification are required before all planned features can be considered fully implemented and validated. In particular, the outstanding-request lifecycle, PCIe completion handling, error recovery, multi-descriptor execution, and performance experiments require verification against the actual implementation.

The project is intended to develop progressively from a working DMA transfer model into a more comprehensive PCIe DMA subsystem simulator.

---

## Objectives

The main objectives of this project are to:

- Understand the architecture of a PCIe DMA subsystem.
- Model DMA descriptor processing and controller state transitions.
- Simulate PCIe Memory Read and Memory Write transactions.
- Explore IOMMU-based address translation and TLB behavior.
- Implement request tracking and PCIe tag management.
- Study transfer latency, transaction overhead, and throughput-related trade-offs.
- Develop regression tests to verify correctness and detect implementation errors.
- Build a foundation for future architectural experiments involving PCIe DMA.

---

## Current Scope

The project is designed to cover the following components. The implementation and verification status of each component should be assessed individually.

### 1. Event-Driven Simulation

The simulator uses a cycle-based event scheduling model to represent operations that occur at different simulation cycles.

The event scheduler provides the foundation for modeling transaction latency, memory operations, and DMA state transitions.

### 2. DMA Descriptor Ring

The descriptor ring provides a mechanism for storing and managing DMA descriptors.

The descriptor-related components include:

- Descriptor storage and ring management
- Descriptor validation
- Descriptor ownership and status fields
- Regression tests for descriptor ring behavior
- Regression tests for descriptor validation

### 3. DMA Controller State Machine

The DMA controller models the progression of a transfer through a sequence of states:

```text
IDLE
  |
  v
FETCH_DESCRIPTOR
  |
  v
TRANSLATE_ADDRESS
  |
  v
GENERATE_REQUEST
  |
  v
WAIT_COMPLETION
  |
  v
UPDATE_DESCRIPTOR
  |
  v
DONE
  |
  v
IDLE

The controller coordinates descriptor processing, address translation, transaction generation, and memory transfer operations.

### 4.IOMMU Address Translation

The simulator includes an address translation component for translating I/O virtual addresses (IOVAs) into physical addresses.

The model includes IOMMU-related translation and TLB accounting.

The current demonstration uses identity mappings. More extensive testing is needed to validate translation behavior across different mappings, page boundaries, and error conditions.

### 5.PCIe Transaction Modeling

The simulator includes PCIe transaction modeling for Memory Read and Memory Write requests.

The current demonstration issues one Memory Read request and one Memory Write request for a 4096-byte transfer.

The PCIe link model accounts for transaction transmission, modeled bytes, and a configurable fixed latency.

The current demonstration uses a simplified transaction model. It should not be interpreted as a complete implementation of PCIe completion processing or the full PCIe protocol.

### 6.Outstanding Request and Tag Tracking

The project includes an outstanding-request tracking component intended to manage requests and their associated PCIe tags.

The component maintains information such as:

Allocated tags
Outstanding requests
Request issue cycles
Request completion counts
Unknown completion counts
Maximum outstanding-request capacity

The tracker exists as a component, but its complete integration into the end-to-end DMA transaction lifecycle must be verified before claiming that the full request-completion lifecycle is modeled.

### 7.Memory Transfer Verification

The current demonstration performs a simulated transfer from source memory to destination memory.

The verification procedure compares the destination data against the original source data.

A successful comparison demonstrates that all bytes in the tested transfer match. It does not, by itself, establish correctness for every transfer size, descriptor configuration, or error condition.

## Architecture

  DMA Descriptors
                     |
                     v
              DMA Controller
              State Machine
                     |
                     v
             Address Translation
                (IOMMU / TLB)
                     |
                     v
           PCIe Transaction Engine
                     |
                     v
               PCIe Link Model
                     |
                     v
              Memory Operations
                     |
                     v
           Transfer Verification

## Project Structure

pcie-dma-simulator/
├── include/          # Header files
├── src/              # Simulator implementation
├── tests/            # Component and regression tests
├── benchmarks/       # Performance experiments and analysis
├── docs/             # Learning notes and verification documents
├── CMakeLists.txt    # Build configuration
└── README.md         # Project documentation

## Technology Stack

C++17 — simulator implementation
CMake — build configuration
CTest — regression test execution
Python 3 — benchmark analysis, where applicable
Matplotlib — benchmark visualization, where applicable
Git and GitHub — version control and project history

## Build and Run

### Requirements
A C++17-compatible compiler
CMake 3.16 or later
A compatible build tool, such as GNU Make

Python 3 and Matplotlib are only required for benchmark analysis scripts that use them.Requirements
A C++17-compatible compiler
CMake 3.16 or later
A compatible build tool, such as GNU Make

Python 3 and Matplotlib are only required for benchmark analysis scripts that use them.

#### Configure the Build
From the project root, run:

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

#### Build the Simulator
cmake --build build -j2

#### Run the Simulator
./build/pcie_dma_simulator

#### Run the Regression Tests
ctest --test-dir build --output-on-failure

## Verified Demonstration

The current verified demonstration performs a simulated 4096-byte DMA transfer.

The model:

1.Initializes source memory with a known data pattern.
2.Initializes destination memory.
3.Submits a DMA descriptor.
4.Processes the descriptor through the DMA controller state machine.
5.Translates the source and destination addresses.
6.Issues a PCIe Memory Read request.
7.Retrieves the source data.
8.Issues a PCIe Memory Write request.
9.Writes the data to destination memory.
10.Compares the destination data against the original source data.

### Representative Simulation Output

PCIe Link: Gen4 x16
Theoretical bandwidth: 31.5077 GB/s
Starting end-to-end DMA transfer
Transfer size: 4096 bytes

Cycle 0: DMA FSM -> IDLE
Cycle 0: DMA FSM -> FETCH_DESCRIPTOR
  Source: 0x1000
  Destination: 0x8000
  Length: 4096 bytes
Cycle 0: DMA FSM -> TRANSLATE_ADDRESS
  Source IOVA translated to physical address: 0x1000
  Destination IOVA translated to physical address: 0x8000
Cycle 0: DMA FSM -> GENERATE_REQUEST
Cycle 0: DMA FSM -> WAIT_COMPLETION
Cycle 0: PCIe MRd issued address=0x1000 length=4096 tag=0
Cycle 145: Read data returned: 4096 bytes
Cycle 145: PCIe MWr issued address=0x8000 length=4096 tag=1
Cycle 290: Destination memory write completed.
Cycle 290: DMA FSM -> UPDATE_DESCRIPTOR
Cycle 290: DMA FSM -> DONE
Cycle 290: DMA FSM -> IDLE

========== VERIFICATION ==========
Simulation finished at cycle: 290
Descriptors completed: 1
PCIe TLP transmissions: 2
PCIe bytes modelled: 8192
IOMMU TLB hits: 0
IOMMU TLB misses: 2

PASS: all 4096 destination bytes match the source.


### Interpretation

The output demonstrates that:

One descriptor completed in the tested scenario.
The model issued two PCIe transaction transmissions.
The model accounted for 8192 transmitted PCIe bytes.
The two address translations were recorded as TLB misses.
The transfer completed at simulation cycle 290.
All 4096 destination bytes matched the source data.

The cycle count and bandwidth figures are results of the simulator's model, not direct measurements of a real PCIe device.

The current demonstration does not establish that all PCIe completion, request-tracking, interrupt, error-recovery, or multi-descriptor scenarios are fully implemented and verified.

## Limitations
This project models PCIe DMA behavior for educational and architectural experimentation. It does not implement a physical PCIe controller, Linux driver, real hardware DMA, or PCIe compliance testing.

Simulation results represent the implemented model and are not measurements of real hardware.
