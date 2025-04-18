# PageStreamer

## Overview

PageStreamer is a tool that automates streaming web pages to YouTube, Twitch and other platforms. It sets up a headless browser environment that captures any web page and streams it to your streaming platform, designed for scheduling it.

## Quick Installation

```bash
bash -c "$(curl -fsSL https://raw.githubusercontent.com/cezou/PageStreamer/master/install.sh)"
```

## Example of usage

You can use PageStreamer to stream website pages like [RouletteTV History page](https://roulette-tv.vercel.app/history) to a YouTube channel such as [FortunaCasinoTV](https://www.youtube.com/@FortunaCasinoTV/live).

Since it's a livestream, you can put it on a __FiveM TV__ _(using /media)_.

<div align="center">
  <a href="https://www.youtube.com/watch?v=OoPI71_KAIg" target="_blank">
    <img src="https://img.youtube.com/vi/OoPI71_KAIg/maxresdefault.jpg" alt="RouletteTV Demonstration" width="600" style="border-radius:10px; box-shadow: 0 4px 8px rgba(0,0,0,0.3);">
	      <p align="center">
      ▶️ <a href="https://www.youtube.com/watch?v=OoPI71_KAIg" target="_blank"><b>Watch Demo on YouTube</b></a>
    </p>

  </a>
</div>


## Features

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
pagestreamer    start     # Start streaming the configured web page
                stop      # Stop the current stream
                status    # Check if streaming is active
                --config  # Configure stream settings (platform, key, URL)
                --schedule # Set up automatic streaming schedule
```

## System Requirements

- Linux-based system (Ubuntu/Debian recommended)
- Internet connection
- Account on a streaming platform with streaming enabled
- Stream key from your platform

## Code Standards

This project follows the [Squid Norm](https://cezou.github.io/SquidNorm/#/) coding standards.

## Author

[Cezou](https://github.com/cezou/)
 