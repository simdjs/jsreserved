# jsreserved

Fast recognition of JavaScript reserved words using a gperf-generated perfect hash function.

## Overview

This project provides a lookup function that maps any of the 45 JavaScript reserved words (including future reserved words like `implements`, `interface`, etc.) to a `Token` enum value in O(1) time. Non-reserved inputs return `std::nullopt`.

## How `gperf_neon.h` works

The implementation is a modified [gperf](https://www.gnu.org/software/gperf/) output, hand-optimized for ARM NEON (AArch64).


### Fallback path

For the time being, the function is optimized for ARM processors.

## Benchmark

The `benchmark/` directory contains a harness that compares the gperf+NEON implementation against a naive if/else chain across three workloads (all hits, all misses, 50/50 mixed) on the full set of 45 JavaScript reserved words.

Build and run:

```bash
cd benchmark/build
cmake .. && make
./benchmark          # add --verify to check correctness
```
