# Smart ICU RTOS

Smart ICU Patient Monitoring System using **ESP32**, **FreeRTOS**, **Wokwi Simulator**, **Flask**, and **React Dashboard**.

The project simulates an ICU monitor that reads heart rate (BPM), body temperature, and SpO₂ from virtual potentiometers. A high-priority alarm task drives a local LED and buzzer when vital signs are critical or when the emergency button interrupt is triggered. Patient data is posted to a web dashboard through HTTP, and the dashboard can remotely control the ESP32.

---

## Architecture

```
┌─────────────────────┐     HTTP POST /data      ┌──────────────────────┐
│   ESP32 (Wokwi)     │ ─────────────────────────▶│   Flask Dashboard    │
│                     │                            │   (Python)           │
│  7 FreeRTOS Tasks:  │ ◀── GET /command (polling) │                      │
│  • HeartRateTask    │                            │  REST API:           │
│  • TemperatureTask  │                            │  /data     (POST)    │
│  • OxygenTask       │                            │  /latest   (GET)     │
│  • AlarmTask        │                            │  /control  (POST)    │
│  • WiFiTask         │                            │  /command  (GET)     │
│  • CommandTask      │                            │  /control/reset      │
│  • MonitoringTask   │                            └──────────┬───────────┘
│                     │                                       │
│  GPIO:              │                            ┌──────────▼───────────┐
│  34 → BPM pot       │                            │   React Frontend     │
│  35 → Temp pot      │                            │   (Vite + Lucide)    │
│  32 → SpO₂ pot      │                            │                      │
│  27 → Emerg. btn    │                            │  Live vitals display │
│  25 → LED (alarm)   │                            │  Slider controls     │
│  26 → Buzzer (PWM)  │                            │  Emergency button    │
└─────────────────────┘                            └──────────────────────┘
```

---

## Features

- **7 FreeRTOS tasks** with rate-monotonic priority assignment
- **Queue, Mutex, Binary Semaphore, ISR** — all RTOS primitives demonstrated
- **Deferred interrupt processing** via binary semaphore for emergency button
- **Stack high-water mark monitoring** for every task
- **Advanced RTOS demo** (opt-in): deadlock prevention, priority inversion demo, protected counter with spinlock
- **React dashboard** with live vitals, control sliders, and telemetry panel
- **Bidirectional control** — dashboard can override ESP32 sensor values
- **Wokwi simulation** with full wiring (breadboard, potentiometers, LED, buzzer, button)

---

## Pin Mapping

| Function | GPIO |
| --- | --- |
| BPM potentiometer | 34 |
| Temperature potentiometer | 35 |
| SpO₂ potentiometer | 32 |
| Emergency button | 27 (INPUT_PULLUP, FALLING edge) |
| Alarm LED | 25 |
| Buzzer (PWM) | 26 |

---

## Task Timing (Rate-Monotonic)

| Task | Period | Priority | Stack |
| --- | ---: | ---: | ---: |
| **AlarmTask** | 50 ms | 5 (highest) | 4096 |
| HeartRateTask | 100 ms | 4 | 2048 |
| OxygenTask | 100 ms | 4 | 2048 |
| TemperatureTask | 150 ms | 3 | 2048 |
| **WiFiTask** | 2000 ms | 2 | 8192 |
| **CommandTask** | 750 ms | 2 | 6144 |
| MonitoringTask | 500 ms | 1 (lowest) | 3072 |

---

## Dashboard API

### `POST /data` — ESP32 sends patient data

```json
{"bpm": 120, "temp": 37.5, "spo2": 96, "status": "NORMAL", "source": "esp32"}
```

### `GET /latest` — Get latest patient data + active command

Returns combined patient snapshot and the current ESP32 command.

### `POST /control` — Dashboard sends control command

```json
{
  "mode": "web",
  "bpm": 90,
  "temp": 36.8,
  "spo2": 97,
  "emergency": false,
  "alarm_override": "auto"
}
```

### `GET /command` — ESP32 polls for active command

Returns the latest command set from the dashboard.

### `POST /control/reset` — Reset to normal values

### Control Modes

| Mode | Behavior |
| --- | --- |
| `sensor` | ESP32 reads from Wokwi potentiometers |
| `web` | ESP32 uses values from the dashboard |

### Critical Thresholds

| Parameter | Critical If |
| --- | --- |
| BPM | > 130 |
| Temperature | > 38.0°C |
| SpO₂ | < 90% |

---

## Quick Start

### Prerequisites

- **Python 3.8+** with pip
- **Node.js 18+** with npm
- **PlatformIO** (for ESP32 firmware)
- **Wokwi for VSCode** extension
- **ngrok** (optional, for public access)

### 1. Clone & Install

```bash
git clone https://github.com/ArdeezYy/Smart-ICU-RTOS.git
cd Smart-ICU-RTOS
```

### 2. Build the Dashboard Frontend

```bash
cd dashboard/frontend
npm install
npm run build
cd ..
```

### 3. Install Python Dependencies

```bash
pip install -r requirements.txt
```

### 4. Run the Flask Dashboard

```bash
python app.py
```

Open `http://localhost:5000` — the React dashboard should appear.

### 5. Configure ESP32 Endpoint

Edit `src/main.cpp` and set `serverBaseUrl` to your Flask server address:

