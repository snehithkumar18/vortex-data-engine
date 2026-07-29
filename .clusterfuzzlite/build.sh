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

NPROC=$(nproc 2>/dev/null || echo 4)
printf "%s\n" $SRC_FILES | xargs -n 1 -P $NPROC $CXX $CXXFLAGS $COMMON_FLAGS -c

ar rcs libvde.a *.o

FUZZERS="container_fuzzer codec_fuzzer stream_fuzzer record_fuzzer query_fuzzer pipeline_fuzzer"

for fuzzer in $FUZZERS; do
    if [ -f "$SRC/fuzz/${fuzzer}.cc" ]; then
        $CXX $CXXFLAGS $COMMON_FLAGS $SRC/fuzz/${fuzzer}.cc libvde.a \
            $LIB_FUZZING_ENGINE -o $OUT/${fuzzer} &
    fi
done
wait

for fuzzer in $FUZZERS; do
    if [ -d "$SRC/fuzz/corpus/${fuzzer}" ]; then
        zip -j -q $OUT/${fuzzer}_seed_corpus.zip $SRC/fuzz/corpus/${fuzzer}/*.bin &
    fi
done
wait

if [ -f "$SRC/fuzz/dictionary.txt" ]; then
    for fuzzer in $FUZZERS; do
        cp $SRC/fuzz/dictionary.txt $OUT/${fuzzer}.dict
    done
fi
