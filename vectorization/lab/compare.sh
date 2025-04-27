#!/bin/bash

if [ $# -ne 3 ]; then
    echo "Usage: $0 <M> <K> <N>"
    exit 1
fi

M=$1
K=$2
N=$3

echo "Running matrix multiplication..."
./main $M $K $N

echo "--------------------"

echo "Comparing output with expected results..."
./float-diff c.bin ans.bin

echo "Comparison complete."
