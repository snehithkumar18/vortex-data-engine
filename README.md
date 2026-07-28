# Vortex Data Engine (VDE)

A high-performance C++17 library for reading, writing, inspecting, and querying `.vdx` binary archive files.

## Overview

Vortex Data Engine is a production-grade data engine built for high-throughput telemetry processing, binary archive inspection, and stream reassembly in resource-constrained environments.

## Features

- **Extensible Container Format (`.vdx`):** Section-based file envelope supporting compressed blocks, records, indexes, metadata, and custom extensions.
- **Multiple Compression Codecs:** Built-in RLE, Delta, Huffman, and Bitpack decompression routines.
- **Stateful Stream Manager:** Out-of-order fragment reassembly with session tracking and timeout enforcement.
- **In-Memory Query Engine:** AST-based expression evaluator and binary search indexing.
- **Hermetic Fuzzing Integration:** Includes 6 LibFuzzer targets wired for ClusterFuzzLite automated QA.

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Vortex Data Engine                       │
├──────────────┬──────────────┬──────────────┬────────────────┤
│ Container    │ Codec        │ Stream       │ Query          │
│ Parser       │ Engine       │ Manager      │ Engine         │
├──────────────┼──────────────┼──────────────┼────────────────┤
│ Header &     │ RLE          │ Fragment     │ AST Expr       │
│ Sections     │ Delta        │ Reassembly   │ Evaluation     │
│ Metadata     │ Huffman      │ Sessions     │ Index Search   │
│ Inspection   │ Bitpack      │ Timeout      │ Transactions   │
└──────────────┴──────────────┴──────────────┴────────────────┘
```

## Building

```bash
mkdir build && cd build
cmake ..
make -j
```

### Enable Sanitizers & Fuzz Targets

```bash
cmake .. -DVDE_SANITIZE=ON -DVDE_FUZZ=ON
make -j
```

## Tools

- `vdx_inspect <file.vdx>`: Print container headers and section table.
- `vdx_validate <file.vdx>`: Perform structural integrity checks.
- `vdx_convert <file.vdx>`: Convert binary records to JSON format.

## License

MIT License. Copyright 2026 Vortex Systems Contributors.
