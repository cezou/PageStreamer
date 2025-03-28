# RouletteTV Streaming System

## Overview

This project automates streaming of the [RouletteTV History page](https://roulette-tv.vercel.app/history) to YouTube. It sets up a headless browser environment that captures a web page and streams it to YouTube on a scheduled basis.

## Features

- **Automated Streaming**: Runs automatically at scheduled times
- **Headless Browser**: Uses Puppeteer to render web content
- **Virtual Display**: Leverages Xvfb for headless rendering
- **Virtual Audio**: Creates virtual audio devices for streaming
- **Log Rotation**: Manages log files to prevent disk space issues
- **Monitoring**: Monitors stream health and restarts if needed
- **Scheduled Operation**: Streams only during specified time periods

## Schedule

The system is configured to stream at the following times:
- **Start**: Friday, Saturday, Sunday at 20:30 (Paris time)
- **Stop**: Saturday, Sunday, Monday at 03:30 (Paris time)

## Directory Structure

- **scripts/**: Contains all operational scripts
  - `stream.js`: Main streaming script using Puppeteer and ffmpeg
  - `start_stream.sh`: Script to initialize and start the stream
  - `stop_stream.sh`: Script to properly stop all related processes
  - `check_stream_time.sh`: Checks if current time is in streaming schedule
  - Various utility scripts

- **logs/**: Contains log files (rotated automatically)

## Usage

### Manual Operation

```bash
# Start the stream manually
cd /home/ubuntu/stream
bash scripts/start_stream.sh

# Check stream status
bash scripts/status_stream.sh

# Stop the stream
bash scripts/stop_stream.sh
```

### Automatic Operation

The system uses crontab for scheduled operation. View current schedule with:

```bash
crontab -l
```

## Dependencies

- Node.js
- Puppeteer
- ffmpeg
- Xvfb
- PulseAudio

## Code Standards

This project follows the [Squid Norm](SquidNorm.en.md) coding standards for consistent and maintainable code.

## Configuration

Stream configuration is stored in `.env` file with the YouTube stream key.

## Author

Cezou
