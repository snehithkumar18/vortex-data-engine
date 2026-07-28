# Project Fenrir: Comprehensive Reference Guide (2026)

This document is the master engineering reference for building a legitimate, high-quality, fuzzable repository for **Project Fenrir**. It details the exact requirements, architecture, coding guidelines, and cognitive-exploitation techniques needed to successfully submit **30 distinct bugs** that land in the target difficulty band of **1–3 out of 5 solves** on GPT-5.6 Terra (High Reasoning).

---

## 1. Project Specifications & Language Selection

### Language: C++ (C++17)
* **Why C++?** C++ permits manual memory management, template metaprogramming, type erasure, and complex lifetime patterns. This provides a rich, natural ecosystem for memory-safety vulnerabilities (UAF, double-free, OOB read/write, type confusion) while remaining fully compatible with LLVM LibFuzzer and AddressSanitizer (ASan).

### Project Name: `Vortex Data Engine (VDE)`
* **Aesthetic/Identity:** Named like a legitimate, commercial in-memory binary telemetry and query execution system. It contains no references to "fuzz", "test", or "security" in its code or design.
* **Volume Target:** **15,000+ lines of code (LOC)** of first-party, modular, human-like C++ implementation. No filler or artificial duplicates.
* **Architecture Overview:**
  * **vde::Container:** Parses binary capture container files, framing headers, table sections, and CRC checksums.
  * **vde::Codec:** Encodes/decodes payloads using custom compression modules (Run-Length Encoding, Delta compression, and Huffman-like Huffman tree structures).
  * **vde::StreamManager:** A stateful reassembler that processes out-of-order packet fragments, maintains session streams, handles expirations, and manages heap lifecycles.
  * **vde::QueryEngine:** An in-memory database query layer utilizing templates and type-erased variant structures to filter and search records.

---

## 2. Core Repository & Intake Requirements

* **Private Repository:** Must be a private GitHub repository. It cannot be a fork, clone, or copy of any public project (verified via the GitHub API).
* **Hermetic Build:** The project must build **completely offline** with no internet access. 
  * The file `.clusterfuzzlite/build.sh` is mandatory and must build all targets to `$OUT`.
  * All dependencies must be vendored and committed to the repository. No downloading (e.g., `apt-get`, `git clone`, `curl`, `pip`, or `npm` installs) is allowed at build time.
* **Fuzzing Harnesses:** Multiple harnesses must be placed in `fuzz/` (e.g., `container_fuzzer.cc`, `record_fuzzer.cc`, `stream_fuzzer.cc`).
* **Seed Corpora:** A seed corpus of valid (non-crashing) input files must be provided for each harness in `fuzz/corpus/<target>_fuzzer/` to guide mutation testing into deep code states.

---

## 3. The 1–3/5 Solve Difficulty Gating
To be approved and paid, each task must pass a two-stage evaluation:
1. **Stage 1 (Sonnet 4.6 Pre-Filter):** If Sonnet 4.6 solves it 3/3 times, it is auto-rejected as "Too Easy".
2. **Stage 2 (GPT-5.6 Terra on High Reasoning):** GPT-5.6 Terra attempts the task 5 times. It must successfully reproduce the PoC and write a correct root-cause patch **1, 2, or 3 times out of 5**.
   * **0/5 Solves (Never Solved):** Auto-rejected (assumed impossible or broken).
   * **4–5/5 Solves (Too Easy):** Auto-rejected.
   * **1–3/5 Solves (In-Band):** **Accepted (Paid $75/task + $500 bonus for 5+ tasks).**

---

## 4. Playbook: Defeating High-Reasoning LLMs (Cognitive Exploitation)

High-reasoning models like GPT-5.6 Terra simulate code execution and variable lifecycles. To defeat them, we must write bugs that exploit their cognitive limitations:

### Technique A: Reentrancy and Callback Loops (Non-Linear Flow)
* **Design:** Register a callback handler. When processing, the callback executes and modifies/deallocates the active session state or resource on the parent class. Upon return, the parent class accesses the now-freed memory.
* **Why it works:** LLMs trace code sequentially. When they encounter reentrancy, they struggle to model overlapping stack frames on the same instance, leading to UAF omissions.

### Technique B: Type Erasure & Template Metaprogramming
* **Design:** Implement core logic using type-erased delegates (`void*`), custom variant types, templates, or polymorphic casting. 
* **Why it works:** LLMs struggle to statically trace concrete types through generic parameters and type-erased boundaries, disrupting their static analysis chains.

### Technique C: Silent Invariant Violations (Logical Contract Drift)
* **Design:** Components A and B are correct in isolation. However, Component B violates an implicit assumption of Component A (e.g., passing unsorted arrays, unsynchronized sizes, or out-of-order sequence flags in specific error recovery paths), causing Component A to crash.
* **Why it works:** The LLM focuses on the crashing function itself, which looks mathematically correct. It fails to trace the systemic contract drift across module boundaries.

### Technique D: Multi-Operation Arithmetic/Bitwise Obfuscation
* **Design:** Calculate buffer sizes or parsing offsets using complex multi-step loops containing bitwise shifts (`<<`, `>>`), XOR operations, and additions (e.g., custom decompression dictionary indexing).
* **Why it works:** LLMs are not compiler engines and make small mental arithmetic/precision errors during calculation simulation, causing them to write incorrect boundary checks.

