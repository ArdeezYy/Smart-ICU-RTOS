# Smart ICU RTOS

Smart ICU Patient Monitoring System using ESP32, FreeRTOS, Wokwi, Flask, and a React dashboard.

The project simulates an ICU monitor that reads heart rate, body temperature, and SpO2 from virtual potentiometers. A high-priority alarm task drives a local LED and buzzer when vital signs are critical or when the emergency button interrupt is triggered. Patient data is also posted to a web dashboard through HTTP.

## Features

- ESP32 firmware with FreeRTOS tasks for BPM, temperature, SpO2, alarm handling, WiFi posting, and serial monitoring.
- Rate-monotonic-style priorities: the 50 ms alarm task has the highest priority.
- Queue, mutex, binary semaphore, ISR deferred processing, and stack high-water mark logging.
- Optional advanced RTOS demo for deadlock prevention, priority inversion, and protected counter access.
- React dashboard served by Flask with `/data`, `/latest`, `/control`, and `/command` JSON endpoints.

## Pin Mapping

| Function | GPIO |
| --- | --- |
| BPM potentiometer | 34 |
| Temperature potentiometer | 35 |
| SpO2 potentiometer | 32 |
| Emergency button | 27 |
| Alarm LED | 25 |
| Buzzer | 26 |

## Timing

| Task | Period | Priority |
| --- | ---: | ---: |
| AlarmTask | 50 ms | 5 |
| HeartRateTask | 100 ms | 4 |
| OxygenTask | 100 ms | 4 |
| TemperatureTask | 150 ms | 3 |
| WiFiTask | 2000 ms | 2 |
| MonitoringTask | 500 ms | 1 |

## Dashboard API

ESP32 sends this payload to `POST /data`:

```json
{"bpm": 120, "temp": 37.5, "spo2": 96, "status": "NORMAL"}
```

The dashboard exposes the latest data and active command at `GET /latest`.

React sends ESP32 control commands to `POST /control`:

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

ESP32 polls the active command from `GET /command`.

Control modes:

- `web`: ESP32 uses BPM, temperature, and SpO2 values from the dashboard.
- `sensor`: ESP32 returns to Wokwi potentiometer input.

## Run The Dashboard

```powershell
cd dashboard/frontend
npm install
npm run build
cd ..
python app.py
```

Open `http://localhost:5000`.

For quick backend-only testing without the React build, Flask falls back to the simple HTML template.

## Configure ESP32 HTTP Endpoint

Edit `serverBaseUrl` in `src/main.cpp`:

```cpp
const char *serverBaseUrl = "http://YOUR_SERVER_IP:5000";
```

For Wokwi, use an address reachable from the simulator, such as an ngrok URL:

```cpp
const char *serverBaseUrl = "https://YOUR-NGROK-URL.ngrok-free.app";
```

## Build Firmware

```powershell
C:\Users\ardik\.platformio\penv\Scripts\platformio.exe run
```

## Development Notes

Run the React dev server only while editing the dashboard:

```powershell
cd dashboard
cd frontend
npm run dev
```

The dev server proxies API calls to Flask on port 5000.

## Advanced RTOS Demo

The concurrency demo is disabled by default so it does not disturb the main monitoring deadline.

To enable it, change this line in `src/main.cpp`:

```cpp
#define ENABLE_ADVANCED_RTOS_DEMO 1
```

Then rebuild and run the firmware.
