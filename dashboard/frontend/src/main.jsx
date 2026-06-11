import React, { useEffect, useMemo, useState } from "react";
import { createRoot } from "react-dom/client";
import {
  Activity,
  Bell,
  Gauge,
  HeartPulse,
  Power,
  RotateCcw,
  SlidersHorizontal,
  Thermometer,
  Waves
} from "lucide-react";
import "./styles.css";

const normalCommand = {
  mode: "web",
  bpm: 90,
  temp: 36.8,
  spo2: 97,
  emergency: false,
  alarm_override: "auto"
};

const criticalCommand = {
  mode: "web",
  bpm: 132,
  temp: 38.4,
  spo2: 88,
  emergency: false,
  alarm_override: "auto"
};

const sensorCommand = {
  ...normalCommand,
  mode: "sensor"
};

function formatTime(value) {
  if (!value) return "Waiting for data";
  return new Date(value).toLocaleString();
}

function statusFromValues(values) {
  if (values.alarm_override === "on" || values.emergency) return "CRITICAL";
  if (values.alarm_override === "off") return "NORMAL";
  return values.bpm > 130 || values.temp > 38 || values.spo2 < 90 ? "CRITICAL" : "NORMAL";
}

async function postJson(url, payload) {
  const response = await fetch(url, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload)
  });
  const data = await response.json();
  if (!response.ok) {
    throw new Error(data.message || "Request failed");
  }
  return data;
}

