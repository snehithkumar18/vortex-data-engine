#!/bin/bash -eu

COMMON_FLAGS="-std=c++17 -I$SRC/include -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION"

SRC_FILES=""
for dir in common container codec record stream query pipeline telemetry_registry storage transaction net; do
    if [ -d "$SRC/src/$dir" ]; then
        for src in $SRC/src/$dir
