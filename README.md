# PageStreamer

## Overview

PageStreamer is a tool that automates streaming web pages to YouTube and other platforms. It sets up a headless browser environment that captures any web page and streams it to your streaming platform on a scheduled basis.

## Features

- **One-command Installation**: Easy installation via a simple curl command
- **Interactive Configuration**: Guided setup for streaming platform and key
- **Simple Controls**: Start, stop, and check status with one command
- **Multiple Platforms**: Support for YouTube, Twitch, Facebook, Instagram, and more
- **Scheduled Streaming**: Set up automatic streaming at specific times
- **Headless Browser**: Uses Puppeteer to render web content without a visible browser
- **Log Management**: Automatically rotates logs to prevent disk space issues
- **Monitoring**: Keeps track of stream health and automatically restarts if needed

## Quick Installation

Install PageStreamer with a single command:

```bash
bash -c "$(curl -fsSL https://raw.githubusercontent.com/cezou/PageStreamer/master/install.sh)"
```

The installer will:
1. Download the project to `~/.pagestreamer`
2. Install all dependencies
3. Build the project
4. Add PageStreamer to your PATH
5. Guide you through platform and stream key configuration

## Usage

### Command Line Interface

PageStreamer offers a simple command-line interface:

```bash
# Start streaming
pagestreamer start

# Check stream status
pagestreamer status

# Stop streaming
pagestreamer stop

# Configure settings
pagestreamer --config
```

### Configuration

You can configure your stream settings anytime with:

```bash
# Configure all settings
pagestreamer --config

# Configure just the platform
pagestreamer --config PLATFORM

# Configure just the stream key
pagestreamer --config STREAM_KEY

# Configure the website URL to stream
pagestreamer --config STREAM_URL

# View current configuration (stream key will be masked)
pagestreamer --config see
```

### Default Schedule

The system is configured to stream:
- **Start**: Friday, Saturday, Sunday at 20:30 (Paris time)
- **Stop**: Saturday, Sunday, Monday at 03:30 (Paris time)

You can modify this schedule by editing the crontab.txt file and running `bash update_crontab.sh`.

## Example

You can use PageStreamer to stream sites like the [RouletteTV History page](https://roulette-tv.vercel.app/history) to a YouTube channel such as [FortunaCasinoTV](https://www.youtube.com/@FortunaCasinoTV/live).

## System Requirements

- Linux-based system (Ubuntu/Debian recommended)
- Internet connection
- Account on a streaming platform with streaming enabled
- Stream key from your platform

## Code Standards

This project follows the [Squid Norm](SquidNorm.en.md) coding standards.

## Author

Cezou
