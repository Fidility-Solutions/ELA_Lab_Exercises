#!/bin/sh

# Device to write to (ensure you use the correct device identifier)
DEVICE="/dev/sda1"

# Check if the device exists
if [ ! -e "$DEVICE" ]; then
  echo "Error: Device $DEVICE does not exist."
  exit 1
fi

# Data to write
DATA="This is a test"

# Continuous loop
while true; do
  echo "Writing to device..."
  echo "$DATA" | dd of="$DEVICE" bs=512 count=1
  echo "Data written to $DEVICE"

  # Sleep for 1 second
  sleep 1

  # Reading data from device
  echo "Reading from device..."
  OUTPUT=$(dd if="$DEVICE" bs=512 count=1 2>/dev/null)
  echo "Content read from $DEVICE: $OUTPUT"

  # Sleep for 1 second
  sleep 1
done

