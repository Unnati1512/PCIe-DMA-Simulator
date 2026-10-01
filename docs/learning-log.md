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
