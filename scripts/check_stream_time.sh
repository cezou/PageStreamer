#!/bin/bash

# ---------------------------------------------------------------------
# @brief Checks if the current time is within the streaming schedule
#
# This script determines if we're within the scheduled streaming hours
# (Friday-Sunday 20:30 to Monday 3:30) and if the stream process is
# currently running. Returns success (0) only if both conditions are met.
# ---------------------------------------------------------------------

# Get script directories
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PARENT_DIR="$( cd "$DIR/.." && pwd )"

# Get current day of week (1-7, where 1 is Monday)
DAY_OF_WEEK=$(date +%u)
# Get current time (24h format)
HOUR=$(date +%H)
MINUTE=$(date +%M)
# Calculate time in minutes since midnight
CURRENT_TIME=$((HOUR * 60 + MINUTE))

# Convert start and end times to minutes since midnight
START_TIME=$((20 * 60 + 30))  # 20:30
END_TIME=$((3 * 60 + 30))     # 03:30

# Check if it's a streaming day (Friday=5, Saturday=6, Sunday=7)
IS_STREAM_DAY=0
if [ "$DAY_OF_WEEK" -eq 5 ] || [ "$DAY_OF_WEEK" -eq 6 ] || [ "$DAY_OF_WEEK" -eq 7 ]; then
    IS_STREAM_DAY=1
fi

# Also check Monday (day 1) for end of stream
if [ "$DAY_OF_WEEK" -eq 1 ] && [ "$CURRENT_TIME" -le "$END_TIME" ]; then
    IS_STREAM_DAY=1
fi

# Check if we're within streaming hours
IS_STREAM_TIME=0
if [ "$IS_STREAM_DAY" -eq 1 ]; then
    # If current time is after start time or before end time
    if [ "$CURRENT_TIME" -ge "$START_TIME" ] || [ "$CURRENT_TIME" -le "$END_TIME" ]; then
        IS_STREAM_TIME=1
    fi
fi

# Check if stream.js process is running
IS_PROCESS_RUNNING=0
if pgrep -f "node stream.js" > /dev/null || [ -f "$DIR/stream.pid" ] && ps -p $(cat "$DIR/stream.pid" 2>/dev/null) > /dev/null 2>&1; then
    IS_PROCESS_RUNNING=1
fi

# Return success (0) if we are in streaming period AND process is running
if [ "$IS_STREAM_TIME" -eq 1 ] && [ "$IS_PROCESS_RUNNING" -eq 1 ]; then
    echo "Within streaming period and process is running."
    exit 0
else
    # Provide details about why this is not a streaming period
    if [ "$IS_STREAM_TIME" -eq 0 ]; then
        echo "Outside streaming period (day=$DAY_OF_WEEK, time=$HOUR:$MINUTE)."
    fi
    if [ "$IS_PROCESS_RUNNING" -eq 0 ]; then
        echo "Stream process is not running."
    fi
    exit 1
fi
