# Experiment Summary

## Outstanding Request Concurrency

The concurrency experiment verified that the outstanding request tracker
can maintain increasing numbers of simultaneously tracked requests.

The experiment measures request-tracking capacity rather than physical
PCIe latency.

## IOMMU / TLB

Repeated accesses to four virtual pages demonstrated the transition from
initial TLB misses and page walks to TLB hits.

For 64 accesses:

- TLB hits: 60
- TLB misses: 4
- Page walks: 4
- Translation faults: 0

## MPS

For a fixed 16 KiB memory-write transfer, increasing MPS reduced the
number of generated Memory Write TLPs:

- 128 B: 128 TLPs
- 256 B: 64 TLPs
- 512 B: 32 TLPs
- 1024 B: 16 TLPs
- 2048 B: 8 TLPs
- 4096 B: 4 TLPs

## MRRS

For a fixed 16 KiB memory-read request, increasing MRRS reduced the
number of generated Memory Read requests:

- 128 B: 128 TLPs
- 256 B: 64 TLPs
- 512 B: 32 TLPs
- 1024 B: 16 TLPs
- 2048 B: 8 TLPs
- 4096 B: 4 TLPs

## MSI-X Interrupt Coalescing

For 16 completions:

| Threshold | Interrupts | Maximum notification latency |
|-----------|------------|------------------------------|
| 1         | 16         | 0                            |
| 2         | 8          | 10                           |
| 4         | 4          | 30                           |
| 8         | 2          | 70                           |

The experiment demonstrates the fundamental trade-off between interrupt
frequency and notification latency.

## Interpretation

The experiments demonstrate architectural relationships rather than
claims about physical PCIe hardware performance.

The simulator provides a controlled environment for studying DMA,
translation, PCIe transaction fragmentation, request concurrency, and
interrupt behavior.
