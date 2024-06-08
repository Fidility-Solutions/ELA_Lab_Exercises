#!/bin/sh

# Display initial free memory
echo "Initial memory usage:"
free -m

# Function to allocate memory
allocate_memory() {
    echo "Allocating memory..."
    while true; do
        # Allocate 1024MB of memory at a time
        dd if=/dev/zero of=/tmp/memory_hog bs=1024M count=1 oflag=append conv=notrunc 2>/dev/null
        sleep 1
    done
}

# Start memory allocation in the background
allocate_memory &

# Get the PID of the background process
MEMORY_HOG_PID=$!

# Monitor memory usage
while true; do
    # Display current free memory
    free -m
    # Check if the memory hog process is still running
    if ! ps -p $MEMORY_HOG_PID > /dev/null; then
        echo "Memory hog process has been killed by OOM Killer"
        break
    fi
    sleep 2
done

# Clean up
echo "Cleaning up..."
rm /tmp/memory_hog
echo "Done."

# Display final memory usage
echo "Final memory usage:"
free -m