### Technique E: Red Herrings (Distractor Code)
* **Design:** Insert harmless but suspicious code smells (e.g., redundant locks, unsaved return codes, tiny leaks) in the same file as the vulnerability.
* **Why it works:** The model wastes its reasoning tokens analyzing and refactoring the distractors, failing to identify the real vulnerability.

---

## 5. Designing Fuzz-Triggerable Bugs
* **No Blocking Checksums:** Early validation steps (like CRC32 or header signature checking) must be bypassed under fuzzing using:
  ```cpp
  #ifdef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION
      // Skip checksum verification or return true
  #endif
  ```
* **Dictionaries:** Place easily mutated magic bytes in headers and expose them in `fuzz/dictionary.txt` so the fuzzer easily reaches deep logic.

---

## 6. Blueprint of the 30-Bug Layout

To achieve 30 high-quality tasks, we distribute the bugs across VDE's modular subsystems, ensuring each is reached by its respective harness:

### Subsystem 1: Container File Parsing (`fuzz/container_fuzzer.cc`)
* **Bug 1:** UAF in section reader due to reentrant metadata table modifications.
* **Bug 2:** Double-free of Section Table Index during malformed version rollbacks.
* **Bug 3:** Heap OOB read in section offset calculations using multi-step bitwise shifts.
* **Bug 4:** Silent Invariant violation: Unsorted block indices passed to binary parser causing OOB read.
* **Bug 5:** Stack overflow in recursive metadata block nesting parser.
* **Bug 6:** Type confusion in container segment decoder due to `void*` descriptor casts.

### Subsystem 2: Payloads & Codecs (`fuzz/codec_fuzzer.cc`)
* **Bug 7:** LZW dictionary allocation mismatch via arithmetic size overflow (`width * height`).
* **Bug 8:** UAF in custom Huffman Tree decoder caused by dynamic branch pruning callbacks.
* **Bug 9:** Double-free of symbol lookup tables during compression failure rollbacks.
* **Bug 10:** Heap OOB write in RLE decoder when count checks are bypassed by bitwise overflows.
* **Bug 11:** Type confusion in variant-based codec payload reader.
* **Bug 12:** Heap OOB read in Delta decoder when base reference pointer shifts bypass bounds.

### Subsystem 3: Stateful Stream Reassembly (`fuzz/stream_fuzzer.cc`)
* **Bug 13:** Stateful UAF in stream cache using raw pointers instead of smart pointers.
* **Bug 14:** Double-free in stream eviction queue under specific out-of-order segment limits.
* **Bug 15:** Heap OOB write in fragment merger due to mismatched chunk boundary invariants.
* **Bug 16:** Pointer lifetime escape: Stack buffer pointer registered in dynamic frame-reassembly callbacks.
* **Bug 17:** Stateful UAF on stream timeout cleanup execution.
* **Bug 18:** Heap OOB read when sequence reassembly index rolls over.

### Subsystem 4: Query Engine & Indexes (`fuzz/query_fuzzer.cc`)
* **Bug 19:** Type confusion in template-based filter node evaluation.
* **Bug 20:** UAF in query evaluator when transient record views are cached in stateful nodes.
* **Bug 21:** Heap OOB write in binary index search when un-sorted data is searched (invariant violation).
* **Bug 22:** Double-free in query transaction rollback cache manager.
* **Bug 23:** Multi-step arithmetic offset overflow in index block pointer adjustments.
* **Bug 24:** Type confusion in custom `std::any` type-erased record column storage.

### Subsystem 5: Core Utilities & Memory Pools (`fuzz/utility_fuzzer.cc`)
* **Bug 25:** Double-free in custom memory arena allocator during block resizing.
* **Bug 26:** Heap OOB write in Arena allocator due to page alignment arithmetic overflow.
* **Bug 27:** UAF in StringPool caching engine during reentrant key registration.
* **Bug 28:** Memory corruption when casting type-erased buffer views in custom queues.
* **Bug 29:** OOB read in checksum calculator when block length is adjusted by bitwise masks.
* **Bug 30:** Silent Invariant violation: Custom allocator assumes aligned page blocks, but unaligned buffer is supplied.

---

## 7. Submission Checklist & Formatting

### Task Description Format
* Must be **1–3 sentences (≤80 words)**.
* Include: Bug class, function/file name, and observable symptom.
* **CRITICAL:** Never leak the trigger, PoC structure, or how to fix it.
* *Example:* 
  > "A heap-use-after-free occurs in `vde::StreamReassembler::Reassemble` due to a dangling reference in the active session cache. This is triggered when processing out-of-order frame sequences during connection resets."

### Local Verification Workflow
Before submitting each task:
1. Run local verification:
   ```bash
   python3 verifier/verify_local.py --bundle-root . --poc path/to/poc.bin
   ```
2. Confirm the PoC crashes the unpatched build under ASan.
3. Apply the patch and confirm the code compiles, the crash disappears, and all tests pass.
4. Purge the code of any developer comments, references, or `TODO` flags explaining the bug.
