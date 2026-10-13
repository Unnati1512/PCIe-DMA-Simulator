# Learning Log

## Day 1 — Event-Driven Simulation

### What is a discrete-event simulator?

A discrete-event simulator models a system by representing important changes in the system as events occurring at specific simulated times or cycles. Instead of executing every possible clock cycle, the simulator jumps to the next scheduled event. This allows hardware behavior to be modeled efficiently.

### Why do we need it for a PCIe DMA simulator?

A DMA subsystem contains many operations that occur at different times, such as descriptor fetching, address translation, PCIe request transmission, and completion arrival. Our simulator needs a common notion of simulated time so that these operations can interact in a deterministic way.

### What is an event?

An event represents something that happens at a particular simulation cycle. It contains a scheduled time and an action that should execute when the simulator reaches that time.

### What is an event queue?

The event queue stores future events ordered by their scheduled cycle. The simulator removes the earliest event, advances its clock to that event's cycle, and executes it.

### Why do we need deterministic ordering?

Multiple hardware events may occur at the same cycle. Giving each event a sequence number allows the simulator to execute same-cycle events in a deterministic insertion order.

### What did I build today?

I implemented the event-driven simulation framework that will provide the timing infrastructure for the PCIe DMA subsystem simulator.


## Day 4 — DMA Controller FSM

Today I implemented the DMA controller as an event-driven finite state machine.
The controller processes descriptors from the DMA descriptor ring through
FETCH_DESCRIPTOR, TRANSLATE_ADDRESS, GENERATE_REQUEST, WAIT_COMPLETION,
UPDATE_DESCRIPTOR, DONE, and ERROR states. I used identity address translation
for this stage because the actual IOMMU and TLB model will be implemented later.
The completion event is simulated through the event-driven simulator, allowing
the controller to wait for a future simulation cycle instead of blocking real time.
I also verified that multiple descriptors are processed sequentially and that
invalid descriptors are rejected before entering the DMA controller.


## Day 5 — PCIe TLP Model

Today I implemented an architectural PCIe Transaction Layer Packet model.
The simulator now represents Memory Read (MRd), Memory Write (MWr), Completion
(Cpl), and Completion with Data (CplD) transactions. Each TLP contains requester
ID, address, length, tag, byte-enable fields, and an optional payload. I also
implemented a transaction engine that generates PCIe memory read and write
requests. Tags are included so that the simulator can later model multiple
outstanding requests and out-of-order completions. This model represents the
PCIe transaction layer at an architectural level rather than implementing the
physical layer or a complete PCIe protocol stack.


## Day 6 — PCIe Link and Bandwidth Model

Today I implemented an architectural PCIe link model that connects the
transaction layer to a simulated transmission path. The model represents
PCIe generations, lane width, raw transfer rate, encoding efficiency,
theoretical bandwidth, serialization time, and fixed transmission latency.

For example, a Gen4 x16 link provides approximately 31.51 GB/s of theoretical
payload bandwidth using the 16 GT/s lane rate, 16 lanes, and 128b/130b encoding
efficiency. A 4096-byte transfer requires 130 simulated serialization cycles
at the modeled 1 GHz simulation clock, followed by the configured 10-cycle
fixed link latency.

TLP transmission is represented as a simulation event rather than a real-time
delay. This allows PCIe transmission latency to participate in end-to-end
simulation timing. The model provides the foundation for later experiments
involving PCIe bandwidth, MPS, request concurrency, TLP fragmentation, and
link utilization.

This implementation is an architectural model of PCIe transaction/link
behavior and does not implement the physical layer or a complete PCIe
protocol stack.


## Day 7 — TLP Fragmentation, MPS, MRRS and 4-KB Boundaries

Today I implemented a PCIe TLP fragmentation model for large DMA transfers.
The fragmenter uses Maximum Payload Size (MPS) for Memory Write transactions
and Maximum Read Request Size (MRRS) for Memory Read requests.

A large transfer is divided into multiple TLPs so that no individual request
exceeds its configured size limit. The fragmenter also prevents a memory
request from crossing a 4-KB address boundary. For example, a 4096-byte
Memory Write with an MPS of 256 bytes is divided into 16 TLPs, while a
4096-byte Memory Read with an MRRS of 512 bytes is divided into 8 requests.

The 4-KB boundary logic was also tested using a transfer beginning at address
0x1F80. Because only 128 bytes remain before the 0x2000 boundary, the transfer
is fragmented into 128, 256, and 128-byte TLPs.

This provides the foundation for modeling realistic PCIe transaction
fragmentation and will later allow DMA performance experiments to study the
effects of MPS, MRRS, transfer size, and request boundaries.


