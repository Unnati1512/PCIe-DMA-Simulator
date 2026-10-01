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
