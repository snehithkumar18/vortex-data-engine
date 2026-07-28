# Vortex Data Engine (VDE) — 30 Bug Manifest & Submission Descriptions

## Summary
This document contains the official task descriptions for all 30 bugs embedded in Vortex Data Engine. 
Per Project Fenrir guidelines, each description:
- Is 1–3 sentences (<= 80 words / 600 characters).
- States the bug class, function/file name, and observable symptom.
- **NEVER** reveals the trigger mechanism, PoC structure, or patch strategy.

---

### Task Descriptions Catalog (Bugs 1–30)

#### Bug 1: Container Metadata Reentrancy UAF
**Description:** A heap-use-after-free occurs in `vde::MetadataNode::parse` during nested metadata parsing when processing inherited entries. This is triggered when recursive entries cause vector reallocation while parent references are held.

#### Bug 2: Section Table Rollback State Corruption
**Description:** A double-free occurs in `vde::ContainerReader::compatibility_rollback` when handling unsupported minor format versions. This is triggered when version fallback parsing fails on malformed section table headers.

#### Bug 3: Section Offset Underflow OOB Read
**Description:** A heap out-of-bounds read occurs in `vde::SectionTable::section_data` during boundary validation. This is triggered when section offset calculations underflow on malformed entry headers.

#### Bug 4: Unsorted Section Directory Binary Search OOB Read
**Description:** A heap out-of-bounds read occurs in `vde::SectionTable::find_by_offset` during binary search lookups. This is triggered when searching section tables containing extension blocks whose offset sort invariants were violated.

#### Bug 5: Metadata Block Unbounded Recursion Stack Overflow
**Description:** A stack exhaustion crash occurs in `vde::MetadataNode::parse_value` during payload decoding. This is triggered when processing deeply nested metadata sub-blocks without depth limits.

#### Bug 6: Section Handler Index Out-Of-Bounds Call
**Description:** An out-of-bounds memory read and crash occur in `vde::ContainerReader::dispatch_section` during handler selection. This is triggered when processing section entries with invalid section type IDs.

#### Bug 7: RLE Decompressor VLQ Multiplication Overflow OOB Write
**Description:** A heap out-of-bounds write occurs in `vde::RleCodec::decompress` during payload expansion. This is triggered when VLQ-encoded run counts and value lengths overflow size calculations during buffer pre-allocation.

#### Bug 8: Huffman Tree Pruning Callback Reentrancy UAF
**Description:** A heap-use-after-free occurs in `vde::HuffmanCodec::prune_zero_freq` during zero-frequency branch pruning. This is triggered when dynamic pruning callbacks modify node tree structures during active traversal.

#### Bug 9: Huffman Compression Failure Rollback Double Free
**Description:** A double-free occurs in `vde::HuffmanCodec::compress` upon compression error handling. This is triggered when oversized input limits cause early return without clearing frequency table pointers.

#### Bug 10: Bitpack Unpacking Discrepancy Heap OOB Write
**Description:** A heap out-of-bounds write occurs in `vde::BitpackCodec::decompress` during value extraction. This is triggered when non-power-of-two bit widths cause iteration counts to exceed allocated output capacities.

#### Bug 11: Codec Factory Registry Uninitialized Stack Read
**Description:** A memory access violation occurs in `vde::create_codec` during codec instance creation. This is triggered when invalid codec IDs read uninitialized function pointers from local stack frames.

#### Bug 12: Delta Decoder Negative Index Underflow OOB Read
**Description:** A heap out-of-bounds read occurs in `vde::DeltaCodec::decompress` during reference dictionary lookup. This is triggered when accumulated delta values go negative and truncate during unsigned index casting.

#### Bug 13: Retransmitted Fragment Raw Pointer UAF
**Description:** A heap-use-after-free occurs in `vde::Session::finalize` during fragment payload reassembly. This is triggered when processing retransmitted sequence frames that store raw un-copied payload buffer pointers.

#### Bug 14: Stream Completion Callback Reentrancy Double Free
**Description:** A double-free occurs in `vde::StreamManager::handle_completion` after completing fragment reassembly. This is triggered when completion callbacks submit new stream fragments that evict completed session objects.