## Day 8 — Scatter-Gather DMA

Today I implemented a scatter-gather DMA model for handling logically
contiguous transfers that are backed by multiple physically non-contiguous
memory segments. A scatter-gather list stores the physical address and length
of each segment while preserving the total logical transfer size.

I implemented a scatter-gather engine that walks the segment list and converts
each segment into PCIe Memory Write TLPs through the existing TLP fragmenter.
This keeps the scatter-gather layer separate from PCIe transaction
fragmentation.

The implementation was tested with four non-contiguous segments totaling
12288 bytes. With an MPS of 256 bytes, the segments generated 16, 16, 8, and
8 TLPs respectively, for a total of 48 Memory Write TLPs. The tests also
verified that each TLP respects the MPS limit, begins at the correct physical
address for each segment, and receives an ordered tag.

Zero-length scatter-gather segments are rejected because they do not represent
a valid DMA transfer segment.


## Day 9 — IOMMU and TLB

Today I implemented an architectural IOMMU model that translates device-visible
I/O virtual addresses (IOVAs) into physical addresses using a page-based
mapping system. The model uses a 4096-byte page size and preserves the page
offset during address translation.

I also implemented a translation lookaside buffer (TLB) that caches recent
IOVA-to-physical-page translations. A translation that is not present in the
TLB causes a simulated page-table walk, after which the translation is inserted
into the TLB. Subsequent accesses to the same page can therefore be completed
as TLB hits without another page-table lookup.

The simulator tracks TLB hits, TLB misses, page walks, and translation faults.
Unmapped IOVAs generate translation faults, while invalid page-aligned mappings
are rejected.

I also tested the interaction between scatter-gather DMA and the IOMMU by
representing scatter-gather segment addresses as IOVAs and translating each
segment into its corresponding physical address. This establishes the address
translation stage between the DMA scatter-gather layer and the later PCIe
transaction model.


## Day 10 — Outstanding Requests and Out-of-Order Completions

Today I implemented an outstanding-request tracker for the PCIe DMA
architecture. The tracker allocates PCIe request tags, stores requests that
are currently in flight, and matches returning completions to the original
request using the tag.

The model supports a configurable maximum number of outstanding requests.
This allows the simulator to represent different levels of DMA request
concurrency, from a single in-flight request to dozens of concurrent
transactions.

I also implemented out-of-order completion handling. Eight memory read
requests were issued and their completions were deliberately returned in a
different order. The tracker correctly matched every completion to its
original request using the PCIe tag rather than assuming completion order
matched issue order.

The implementation also detects duplicate or unknown completions and tracks
statistics for issued requests, completed requests, outstanding requests, and
invalid completions.

A concurrency experiment verified the request tracker at concurrency levels
of 1, 2, 4, 8, 16, 32, and 64 outstanding requests. This establishes the
foundation for later performance experiments involving request concurrency,
latency hiding, and PCIe throughput.


## Day 11 — MSI-X and Interrupt Coalescing

Today I implemented an architectural model of MSI-X interrupt delivery for
the PCIe DMA subsystem. The model represents multiple interrupt vectors and
allows completion events to trigger a selected MSI-X vector.

I then implemented interrupt coalescing to model the tradeoff between
interrupt frequency and completion notification latency. Threshold-based
coalescing groups multiple DMA completions and generates an interrupt after
a configurable number of completions.

Timer-based coalescing was also implemented. When completions arrive but the
coalescing threshold has not been reached, a timer can generate the
interrupt after a configurable delay.

The tests verified direct MSI-X delivery, threshold-based coalescing, and
timer-based coalescing. With a threshold of four, eight completions generated
two interrupts instead of eight. This establishes the foundation for later
performance experiments comparing interrupt rate and notification latency.


## Day 12 — Error Injection and Recovery

Today I implemented controlled error injection and recovery for the PCIe DMA
subsystem simulator. The error model supports completion timeouts, TLP errors,
IOMMU translation faults, and invalid DMA descriptors.

I implemented deterministic error injection using PCIe request tags so that
specific requests can be forced to encounter a selected failure. This allows
repeatable experiments instead of relying on random failures.

I also implemented an error recovery manager that classifies errors as
retryable or non-retryable. Completion timeouts and TLP errors are modeled as
retryable, while translation faults and invalid descriptors are modeled as
abort conditions.

The tests verified four injected error types, retry behavior, successful
recovery, and request abortion. Four errors were detected, two retries were
attempted, one retry recovered successfully, and three requests were aborted.

This establishes the foundation for later experiments measuring the impact
of failures and recovery behavior on DMA performance.


## Day 13 — Performance Counters and Benchmark Infrastructure

