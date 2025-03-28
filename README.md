# PageStreamer

## Overview

PageStreamer is a tool that automates streaming web pages to YouTube and other platforms. It sets up a headless browser environment that captures any web page and streams it to your streaming platform on a scheduled basis.

## Quick Installation

```bash
bash -c "$(curl -fsSL https://raw.githubusercontent.com/cezou/PageStreamer/master/install.sh)"
```


## Features

- **One-command Installation**: Easy setup and guided setup via a simple curl command
- **Simple Controls**: Start, stop, and check status with one command : pagestreamer
- **Multiple Platforms**: Support for YouTube, Twitch, Facebook, Instagram, TikTok and more
- **Scheduled Streaming**: Set up automatic streaming times using crontab

## How It Works

PageStreamer uses a headless Chromium browser powered by Puppeteer to render web pages without displaying them on screen. The process works as follows:

1. A virtual display (Xvfb) is created to run the browser in a headless environment
2. Puppeteer launches Chromium and navigates to your specified web page
3. FFmpeg captures the virtual display and audio output
4. The captured content is encoded and streamed to your selected platform

This approach is ideal for:
- Remote servers (AWS, OCI, etc.) without a physical display
- Windows Subsystem for Linux (WSL) environments
- Dedicated streaming machines running Linux


## Usage

### Command Line Interface

```bash
start     # Start streaming the configured web page
stop      # Stop the current stream
status    # Check if streaming is active
--config  # Configure stream settings (platform, key, URL)
--schedule # Set up automatic streaming schedule
```

## Example

You can use PageStreamer to stream sites like the [RouletteTV History page](https://roulette-tv.vercel.app/history) to a YouTube channel such as [FortunaCasinoTV](https://www.youtube.com/@FortunaCasinoTV/live).

## System Requirements

- Linux-based system (Ubuntu/Debian recommended)
- Internet connection
- Account on a streaming platform with streaming enabled
- Stream key from your platform

## Code Standards

This project follows the [Squid Norm](https://cezou.github.io/SquidNorm/#/) coding standards.

## Author

[Cezou](https://github.com/cezou/)
 