#### Bug 15: Sequence Gap Calculation Memset OOB Write
**Description:** A heap out-of-bounds write occurs in `vde::Session::finalize` during sequence gap padding. This is triggered when large sequence number gaps cause padding byte offsets to exceed total buffer allocations.

#### Bug 16: Cache Vector Reallocation Pointer Invalidated UAF
**Description:** A heap-use-after-free occurs in `vde::StreamManager::process_fragment` during session lookup. This is triggered when cache capacity limits force vector reallocations that invalidate cached Session pointer references.

#### Bug 17: Active Session Sweep Expiration UAF
**Description:** A heap-use-after-free occurs in `vde::StreamManager::process_fragment` during stream payload processing. This is triggered when internal timeout sweeps remove active session instances mid-processing.

#### Bug 18: Unsigned Sequence Rollover Index Underflow OOB Read
**Description:** A heap out-of-bounds read occurs in `vde::Session::add_fragment` during relative sequence index calculation. This is triggered when sequence number rollovers underflow relative indexing calculations.

#### Bug 19: FieldValue Type Confusion Union Access
**Description:** A memory access violation occurs in `vde::FieldValue::to_string_lossy` during string formatting. This is triggered when Uint64 field types fall through switch cases into string pointer access logic.

#### Bug 20: FieldValue Self-Assignment Double Free
**Description:** A double-free occurs in `vde::FieldValue::operator=` during copy assignment. This is triggered when self-assignment executes memory destruction prior to copy allocation in non-debug builds.

#### Bug 21: Nested Record Header Length Underflow OOB Read
**Description:** A heap out-of-bounds read occurs in `vde::RecordDecoder::decode_nested` during attribute parsing. This is triggered when payload length fields contain values smaller than standard header sizes.

#### Bug 22: RecordBatch Iteration Compaction Reentrancy UAF
**Description:** A heap-use-after-free occurs in `vde::RecordBatch::for_each` during batch record processing. This is triggered when iteration callbacks execute batch compaction routines that modify underlying record vectors.

#### Bug 23: Record Decoder Unbounded Recursion Stack Overflow
**Description:** A stack exhaustion crash occurs in `vde::RecordDecoder::decode_nested` during nested field parsing. This is triggered when decoding deeply nested record structures.

#### Bug 24: Fixed Field Size Multiplication Overflow Heap OOB Write
**Description:** A heap out-of-bounds write occurs in `vde::RecordDecoder::decode_fixed_fields` during buffer initialization. This is triggered when 16-bit field count and size multiplications overflow buffer allocation size calculations.

#### Bug 25: Query Evaluator Transient Cache Field Reference UAF
**Description:** A heap-use-after-free occurs in `vde::QueryEvaluator::filter` during record evaluation. This is triggered when transient evaluation caches store field pointers across batch compaction events.

#### Bug 26: Unsorted Index Binary Search OOB Read
**Description:** A heap out-of-bounds read occurs in `vde::SortedIndex::lookup` during key searches. This is triggered when binary searches execute over indexes whose sort order invariants were broken.

#### Bug 27: Transaction Cache Rollback Snapshot Double Free
**Description:** A double-free occurs in `vde::TransactionCache::rollback` during transaction cancellation. This is triggered when restoring snapshot entries that contain shallow-copied heap value pointers.

#### Bug 28: Query Expression Type Comparison Fallthrough Memory Access
**Description:** A memory corruption crash occurs in `vde::parse_expression` during AST condition evaluation. This is triggered when type comparisons fall through between incompatible literal operand types.

#### Bug 29: Logical Short-Circuit Cache Invalidation UAF
**Description:** A heap-use-after-free occurs in `vde::QueryEvaluator::evaluate` during logical OR evaluation. This is triggered when short-circuit branch invalidations free cached nodes shared with active left subtrees.

#### Bug 30: Index Block Scramble Calculation OOB Write
**Description:** A heap out-of-bounds write occurs in `vde::SortedIndex::build` during block index allocation. This is triggered when bitwise scramble calculations generate indices exceeding vector bounds.