Today I implemented a performance-counter subsystem for the PCIe DMA
architectural simulator. The counters track DMA operations, transferred
bytes, PCIe TLP activity, memory reads and writes, outstanding requests,
interrupts, TLB behavior, errors, retries, aborted requests, and latency.

I also implemented a benchmark configuration and result abstraction so that
experiments can be described using parameters such as transfer size and
request concurrency. Each benchmark produces a structured result containing
the performance-counter snapshot and simulated execution time.

The performance-counter tests verified accumulation, maximum-value tracking,
latency accounting, and counter reset behavior. The benchmark test verified
that a configured workload produces deterministic measurement results.

This establishes the measurement infrastructure required for the next stage,
where different DMA and PCIe configurations will be compared using generated
CSV data and plots.


# Day 14 — Performance Experiments and Analysis

Today I used the architectural models developed in previous days to
generate measurable experiment data and plots.

## 1. Outstanding Request Concurrency

The concurrency experiment evaluated the behavior of the outstanding
request tracker at concurrency levels of 1, 2, 4, 8, 16, 32, and 64.

The experiment verified that the request tracker can maintain the configured
number of outstanding requests and correctly track issued and completed
requests.

The current experiment measures request-tracking capacity rather than
physical PCIe latency.

## 2. IOMMU and TLB Behavior

The TLB experiment evaluated 1, 2, 4, 8, 16, 32, and 64 address translation
accesses.

The access pattern repeatedly used four virtual pages. The first access to
each page caused a TLB miss and page walk. Subsequent accesses to those
pages produced TLB hits.

For 64 accesses, the experiment produced:

- 60 TLB hits
- 4 TLB misses
- 4 page walks
- 0 translation faults

This demonstrates how TLB reuse reduces repeated page-table walks.

## 3. PCIe MPS Fragmentation

A fixed 16 KiB Memory Write transfer was fragmented using MPS values of
128, 256, 512, 1024, 2048, and 4096 bytes.

The number of generated TLPs decreased as MPS increased:

- 128-byte MPS -> 128 TLPs
- 256-byte MPS -> 64 TLPs
- 512-byte MPS -> 32 TLPs
- 1024-byte MPS -> 16 TLPs
- 2048-byte MPS -> 8 TLPs
- 4096-byte MPS -> 4 TLPs

The total payload remained 16384 bytes for every configuration.

This demonstrates the relationship between maximum payload size and PCIe
transaction fragmentation.

## 4. PCIe MRRS Fragmentation

A fixed 16 KiB Memory Read request was fragmented using MRRS values of
128, 256, 512, 1024, 2048, and 4096 bytes.

The resulting TLP counts followed the same fragmentation relationship as
the MPS experiment because the transfer started at a 4 KiB-aligned address
and the tested sizes divided the transfer evenly.

## 5. MSI-X Interrupt Coalescing

Sixteen DMA completion events were processed using coalescing thresholds
of 1, 2, 4, and 8.

The results were:

| Threshold | Interrupts | Maximum Notification Latency |
|-----------|------------|------------------------------|
| 1 | 16 | 0 cycles |
| 2 | 8 | 10 cycles |
| 4 | 4 | 30 cycles |
| 8 | 2 | 70 cycles |

Increasing the coalescing threshold reduced the number of MSI-X interrupts,
but increased the maximum notification latency.

This demonstrates the basic interrupt-rate versus notification-latency
trade-off.

## Analysis Output

The experiments generated CSV datasets under:

`benchmarks/data/`

The following plots were generated with Matplotlib:

- `concurrency.png`
- `tlb.png`
- `mps.png`
- `interrupts.png`

The plots are stored under:

`benchmarks/plots/`

These experiments provide the measurement and analysis layer required for
the final integration and verification stage of the simulator.


# Day 15 — Final Integration and Verification

Today I completed the final integration and verification stage of the
PCIe DMA simulator.

The project was reviewed as an architectural pipeline connecting the
simulation framework, memory subsystem, DMA descriptor engine, PCIe TLP
model, TLP fragmentation, scatter-gather processing, IOMMU/TLB,
outstanding request tracking, MSI-X interrupt handling, error recovery,
and performance measurement.

I verified that the project builds through CMake and that the implemented
components and benchmark infrastructure can be executed independently.

The final verification also confirmed that the benchmark data and plots
generated during Day 14 remain reproducible from the experiment programs.

The final project documentation distinguishes architectural simulation
from physical PCIe implementation and avoids claiming PCIe compliance or
real hardware performance.

The completed simulator provides a controlled environment for studying
DMA and PCIe architectural trade-offs including request concurrency,
address translation, TLP fragmentation, and interrupt coalescing.
