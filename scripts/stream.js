const puppeteer = require('puppeteer');
const { spawn, execSync } = require('child_process');
const fs = require('fs');
const dotenv = require('dotenv');
const path = require('path');

// Load environment variables from parent directory .env file
dotenv.config({ path: path.join(__dirname, '..', '.env') });

const STREAM_KEY = process.env.STREAM_KEY;
const PLATFORM = process.env.PLATFORM || 'rtmp://a.rtmp.youtube.com/live2';
// Ajouter la variable pour l'URL du site à streamer avec une valeur par défaut
const STREAM_URL = process.env.STREAM_URL || 'https://roulette-tv.vercel.app/history';

if (!STREAM_KEY) {
  console.error("ERROR: Streaming key not defined in .env file!");
  process.exit(1);
}

const WIDTH = 1920;
const HEIGHT = 1080;
const MAX_RETRY_COUNT = 3;
const RETRY_DELAY_MS = 30000; // 30 seconds

/**
 * @brief Logs a message with ISO timestamp
 * 
 * This function adds an ISO timestamp before the message and outputs it
 * to the console.
 * 
 * @param {string} message - The message to log
 */
function logWithTimestamp(message) {
  const timestamp = new Date().toISOString();
  console.log(`[${timestamp}] ${message}`);
}

/**
 * @brief Starts the ffmpeg process for streaming
 * 
 * This function launches ffmpeg with appropriate parameters for capturing
 * the screen and audio, then streaming to YouTube. It also sets up event
 * handlers for process output and handles automatic restart on failure.
 * 
 * @return {ChildProcess} The ffmpeg child process
 */
function startFFmpeg() {
  logWithTimestamp("Starting ffmpeg...");
  
  if (!STREAM_KEY) {
    logWithTimestamp("ERROR: STREAM_KEY not defined. Check .env file in parent directory.");
    process.exit(1);
  }
  
  const ffmpeg = spawn('ffmpeg', [
    '-f', 'pulse', '-i', 'virt_output.monitor',
    '-thread_queue_size', '1024',
    '-f', 'x11grab', 
    '-probesize', '10M',
    '-s', `${WIDTH}x${HEIGHT}`, 
    '-r', '24', 
    '-i', ':99.0',
    '-c:v', 'libx264',
    '-preset', 'veryfast',
    '-tune', 'zerolatency',
    '-b:v', '4000k',
    '-maxrate', '4000k',
    '-bufsize', '7000k',
    '-g', '24',
    '-crf', '23',
    '-fflags', 'nobuffer',
    '-flush_packets', '1',
    '-c:a', 'aac',
    '-b:a', '192k',
    '-ar', '48000',
    '-ac', '2',
    '-reconnect', '1',
    '-reconnect_streamed', '1',
    '-reconnect_delay_max', '120',
    '-f', 'flv',
    `-xerror`,
    `${PLATFORM}/${STREAM_KEY}`
  ]);

  ffmpeg.stdout.on('data', data => logWithTimestamp(`ffmpeg stdout: ${data}`));
  
  ffmpeg.stderr.on('data', data => {
    const dataStr = data.toString();
    if (!dataStr.match(/^frame=\s*\d+/)) {
      logWithTimestamp(`ffmpeg stderr: ${dataStr}`);
    }
  });

  ffmpeg.on('close', (code, signal) => {
    logWithTimestamp(`ffmpeg stopped with code: ${code}, signal: ${signal}`);
    
    if (code !== 0 && !process.exitCode) {
      logWithTimestamp("Attempting to restart ffmpeg in 30 seconds...");
      setTimeout(startFFmpeg, RETRY_DELAY_MS);
    }
  });

  return ffmpeg;
}

/**
 * @brief Main function that initializes and manages the streaming process
 * 
 * This function sets up the virtual display, launches the browser, navigates
 * to the streaming page, and starts ffmpeg for streaming. It also handles
 * cleanup and monitoring of the stream status.
 */
