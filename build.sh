#!/bin/bash -eu

COMMON_FLAGS="-std=c++17 -I$SRC/include -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION"

SRC_FILES=""
for dir in common container codec record stream query pipeline catalog storage transaction net; do
    if [ -d "$SRC/src/$dir" ]; then
        for src in $SRC/src/$dir/*.cc; do
            if [ -f "$src" ]; then
                SRC_FILES="$SRC_FILES $src"
            fi
        done
    fi
done

$CXX $CXXFLAGS $COMMON_FLAGS -c $SRC_FILES

FUZZERS="container_fuzzer codec_fuzzer stream_fuzzer record_fuzzer query_fuzzer pipeline_fuzzer"

for fuzzer in $FUZZERS; do
    if [ -f "$SRC/fuzz/${fuzzer}.cc" ]; then
        $CXX $CXXFLAGS $COMMON_FLAGS *.o $SRC/fuzz/${fuzzer}.cc \
            $LIB_FUZZING_ENGINE -o $OUT/${fuzzer}
            
        if [ -d "$SRC/fuzz/corpus/${fuzzer}" ]; then
            zip -j $OUT/${fuzzer}_seed_corpus.zip $SRC/fuzz/corpus/${fuzzer}/*.bin
        fi
    fi
done

if [ -f "$SRC/fuzz/dictionary.txt" ]; then
    cp $SRC/fuzz/dictionary.txt $OUT/container_fuzzer.dict
    cp $SRC/fuzz/dictionary.txt $OUT/codec_fuzzer.dict
    cp $SRC/fuzz/dictionary.txt $OUT/stream_fuzzer.dict
    cp $SRC/fuzz/dictionary.txt $OUT/record_fuzzer.dict
    cp $SRC/fuzz/dictionary.txt $OUT/query_fuzzer.dict
    cp $SRC/fuzz/dictionary.txt $OUT/pipeline_fuzzer.dict
fi
