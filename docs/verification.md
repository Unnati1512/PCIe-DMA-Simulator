# Final Verification — PCIe DMA Simulator

## 1. Project Scope

This project is an event-driven C++17 architectural simulator for a
PCIe DMA subsystem.

The simulator models architectural behavior rather than implementing
a physical PCIe controller, Linux PCIe driver, or PCIe compliance stack.

## 2. Verification Areas

### Simulation Framework

- Event scheduling
- Cycle-based simulation
- Deterministic event ordering
- Run-until-cycle behavior

### Memory Subsystem

- Bounds-checked memory access
- Read and write operations
- Configurable access latency
- Scheduled memory operations

### DMA

- DMA descriptors
- Descriptor ring management
- DMA controller state machine
- Descriptor validation

### PCIe Transaction Modeling

- Memory Read TLPs
- Memory Write TLPs
- Completion TLPs
- Completion-with-data TLPs
- MPS-based fragmentation
- MRRS-based fragmentation
- 4-KB boundary handling

### Scatter-Gather

- Multiple non-contiguous memory segments
- Segment length accounting
- TLP generation across segments

### IOMMU and TLB

- IOVA to physical-address translation
- Page mapping and unmapping
- TLB hits
- TLB misses
- Page walks
- Translation faults

### Outstanding Requests

- Request tracking
- Tag allocation
- Completion matching
- Outstanding-request limits

### MSI-X

- Vector registration
- Interrupt triggering
- Interrupt counting
- Interrupt coalescing

### Error Recovery

- Completion timeout injection
- Translation fault injection
- Invalid descriptor injection
- TLP error injection
- Retryable error handling
- Non-retryable error handling

### Performance Measurement

- DMA operation counters
- TLP counters
- Byte counters
- Request counters
- TLB counters
- Interrupt counters
- Error and retry counters
- Benchmark CSV generation
- Matplotlib analysis

## 3. Day 14 Experiments

The following experiments were generated and validated:

- Outstanding request concurrency
- IOMMU/TLB behavior
- MPS fragmentation
- MRRS fragmentation
- MSI-X interrupt coalescing

The generated CSV files are stored under:

    benchmarks/data/

The generated plots are stored under:

    benchmarks/plots/

## 4. Important Modeling Limitation

The simulator is an architectural model.

It does not claim to reproduce physical PCIe hardware timing,
electrical behavior, protocol-compliance testing, or real hardware
throughput.

Some benchmark infrastructure is intentionally synthetic and is used
to study the behavior of the modeled subsystem.

## 5. Final Verification Principle

A successful build and passing tests establish software correctness
for the implemented model.

The benchmark experiments are used to demonstrate architectural
relationships and trade-offs within that model.
