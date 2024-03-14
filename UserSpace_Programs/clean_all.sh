#!/bin/bash

# Find all directories containing a Makefile
directories=$(find . -type f -name Makefile -exec dirname {} \;)

# Execute 'make clean' in each directory
for dir in $directories; do
    (cd "$dir" && make clean)
done

