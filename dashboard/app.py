from datetime import datetime, timezone
import threading

from flask import Flask, jsonify, render_template, request

app = Flask(__name__)

latest_data = {
    "bpm": 0,
    "temp": 0.0,
    "spo2": 0,
    "status": "NORMAL",
    "updated_at": None,
}

lock = threading.Lock()


@app.route("/")
def index():
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

    required_fields = {"bpm", "temp", "spo2", "status"}
    missing_fields = sorted(required_fields - payload.keys())
    if missing_fields:
        return (
            jsonify(
                {
                    "status": "error",
                    "message": "missing required fields",
                    "fields": missing_fields,
                }
            ),
            400,
        )

    try:
        sanitized = {
            "bpm": int(payload["bpm"]),
            "temp": float(payload["temp"]),
            "spo2": int(payload["spo2"]),
            "status": str(payload["status"]).upper(),
            "updated_at": datetime.now(timezone.utc).isoformat(),
        }
    except (TypeError, ValueError):
        return jsonify({"status": "error", "message": "invalid field types"}), 400

    with lock:
        latest_data = sanitized

    app.logger.info("Updated patient data from %s: %s", request.remote_addr, sanitized)
    return jsonify({"status": "ok", "received": sanitized}), 200


@app.route("/latest")
def latest():
    with lock:
        data_snapshot = latest_data.copy()

    return jsonify(data_snapshot)


if __name__ == "__main__":
    print("Flask dashboard running on http://localhost:5000")
    app.run(debug=True, port=5000, host="0.0.0.0", use_reloader=False)