(async () => {
  let retryCount = 0;
  let browser;
  let ffmpegProcess;

  try {
    logWithTimestamp("Initializing environment...");
    execSync('bash ./create_screen.sh', { stdio: 'inherit' });

    logWithTimestamp("Waiting for environment stabilization...");
    await new Promise(resolve => setTimeout(resolve, 2000));

    /**
     * @brief Cleans up resources before shutdown
     * 
     * This function properly terminates all child processes and browser
     * instances when the application is shutting down.
     */
    const cleanup = () => {
      logWithTimestamp("Cleaning up resources...");
      if (ffmpegProcess) {
        try {
          ffmpegProcess.kill('SIGTERM');
        } catch (e) {
          logWithTimestamp(`Error stopping ffmpeg: ${e.message}`);
        }
      }
      
      if (browser) {
        try {
          browser.close();
        } catch (e) {
          logWithTimestamp(`Error closing browser: ${e.message}`);
        }
      }
    };

    process.on('SIGINT', () => {
      logWithTimestamp("SIGINT received, shutting down...");
      cleanup();
      process.exit(0);
    });
    
    process.on('SIGTERM', () => {
      logWithTimestamp("SIGTERM received, shutting down...");
      cleanup();
      process.exit(0);
    });

    logWithTimestamp("Launching browser...");
    browser = await puppeteer.launch({
      headless: false,
      executablePath: '/snap/bin/chromium',
      ignoreDefaultArgs: ['--enable-automation'],
      args: [
        '--no-sandbox',
        '--disable-setuid-sandbox',
        `--window-size=${WIDTH},${HEIGHT}`,
        '--start-fullscreen',
        '--autoplay-policy=no-user-gesture-required',
        '--disable-infobars'
      ]
    });

    const page = await browser.newPage();
    await page.setViewport({ width: WIDTH, height: HEIGHT });

    await page.evaluateOnNewDocument(() => {
      Object.defineProperty(navigator, 'webdriver', { get: () => false });
    });

    logWithTimestamp("Navigating to page...");
    // Utiliser l'URL configurée au lieu de l'URL codée en dur
    await page.goto(STREAM_URL, { waitUntil: 'networkidle2' });
    logWithTimestamp(`Page loaded: ${STREAM_URL}`);

    await page.evaluate(() => {
      document.body.style.zoom = '1.3';
    });
    await page.mouse.click(WIDTH, HEIGHT);
    await page.mouse.move(10, 10);

    ffmpegProcess = startFFmpeg();

    logWithTimestamp('Stream started...');
    
    const pingInterval = setInterval(() => {
      try {
        if (!browser || !browser.isConnected()) {
          logWithTimestamp("Browser disconnected, attempting restart...");
          clearInterval(pingInterval);
          throw new Error("Browser disconnected");
        }
        
        logWithTimestamp(`Stream running for ${process.uptime().toFixed(2)} seconds`);
        
        try {
          const ffmpegRunning = execSync('pgrep -f "ffmpeg.*rtmp"').toString().trim() !== '';
          if (!ffmpegRunning) {
            logWithTimestamp("FFmpeg not running, restarting...");
            ffmpegProcess = startFFmpeg();
          }
        } catch (error) {
          logWithTimestamp("FFmpeg not running, restarting...");
          ffmpegProcess = startFFmpeg();
        }
        
        execSync('bash ./rotate_logs.sh', { stdio: 'ignore' });
      } catch (err) {
        logWithTimestamp(`Error during ping: ${err.message}`);
        if (++retryCount > MAX_RETRY_COUNT) {
          logWithTimestamp(`Maximum retry count (${MAX_RETRY_COUNT}) reached, shutting down.`);
          clearInterval(pingInterval);
          process.exit(1);
        }
      }
    }, 30000);
  } catch (err) {
    logWithTimestamp(`Error during startup: ${err.message}`);
    logWithTimestamp(err.stack);
    process.exit(1);
  }
})();
