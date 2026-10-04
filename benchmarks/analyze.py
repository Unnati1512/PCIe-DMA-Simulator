import csv
from pathlib import Path

import matplotlib.pyplot as plt


DATA_DIR = Path("benchmarks/data")
PLOT_DIR = Path("benchmarks/plots")

PLOT_DIR.mkdir(parents=True, exist_ok=True)


def read_csv(filename):
    with open(DATA_DIR / filename, newline="") as file:
        return list(csv.DictReader(file))


# ------------------------------------------------------------
# Experiment 1: Outstanding-request concurrency
# ------------------------------------------------------------

rows = read_csv("concurrency.csv")

concurrency = [
    int(row["concurrency"])
    for row in rows
]

maximum_outstanding = [
    int(row["maximum_outstanding"])
    for row in rows
]

plt.figure()
plt.plot(
    concurrency,
    maximum_outstanding,
    marker="o"
)
plt.xlabel("Concurrency")
plt.ylabel("Maximum Outstanding Requests")
plt.title("Outstanding Requests vs Concurrency")
plt.grid(True)
plt.tight_layout()
plt.savefig(PLOT_DIR / "concurrency.png", dpi=150)
plt.close()


# ------------------------------------------------------------
# Experiment 2: IOMMU / TLB behavior
# ------------------------------------------------------------

rows = read_csv("tlb.csv")

accesses = [
    int(row["accesses"])
    for row in rows
]

tlb_hits = [
    int(row["tlb_hits"])
    for row in rows
]

tlb_misses = [
    int(row["tlb_misses"])
    for row in rows
]

plt.figure()
plt.plot(
    accesses,
    tlb_hits,
    marker="o",
    label="TLB Hits"
)
plt.plot(
    accesses,
    tlb_misses,
    marker="o",
    label="TLB Misses"
)
plt.xlabel("Memory Translation Accesses")
plt.ylabel("Count")
plt.title("IOMMU TLB Behavior")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig(PLOT_DIR / "tlb.png", dpi=150)
plt.close()


# ------------------------------------------------------------
# Experiment 3: MPS and MRRS fragmentation
# ------------------------------------------------------------

mps_rows = read_csv("mps.csv")
mrrs_rows = read_csv("mrrs.csv")

mps = [
    int(row["mps"])
    for row in mps_rows
]

mps_tlps = [
    int(row["tlp_count"])
    for row in mps_rows
]

mrrs = [
    int(row["mrrs"])
    for row in mrrs_rows
]

mrrs_tlps = [
    int(row["tlp_count"])
    for row in mrrs_rows
]

plt.figure()
plt.plot(
    mps,
    mps_tlps,
    marker="o",
    label="Memory Write / MPS"
)
plt.plot(
    mrrs,
    mrrs_tlps,
    marker="o",
    label="Memory Read / MRRS"
)
plt.xlabel("MPS / MRRS (bytes)")
plt.ylabel("Number of TLPs")
plt.title("PCIe TLP Fragmentation vs MPS / MRRS")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig(PLOT_DIR / "mps.png", dpi=150)
plt.close()


# ------------------------------------------------------------
# Experiment 4: MSI-X interrupt coalescing
# ------------------------------------------------------------

rows = read_csv("interrupts.csv")

threshold = [
    int(row["threshold"])
    for row in rows
]

interrupts = [
    int(row["interrupts"])
    for row in rows
]

max_latency = [
    int(row["max_notification_latency"])
    for row in rows
]

plt.figure()
plt.plot(
    threshold,
    interrupts,
    marker="o",
    label="MSI-X Interrupts"
)
plt.plot(
    threshold,
    max_latency,
    marker="o",
    label="Maximum Notification Latency"
)
plt.xlabel("Coalescing Threshold")
plt.ylabel("Count / Cycles")
plt.title("MSI-X Interrupt Coalescing Trade-off")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig(PLOT_DIR / "interrupts.png", dpi=150)
plt.close()


print("Generated plots:")
print("  benchmarks/plots/concurrency.png")
print("  benchmarks/plots/tlb.png")
print("  benchmarks/plots/mps.png")
print("  benchmarks/plots/interrupts.png")
