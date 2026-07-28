#!/bin/bash -eu

# Vortex Data Engine - ClusterFuzzLite hermetic build script
# Builds all fuzz targets into $OUT without network dependencies

COMMON_FLAGS="-std=c++17 -I$SRC/include -DFUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION"

# Compile all source files
SRC_FILES=""
for dir in common container codec record stream query pipeline; do
    for src in $SRC/src/$dir
