#!/bin/bash -eu

# Vortex Data Engine - ClusterFuzzLite hermetic build script
# Builds all fuzz targets into $OUT without network dependencies

COMMON_FLAGS="-std=c++17 -I$SRC/include -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION"

# Compile all source files
SRC_FILES=""
for dir in common container codec record stream query pipeline; do
    for src in $SRC/src/$dir/*.cc; do
        if [ -f "$src" ]; then
            obj_name=$(basename "$src" .cc).o
            $CXX $CXXFLAGS $COMMON_FLAGS -c "$src" -o "$obj_name"
            SRC_FILES="$SRC_FILES $obj_name"
        fi
    done
done

# Build each fuzzer
for fuzzer in $SRC/fuzz/*_fuzzer.cc; do
    fuzzer_name=$(basename "$fuzzer" .cc)
    $CXX $CXXFLAGS $COMMON_FLAGS $LIB_FUZZING_ENGINE "$fuzzer" $SRC_FILES -o "$OUT/$fuzzer_name"
    
    # Package seed corpus if present
    corpus_dir="$SRC/fuzz/corpus/$fuzzer_name"
    if [ -d "$corpus_dir" ] && [ "$(ls -A $corpus_dir 2>/dev/null)" ]; then
        zip -j "$OUT/${fuzzer_name}_seed_corpus.zip" "$corpus_dir"/*
    fi
done

# Copy dictionary
if [ -f "$SRC/fuzz/dictionary.txt" ]; then
    for fuzzer in $SRC/fuzz/*_fuzzer.cc; do
        fuzzer_name=$(basename "$fuzzer" .cc)
        cp "$SRC/fuzz/dictionary.txt" "$OUT/${fuzzer_name}.dict"
    done
fi