function App() {
  const [patient, setPatient] = useState({
    bpm: 0,
    temp: 0,
    spo2: 0,
    status: "NORMAL",
    source: "boot",
    updated_at: null
  });
  const [command, setCommand] = useState(sensorCommand);
  const [draft, setDraft] = useState(normalCommand);
  const [draftDirty, setDraftDirty] = useState(false);
  const [connection, setConnection] = useState("connecting");
  const [message, setMessage] = useState("Ready");

  const effectiveStatus = patient.status || statusFromValues(draft);

  async function refreshLatest() {
    try {
      const response = await fetch("/latest");
      const data = await response.json();
      setPatient(data.patient ?? data);
      if (data.command) {
        setCommand(data.command);
        if (!draftDirty) {
          setDraft({
            mode: data.command.mode,
            bpm: data.command.bpm,
            temp: data.command.temp,
            spo2: data.command.spo2,
            emergency: data.command.emergency,
            alarm_override: data.command.alarm_override
          });
        }
      }
      setConnection("online");
    } catch (error) {
      setConnection("offline");
      setMessage(error.message);
    }
  }

  async function sendCommand(nextCommand, successMessage) {
    try {
      const response = await postJson("/control", nextCommand);
      setCommand(response.command);
      setDraft({
        mode: response.command.mode,
        bpm: response.command.bpm,
        temp: response.command.temp,
        spo2: response.command.spo2,
        emergency: response.command.emergency,
        alarm_override: response.command.alarm_override
      });
      setDraftDirty(false);
      setMessage(successMessage);
      await refreshLatest();
    } catch (error) {
      setMessage(error.message);
    }
  }

  async function resetAlarm() {
    try {
      const response = await postJson("/control/reset", {});
      setCommand(response.command);
      setDraft({
        mode: response.command.mode,
        bpm: response.command.bpm,
        temp: response.command.temp,
        spo2: response.command.spo2,
        emergency: response.command.emergency,
        alarm_override: response.command.alarm_override
      });
      setDraftDirty(false);
      setMessage("Alarm reset to normal values");
      await refreshLatest();
    } catch (error) {
      setMessage(error.message);
    }
  }

  function updateDraft(field, value) {
    setDraftDirty(true);
    setDraft((current) => ({
      ...current,
      mode: "web",
      [field]: value
    }));
  }

  useEffect(() => {
    refreshLatest();
    const interval = window.setInterval(refreshLatest, 1000);
    return () => window.clearInterval(interval);
  }, [draftDirty]);

  const statusClass = effectiveStatus === "CRITICAL" ? "critical" : "normal";
  const connectionLabel = connection === "online" ? "Online" : connection === "offline" ? "Offline" : "Connecting";

  const vitals = useMemo(
    () => [
      { label: "BPM", value: patient.bpm, unit: "", icon: HeartPulse },
      { label: "Temperature", value: Number(patient.temp ?? 0).toFixed(1), unit: "C", icon: Thermometer },
      { label: "SpO2", value: patient.spo2, unit: "%", icon: Waves }
    ],
    [patient]
  );

  return (
    <main className="shell">
      <section className="header-band">
        <div>
          <p className="eyebrow">ESP32 + FreeRTOS</p>
          <h1>Smart ICU Monitor</h1>
        </div>
        <div className={`status-pill ${statusClass}`}>
          <Bell size={18} />
          <span>{effectiveStatus}</span>
        </div>
      </section>

      <section className="overview-grid">
        {vitals.map((item) => {
          const Icon = item.icon;
          return (
            <article className="vital-card" key={item.label}>
              <div className="vital-label">
                <Icon size={20} />
                <span>{item.label}</span>
              </div>
              <strong>
                {item.value}
                {item.unit && <small>{item.unit}</small>}
              </strong>
            </article>
          );
        })}
      </section>

      <section className="workspace">
        <div className="control-panel">
          <div className="panel-title">
            <SlidersHorizontal size={20} />
            <h2>Control Panel</h2>
          </div>

          <RangeControl label="BPM" value={draft.bpm} min={40} max={180} step={1} onChange={(value) => updateDraft("bpm", value)} />
          <RangeControl label="Temperature" value={draft.temp} min={30} max={45} step={0.1} onChange={(value) => updateDraft("temp", value)} />
          <RangeControl label="SpO2" value={draft.spo2} min={70} max={100} step={1} onChange={(value) => updateDraft("spo2", value)} />

          <div className="button-grid">
            <button type="button" onClick={() => sendCommand(draft, "Custom values sent to ESP32")}>
              <Gauge size={18} />
              Apply Values
            </button>
            <button type="button" onClick={() => sendCommand(normalCommand, "Normal scenario sent")}>
              <Activity size={18} />
              Set Normal
            </button>
            <button type="button" className="warning" onClick={() => sendCommand(criticalCommand, "Critical scenario sent")}>
              <Bell size={18} />
              Set Critical
            </button>
            <button
              type="button"
              className="danger"
              onClick={() => sendCommand({ ...draft, mode: "web", emergency: true, alarm_override: "on" }, "Emergency command sent")}
            >
              <Power size={18} />
              Emergency
            </button>
            <button type="button" onClick={resetAlarm}>
              <RotateCcw size={18} />
              Reset Alarm
            </button>
            <button type="button" onClick={() => sendCommand(sensorCommand, "Sensor mode enabled")}>
              <SlidersHorizontal size={18} />
              Sensor Mode
            </button>
          </div>
        </div>

        <aside className="telemetry-panel">
          <div className="panel-title">
            <Activity size={20} />
            <h2>Telemetry</h2>
          </div>
          <dl>
            <div>
              <dt>Connection</dt>
              <dd className={connection}>{connectionLabel}</dd>
            </div>
            <div>
              <dt>Mode</dt>
              <dd>{command.mode === "web" ? "Web Control" : "Sensor Mode"}</dd>
            </div>
            <div>
              <dt>Emergency</dt>
              <dd>{command.emergency ? "Active" : "Inactive"}</dd>
            </div>
            <div>
              <dt>Alarm</dt>
              <dd>{command.alarm_override}</dd>
            </div>
            <div>
              <dt>Source</dt>
              <dd>{patient.source || "unknown"}</dd>
            </div>
            <div>
              <dt>Last update</dt>
              <dd>{formatTime(patient.updated_at)}</dd>
            </div>
          </dl>
          <p className="message">{message}</p>
        </aside>
      </section>
    </main>
  );
}

function RangeControl({ label, value, min, max, step, onChange }) {
  const numericValue = Number(value);

  return (
    <label className="range-control">
      <span>
        {label}
        <strong>{step < 1 ? numericValue.toFixed(1) : numericValue}</strong>
      </span>
      <input
        type="range"
        min={min}
        max={max}
        step={step}
        value={numericValue}
        onChange={(event) => onChange(Number(event.target.value))}
      />
    </label>
  );
}

createRoot(document.getElementById("root")).render(<App />);
