# Smart ICU RTOS

Smart ICU Patient Monitoring System using ESP32, FreeRTOS, Wokwi, and a Flask dashboard.

The project simulates an ICU monitor that reads heart rate, body temperature, and SpO2 from virtual potentiometers. A high-priority alarm task drives a local LED and buzzer when vital signs are critical or when the emergency button interrupt is triggered. Patient data is also posted to a web dashboard through HTTP.

## Features

- ESP32 firmware with FreeRTOS tasks for BPM, temperature, SpO2, alarm handling, WiFi posting, and serial monitoring.
- Rate-monotonic-style priorities: the 50 ms alarm task has the highest priority.
- Queue, mutex, binary semaphore, ISR deferred processing, and stack high-water mark logging.
- Optional advanced RTOS demo for deadlock prevention, priority inversion, and protected counter access.
- Flask dashboard with `/data` and `/latest` JSON endpoints.

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

The dashboard exposes the latest data at `GET /latest`.

## Run The Dashboard

```powershell
cd dashboard
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -r requirements.txt
python app.py
```

Open `http://localhost:5000`.

## Configure ESP32 HTTP Endpoint

Edit `serverName` in `src/main.cpp`:

```cpp
const char *serverName = "http://YOUR_SERVER_IP:5000/data";
```

For Wokwi, use an address reachable from the simulator, such as an ngrok URL:

```cpp
const char *serverName = "https://YOUR-NGROK-URL.ngrok-free.app/data";
```

## Build Firmware

```powershell
C:\Users\ardik\.platformio\penv\Scripts\platformio.exe run
```

## Advanced RTOS Demo

The concurrency demo is disabled by default so it does not disturb the main monitoring deadline.

To enable it, change this line in `src/main.cpp`:

```cpp
#define ENABLE_ADVANCED_RTOS_DEMO 1
```

Then rebuild and run the firmware.