```cpp
// Option A: Local IP (for Wokwi on same machine)
const char *serverBaseUrl = "http://192.168.1.12:5000";

// Option B: ngrok public URL (for remote access)
// const char *serverBaseUrl = "https://your-tunnel.ngrok-free.dev";
```

**⚠️ Recommended**: Use **Option A (local IP)** for development. It avoids SSL handshake issues between the ESP32 simulator and ngrok's HTTPS endpoint. You can still expose the dashboard publicly with ngrok for external browsers — they handle HTTPS just fine.

### 6. Find Your Local IP

**Windows:**
```powershell
ipconfig | findstr "IPv4"
```

**macOS/Linux:**
```bash
ifconfig | grep "inet " | grep -v 127.0.0.1
```

### 7. Build & Run Firmware

```bash
# Build the firmware
platformio run

# Or with full path on Windows:
C:\Users\<user>\.platformio\penv\Scripts\platformio.exe run
```

### 8. Start Wokwi Simulation

In VSCode with the **Wokwi for VSCode** extension:
1. Open the project folder
2. Press `F1` → `Wokwi: Start Simulator`

The simulation reads `diagram.json` (wiring) and `wokwi.toml` (firmware path) automatically.

### 9. Verify It Works

Check the Wokwi **Serial Monitor** — you should see:

```
=======================
 SMART ICU MONITOR 
=======================
BPM    : 105
TEMP   : 37.2
SpO2   : 94
STATUS : NORMAL

[WiFiTask] HTTP response code: 200
```

And the dashboard at `http://localhost:5000` should update with live data.

---

## Public Access with ngrok (Optional)

If you want others to access your dashboard from the internet:

```bash
# 1. Install ngrok: https://ngrok.com/download
# 2. Add authtoken (free account required)
ngrok config add-authtoken YOUR_TOKEN

# 3. Start tunnel
ngrok http 5000

# 4. Copy the HTTPS URL (e.g. https://xxxx.ngrok-free.dev)
# 5. Share that URL — anyone can view the dashboard!
```

**Note**: External browsers can access the dashboard via ngrok without issues. The ESP32 firmware should still use the local IP — it doesn't need to go through ngrok.

---

## Advanced RTOS Demo

Disabled by default. Enable in `src/main.cpp`:

```cpp
#define ENABLE_ADVANCED_RTOS_DEMO 1
```

Then rebuild. This spawns additional tasks demonstrating:

| Concept | Implementation |
| --- | --- |
| **Deadlock Prevention** | Two tasks acquire mutexes in reverse order with timeout — one always backs off |
| **Priority Inversion** | Low-priority task holds mutex while high-priority task waits; medium task runs unbounded |
| **Protected Counter** | Spinlock (`portENTER_CRITICAL`) guards a shared counter incremented by two concurrent tasks |

---

## Development

### Frontend Dev Server

```bash
cd dashboard/frontend
npm run dev
```

The Vite dev server proxies `/latest`, `/data`, `/control`, `/command` to Flask on port 5000.

### Build Firmware (Full Clean)

```bash
platformio run --target clean
platformio run
```

---

## Project Structure

```
Smart-ICU-RTOS/
├── src/
│   └── main.cpp              # ESP32 FreeRTOS firmware (521 lines)
├── dashboard/
│   ├── app.py                # Flask REST API backend
│   ├── requirements.txt      # Python dependencies
│   ├── templates/
│   │   └── index.html        # Fallback HTML template
│   ├── static/
│   │   └── style.css         # Fallback CSS
│   └── frontend/
│       ├── index.html        # Vite entry point
│       ├── vite.config.js    # Vite config (API proxy)
│       ├── package.json      # npm dependencies
│       └── src/
│           ├── main.jsx      # React dashboard (305 lines)
│           └── styles.css    # Dashboard styling
├── diagram.json              # Wokwi circuit diagram
├── wokwi.toml                # Wokwi firmware config
├── platformio.ini            # PlatformIO build config
├── LAPORAN.md                # Full academic report (Bahasa Indonesia)
├── AUDIT.md                  # Code & architecture audit
└── README.md                 # This file
```

---

## Troubleshooting

| Problem | Solution |
| --- | --- |
| **HTTP response code: -1** | Check Flask is running (`python app.py`). Verify `serverBaseUrl` IP matches your machine. |
| **DNS Failed for ngrok URL** | Use local IP instead (`http://192.168.x.x:5000`). ngrok HTTPS can cause SSL issues with the ESP32 simulator. |
| **SSL errors (-80, -29312)** | Switch to HTTP and local IP. ESP32's SSL stack is inconsistent with ngrok's free-tier certificates. |
| **Dashboard shows stale data** | Make sure Wokwi simulation is running. Check serial monitor for `[WiFiTask] HTTP response code: 200`. |
| **Wokwi not loading new firmware** | Run `platformio run --target clean && platformio run`, then restart VSCode and Wokwi. |
| **Values not syncing** | Click "Sensor Mode" on the dashboard to let ESP32 read potentiometers. "Web Control" mode overrides with dashboard values. |

---

## License

This is a university project (RTOS course assignment). Free to use and modify.

---

## Credits

- **FreeRTOS** — Real-time operating system kernel
- **Wokwi** — Online ESP32 simulator
- **Flask** — Python web framework
- **React + Vite** — Frontend build toolchain
- **lucide-react** — Icon library
- **ArduinoJson** — JSON parsing for Arduino/ESP32
