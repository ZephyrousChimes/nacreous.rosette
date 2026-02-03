# nacreous.rosette

User-space transport runtime (DPDK).

---

## What this is

`nacreous.rosette` started as part of my learning process with DPDK
and kernel-bypass networking.

Instead of keeping it as isolated experiments, I began shaping it into a
reusable transport runtime that could serve as the networking substrate
for `nacreous.lute`.

It is currently a static library used inside that project.

---

## Current State

This repository is actively evolving.

- Interfaces are unstable.
- Internal structure will change.
- Some components are minimal or incomplete.
- Code will be refactored as understanding improves.

It should be treated as infrastructure under construction.

The goal is not to provide a finished transport stack, but to
incrementally build a NUMA-aware, poll-mode transport runtime
that can expose multiple channel abstractions.

---

## Intent

Long-term, this library should provide:

- Kernel-bypass I/O via DPDK
- Explicit queue-to-core mapping
- NUMA-aligned memory allocation
- Zero-copy packet handling
- A growing set of channel types (unordered, reliable, streaming, etc.)
- Clear separation between I/O substrate and delivery semantics

Right now only a minimal channel implementation exists.

More structured channel types will be introduced as
`nacreous.lute` evolves and requires different delivery semantics.

---

## Relationship to nacreous.lute

This is not meant to be a standalone networking project.

It is embedded into `nacreous.lute` and will continue to evolve
alongside its transport and ingestion requirements.

Channel abstractions here are driven by the needs of that system.

---

## Why it's public in this state

The repository reflects an active learning and iteration process.

Design decisions, interfaces, and structure may change as:

- DPDK understanding deepens
- Performance profiling is added
- NUMA and scheduling strategies are refined
- Channel requirements become clearer

It is intentionally structured for growth, not stability.

---

## Build

Requires DPDK (EAL + hugepages configured).

Built as a static library via CMake.
