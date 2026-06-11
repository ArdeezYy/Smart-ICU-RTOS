from datetime import datetime, timezone
from pathlib import Path
import threading

from flask import Flask, jsonify, render_template, request, send_from_directory

BASE_DIR = Path(__file__).resolve().parent
FRONTEND_DIST = BASE_DIR / "frontend" / "dist"

app = Flask(
    __name__,
    static_folder=str(FRONTEND_DIST / "assets") if FRONTEND_DIST.exists() else "static",
    static_url_path="/assets" if FRONTEND_DIST.exists() else "/static",
)

NORMAL_COMMAND = {
    "mode": "web",
    "bpm": 90,
    "temp": 36.8,
    "spo2": 97,
    "emergency": False,
    "alarm_override": "auto",
}

SENSOR_COMMAND = {
    "mode": "sensor",
    "bpm": 90,
    "temp": 36.8,
    "spo2": 97,
    "emergency": False,
    "alarm_override": "auto",
}

latest_data = {
    "bpm": 0,
    "temp": 0.0,
    "spo2": 0,
    "status": "NORMAL",
    "source": "boot",
    "updated_at": None,
}

latest_command = {
    **SENSOR_COMMAND,
    "updated_at": None,
}

lock = threading.Lock()


def utc_now():
    return datetime.now(timezone.utc).isoformat()


def calculate_status(bpm, temp, spo2, emergency=False, alarm_override="auto"):
    if alarm_override == "on" or emergency:
        return "CRITICAL"
    return "CRITICAL" if bpm > 130 or temp > 38.0 or spo2 < 90 else "NORMAL"


def parse_bool(value):
    if isinstance(value, bool):
        return value
    if isinstance(value, str):
        return value.strip().lower() in {"1", "true", "yes", "on"}
    return bool(value)


def sanitize_patient_payload(payload):
    required_fields = {"bpm", "temp", "spo2", "status"}
    missing_fields = sorted(required_fields - payload.keys())
    if missing_fields:
        return None, {"message": "missing required fields", "fields": missing_fields}

    try:
        data = {
            "bpm": int(payload["bpm"]),
            "temp": float(payload["temp"]),
            "spo2": int(payload["spo2"]),
            "status": str(payload["status"]).upper(),
            "source": str(payload.get("source", "esp32")),
            "updated_at": utc_now(),
        }
    except (TypeError, ValueError):
        return None, {"message": "invalid field types"}

    return data, None


def sanitize_command_payload(payload):
    try:
        mode = str(payload.get("mode", "web")).lower()
        alarm_override = str(payload.get("alarm_override", "auto")).lower()
        command = {
            "mode": mode,
            "bpm": int(payload.get("bpm", NORMAL_COMMAND["bpm"])),
            "temp": float(payload.get("temp", NORMAL_COMMAND["temp"])),
            "spo2": int(payload.get("spo2", NORMAL_COMMAND["spo2"])),
            "emergency": parse_bool(payload.get("emergency", False)),
            "alarm_override": alarm_override,
            "updated_at": utc_now(),
        }
    except (TypeError, ValueError):
        return None, {"message": "invalid command field types"}

    if command["mode"] not in {"web", "sensor"}:
        return None, {"message": "mode must be web or sensor"}
    if command["alarm_override"] not in {"auto", "on", "off"}:
        return None, {"message": "alarm_override must be auto, on, or off"}
    if not 40 <= command["bpm"] <= 180:
        return None, {"message": "bpm must be between 40 and 180"}
    if not 30.0 <= command["temp"] <= 45.0:
        return None, {"message": "temp must be between 30 and 45"}
    if not 70 <= command["spo2"] <= 100:
        return None, {"message": "spo2 must be between 70 and 100"}

    return command, None


def patient_data_from_command(command):
    status = calculate_status(
        command["bpm"],
        command["temp"],
        command["spo2"],
        command["emergency"],
        command["alarm_override"],
    )
    return {
        "bpm": command["bpm"],
        "temp": command["temp"],
        "spo2": command["spo2"],
        "status": status,
        "source": "web-control",
        "updated_at": command["updated_at"],
    }


@app.route("/")
def index():
    if (FRONTEND_DIST / "index.html").exists():
        return send_from_directory(FRONTEND_DIST, "index.html")

    return render_template("index.html")


@app.route("/<path:path>")
def frontend_assets(path):
    asset_path = FRONTEND_DIST / path
    if asset_path.exists() and asset_path.is_file():
        return send_from_directory(FRONTEND_DIST, path)
    if (FRONTEND_DIST / "index.html").exists():
        return send_from_directory(FRONTEND_DIST, "index.html")

    return render_template("index.html")


@app.route("/data", methods=["POST"])
def data():
    global latest_data

    try:
        payload = request.get_json(force=True)
    except Exception as exc:
        app.logger.warning("Invalid JSON from %s: %s", request.remote_addr, exc)
        return jsonify({"status": "error", "message": "invalid json"}), 400

    if not isinstance(payload, dict):
        return jsonify({"status": "error", "message": "payload must be an object"}), 400

    sanitized, error = sanitize_patient_payload(payload)
    if error:
        return jsonify({"status": "error", **error}), 400

    with lock:
        latest_data = sanitized

    app.logger.info("Updated patient data from %s: %s", request.remote_addr, sanitized)
    return jsonify({"status": "ok", "received": sanitized}), 200


@app.route("/latest")
def latest():
    with lock:
        data_snapshot = latest_data.copy()
        command_snapshot = latest_command.copy()

    return jsonify({**data_snapshot, "patient": data_snapshot, "command": command_snapshot})


@app.route("/control", methods=["POST"])
def control():
    global latest_command, latest_data

    try:
        payload = request.get_json(force=True)
    except Exception as exc:
        app.logger.warning("Invalid control JSON from %s: %s", request.remote_addr, exc)
        return jsonify({"status": "error", "message": "invalid json"}), 400

    if not isinstance(payload, dict):
        return jsonify({"status": "error", "message": "payload must be an object"}), 400

    command, error = sanitize_command_payload(payload)
    if error:
        return jsonify({"status": "error", **error}), 400

    with lock:
        latest_command = command
        if command["mode"] == "web":
            latest_data = patient_data_from_command(command)

    return jsonify({"status": "ok", "command": command}), 200


@app.route("/command")
def command():
    with lock:
        command_snapshot = latest_command.copy()

    return jsonify(command_snapshot)


@app.route("/control/reset", methods=["POST"])
def reset_control():
    global latest_command, latest_data

    command = {**NORMAL_COMMAND, "updated_at": utc_now()}

    with lock:
        latest_command = command
        latest_data = patient_data_from_command(command)

    return jsonify({"status": "ok", "command": command}), 200


if __name__ == "__main__":
    print("Flask dashboard running on http://localhost:5000")
    app.run(debug=True, port=5000, host="0.0.0.0", use_reloader=False)
