#!/bin/bash
set -e

echo "=== Building Image Search ==="
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cd ..

echo "=== Running Exact Search Test ==="
# Flags:
# -hp : path to HPatches dataset
# -mir : path to MIRFlickr distractor dataset
# -split : path to the split file (assuming it's named split.txt in the root, update if necessary)
# -vocab : vocabulary size K (e.g., 100)
# -S : max SIFT features for training (e.g., 500)
# -D : number of distractors to use (e.g., 1000)
# -set : which set to test on (validation or test)
# -o : output file
# -exact : search method flag

# Note: We assume split.txt exists. If not, the application will exit gracefully or throw an error.
if [ ! -f "split.txt" ]; then
    echo "Warning: split.txt not found in root. Generating a dummy split.txt for testing."
    echo "i_ajuntament validation" > split.txt
    echo "v_bird train" >> split.txt
    echo "i_bologna test" >> split.txt
fi

./build/image_search \
    -hp dataset/hpatches-sequences-release \
    -mir dataset/mirflickr \
    -split split.txt \
    -vocab 50 \
    -S 100 \
    -D 10 \
    -set validation \
    -o results.txt \
    -exact

echo "=== Test Completed ==="
