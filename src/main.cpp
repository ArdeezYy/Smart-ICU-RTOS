#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

// Set to 1 when you want to run the concurrency demonstration tasks.
#define ENABLE_ADVANCED_RTOS_DEMO 0

// =====================================================
// WIFI CONFIG
// =====================================================

const char *ssid = "Wokwi-GUEST";
const char *password = "";

// Change this to your local Flask server IP or an ngrok URL.
const char *serverBaseUrl = "http://trunks-evidence-gigantic.ngrok-free.dev";

// =====================================================
// PIN CONFIG
// =====================================================

#define BPM_PIN 34
#define TEMP_PIN 35
#define SPO2_PIN 32

#define EMERGENCY_BTN 27

#define LED_ALARM 25
#define BUZZER_PIN 26

// =====================================================
// TIMING CONFIG
// =====================================================

constexpr TickType_t ALARM_PERIOD_TICKS = pdMS_TO_TICKS(50);
constexpr TickType_t BPM_PERIOD_TICKS = pdMS_TO_TICKS(100);
constexpr TickType_t SPO2_PERIOD_TICKS = pdMS_TO_TICKS(100);
constexpr TickType_t TEMP_PERIOD_TICKS = pdMS_TO_TICKS(150);
constexpr TickType_t MONITOR_PERIOD_TICKS = pdMS_TO_TICKS(500);
constexpr TickType_t WIFI_PERIOD_TICKS = pdMS_TO_TICKS(2000);
constexpr TickType_t COMMAND_PERIOD_TICKS = pdMS_TO_TICKS(750);
constexpr TickType_t EMERGENCY_ALARM_TICKS = pdMS_TO_TICKS(3000);

// =====================================================
// DATA STRUCTURE
// =====================================================

typedef struct {
  int bpm;
  float temp;
  int spo2;
  char status[20];
} PatientData;

typedef struct {
  char mode[10];
  int bpm;
  float temp;
  int spo2;
  bool emergency;
  char alarmOverride[10];
} ControlCommand;

// =====================================================
// GLOBAL VARIABLES
// =====================================================

PatientData patient = {0, 0.0f, 0, "NORMAL"};
ControlCommand controlCommand = {"sensor", 90, 36.8f, 97, false, "auto"};

QueueHandle_t patientQueue;
SemaphoreHandle_t dataMutex;
SemaphoreHandle_t emergencySem;

TaskHandle_t heartRateTaskHandle;
TaskHandle_t temperatureTaskHandle;
TaskHandle_t oxygenTaskHandle;
TaskHandle_t alarmTaskHandle;
TaskHandle_t wifiTaskHandle;
TaskHandle_t commandTaskHandle;
TaskHandle_t monitoringTaskHandle;

#if ENABLE_ADVANCED_RTOS_DEMO
SemaphoreHandle_t resource1;
SemaphoreHandle_t resource2;
SemaphoreHandle_t priorityMutex;
SemaphoreHandle_t inversionSem;

volatile int sharedCounter = 0;
portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
#endif

// =====================================================
// HELPERS
// =====================================================

float mapFloat(int value, int inMin, int inMax, float outMin, float outMax) {
  return (static_cast<float>(value - inMin) * (outMax - outMin) /
          static_cast<float>(inMax - inMin)) +
         outMin;
}

void publishPatientSnapshot() {
  xQueueOverwrite(patientQueue, &patient);
}

void setAlarmOutput(bool active) {
  digitalWrite(LED_ALARM, active ? HIGH : LOW);
  ledcWriteTone(0, active ? 1000 : 0);
}

bool isWebControlMode() {
  return strcmp(controlCommand.mode, "web") == 0;
}

bool isCriticalValue(int bpm, float temp, int spo2) {
  return bpm > 130 || temp > 38.0f || spo2 < 90;
}

bool shouldAlarmBeActive(bool localEmergencyActive = false) {
  if (strcmp(controlCommand.alarmOverride, "on") == 0 || controlCommand.emergency) {
    return true;
  }

  return isCriticalValue(patient.bpm, patient.temp, patient.spo2) || localEmergencyActive;
}

void printStackWatermark(const char *taskName, TaskHandle_t handle) {
  Serial.print(taskName);
  Serial.print(" stack high-water mark: ");
  Serial.println(uxTaskGetStackHighWaterMark(handle));
}

// =====================================================
// ISR
// =====================================================

void IRAM_ATTR emergencyISR() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(emergencySem, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// =====================================================
// SENSOR TASKS
// =====================================================

void HeartRateTask(void *pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();

  while (1) {
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
      if (!isWebControlMode()) {
        patient.bpm = map(analogRead(BPM_PIN), 0, 4095, 60, 150);
      }
      publishPatientSnapshot();
      xSemaphoreGive(dataMutex);
    }

    vTaskDelayUntil(&lastWakeTime, BPM_PERIOD_TICKS);
  }
}

void TemperatureTask(void *pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();

  while (1) {
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
      if (!isWebControlMode()) {
        patient.temp = mapFloat(analogRead(TEMP_PIN), 0, 4095, 35.0f, 40.0f);
      }
      publishPatientSnapshot();
      xSemaphoreGive(dataMutex);
    }

    vTaskDelayUntil(&lastWakeTime, TEMP_PERIOD_TICKS);
  }
}

void OxygenTask(void *pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();

  while (1) {
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
      if (!isWebControlMode()) {
        patient.spo2 = map(analogRead(SPO2_PIN), 0, 4095, 85, 100);
      }
      publishPatientSnapshot();
      xSemaphoreGive(dataMutex);
    }

    vTaskDelayUntil(&lastWakeTime, SPO2_PERIOD_TICKS);
  }
}

// =====================================================
// ALARM TASK
// =====================================================

void AlarmTask(void *pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  TickType_t emergencyActiveUntil = 0;

  while (1) {
    while (xSemaphoreTake(emergencySem, 0) == pdTRUE) {
      emergencyActiveUntil = xTaskGetTickCount() + EMERGENCY_ALARM_TICKS;
      Serial.println("[ISR] Emergency button triggered");
    }

    TickType_t now = xTaskGetTickCount();
    bool alarmActive = false;

    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
      bool emergencyActive = now < emergencyActiveUntil;
      alarmActive = shouldAlarmBeActive(emergencyActive);

      strcpy(patient.status, alarmActive ? "CRITICAL" : "NORMAL");
      publishPatientSnapshot();

      xSemaphoreGive(dataMutex);
    }

    setAlarmOutput(alarmActive);
    vTaskDelayUntil(&lastWakeTime, ALARM_PERIOD_TICKS);
  }
}

// =====================================================
// WIFI TASK
// =====================================================

void WiFiTask(void *pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (1) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[WiFiTask] WiFi disconnected. Reconnecting...");
      WiFi.disconnect();
      WiFi.begin(ssid, password);
    } else {
      PatientData snapshot;

      if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
        snapshot = patient;
        xSemaphoreGive(dataMutex);
      }

      String jsonData = "{";
      jsonData += "\"bpm\":" + String(snapshot.bpm) + ",";
      jsonData += "\"temp\":" + String(snapshot.temp, 2) + ",";
      jsonData += "\"spo2\":" + String(snapshot.spo2) + ",";
      jsonData += "\"status\":\"" + String(snapshot.status) + "\",";
      jsonData += "\"source\":\"esp32\"";
      jsonData += "}";

      HTTPClient http;
      String dataUrl = String(serverBaseUrl) + "/data";
      http.begin(dataUrl);
      http.addHeader("Content-Type", "application/json");
      http.setTimeout(1000);

      Serial.println("[WiFiTask] Posting patient snapshot");
      Serial.println(jsonData);

      int httpCode = http.POST(jsonData);
      Serial.println("[WiFiTask] HTTP response code: " + String(httpCode));
      http.end();
    }

    vTaskDelayUntil(&lastWakeTime, WIFI_PERIOD_TICKS);
  }
}

// =====================================================
// COMMAND TASK
// =====================================================

void CommandTask(void *pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();

  while (1) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String commandUrl = String(serverBaseUrl) + "/command";
      http.begin(commandUrl);
      http.setTimeout(1000);

      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        String response = http.getString();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error && xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
          strlcpy(controlCommand.mode, doc["mode"] | "sensor", sizeof(controlCommand.mode));
          controlCommand.bpm = doc["bpm"] | 90;
          controlCommand.temp = doc["temp"] | 36.8f;
          controlCommand.spo2 = doc["spo2"] | 97;
          controlCommand.emergency = doc["emergency"] | false;
          strlcpy(controlCommand.alarmOverride, doc["alarm_override"] | "auto", sizeof(controlCommand.alarmOverride));

          if (isWebControlMode()) {
            patient.bpm = controlCommand.bpm;
            patient.temp = controlCommand.temp;
            patient.spo2 = controlCommand.spo2;
            strcpy(patient.status, shouldAlarmBeActive() ? "CRITICAL" : "NORMAL");
            publishPatientSnapshot();
          }

          Serial.print("[CommandTask] mode=");
          Serial.print(controlCommand.mode);
          Serial.print(" bpm=");
          Serial.print(controlCommand.bpm);
          Serial.print(" temp=");
          Serial.print(controlCommand.temp);
          Serial.print(" spo2=");
          Serial.print(controlCommand.spo2);
          Serial.print(" emergency=");
          Serial.println(controlCommand.emergency ? "true" : "false");

          xSemaphoreGive(dataMutex);
        } else if (error) {
          Serial.println("[CommandTask] Failed to parse command JSON");
        }
      } else {
        Serial.println("[CommandTask] Command endpoint unavailable");
      }

      http.end();
    }

    vTaskDelayUntil(&lastWakeTime, COMMAND_PERIOD_TICKS);
  }
}

// =====================================================
// MONITORING TASK
// =====================================================

void MonitoringTask(void *pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  PatientData receivedData;
  uint32_t stackLogCounter = 0;

  while (1) {
    if (xQueueReceive(patientQueue, &receivedData, 0) == pdTRUE) {
      Serial.println();
      Serial.println("=======================");
      Serial.println(" SMART ICU MONITOR ");
      Serial.println("=======================");
      Serial.print("BPM    : ");
      Serial.println(receivedData.bpm);
      Serial.print("TEMP   : ");
      Serial.println(receivedData.temp);
      Serial.print("SpO2   : ");
      Serial.println(receivedData.spo2);
      Serial.print("STATUS : ");
      Serial.println(receivedData.status);
    }

    if (++stackLogCounter >= 10) {
      stackLogCounter = 0;
      Serial.println("--- STACK HIGH-WATER MARK ---");
      printStackWatermark("HeartRateTask", heartRateTaskHandle);
      printStackWatermark("TemperatureTask", temperatureTaskHandle);
      printStackWatermark("OxygenTask", oxygenTaskHandle);
      printStackWatermark("AlarmTask", alarmTaskHandle);
      printStackWatermark("WiFiTask", wifiTaskHandle);
      printStackWatermark("CommandTask", commandTaskHandle);
      printStackWatermark("MonitoringTask", monitoringTaskHandle);
    }

    vTaskDelayUntil(&lastWakeTime, MONITOR_PERIOD_TICKS);
  }
}

#if ENABLE_ADVANCED_RTOS_DEMO
// =====================================================
// ADVANCED RTOS DEMO TASKS
// =====================================================

// =============================================
// RACE CONDITION DEMO — Sebelum Proteksi
// =============================================

void UnprotectedCounterA(void *pvParameters) {
  while (1) {
    // === TANPA proteksi: RACE CONDITION! ===
    int temp = sharedCounter;       // Baca
    vTaskDelay(pdMS_TO_TICKS(2));   // Simulasi delay (biar task lain sempat tulis)
    sharedCounter = temp + 1;       // Tulis balik — bisa timpa data task lain!

    int snapshot = sharedCounter;
    Serial.print("[RACE-UNPROTECTED] ");
    Serial.print(pcTaskGetName(NULL));
    Serial.print(" counter: ");
    Serial.println(snapshot);

    vTaskDelay(pdMS_TO_TICKS(15));
  }
}

// =============================================
// RACE CONDITION DEMO — Sesudah Proteksi
// =============================================

void ProtectedCounterTask(void *pvParameters) {
  while (1) {
    // === DENGAN proteksi spinlock: AMAN ===
    portENTER_CRITICAL(&spinlock);
    sharedCounter++;
    int snapshot = sharedCounter;
    portEXIT_CRITICAL(&spinlock);

    Serial.print("[RACE-PROTECTED] ");
    Serial.print(pcTaskGetName(NULL));
    Serial.print(" counter: ");
    Serial.println(snapshot);

    vTaskDelay(pdMS_TO_TICKS(15));
  }
}

// =============================================
// PRIORITY INVERSION DEMO — Sebelum PIP
// (Menggunakan Binary Semaphore tanpa priority inheritance)
// =============================================

void InversionLowNoPIP(void *pvParameters) {
  TickType_t start, end;
  while (1) {
    xSemaphoreTake(inversionSem, portMAX_DELAY);
    start = xTaskGetTickCount();

    Serial.println("[NO-PIP] LOW: Locked semaphore");
    vTaskDelay(pdMS_TO_TICKS(3000));  // Tahan 3 detik
    Serial.println("[NO-PIP] LOW: Released semaphore");
    xSemaphoreGive(inversionSem);

    end = xTaskGetTickCount();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void InversionMedNoPIP(void *pvParameters) {
  while (1) {
    Serial.println("[NO-PIP] MEDIUM: Running workload...");
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void InversionHighNoPIP(void *pvParameters) {
  TickType_t waitStart, waitEnd;
  while (1) {
    vTaskDelay(pdMS_TO_TICKS(500));
    waitStart = xTaskGetTickCount();

    if (xSemaphoreTake(inversionSem, pdMS_TO_TICKS(5000)) == pdTRUE) {
      waitEnd = xTaskGetTickCount();
      Serial.print("[NO-PIP] HIGH: Acquired semaphore after ");
      Serial.print((waitEnd - waitStart) * portTICK_PERIOD_MS);
      Serial.println(" ms (tanpa PIP → lambat!)");
      xSemaphoreGive(inversionSem);
    } else {
      Serial.println("[NO-PIP] HIGH: TIMEOUT! Inversion terlalu parah.");
    }
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}

// =============================================
// PRIORITY INVERSION DEMO — Sesudah PIP
// (Menggunakan Mutex dengan priority inheritance)
// =============================================
void InversionLowPIP(void *pvParameters) {
  while (1) {
    xSemaphoreTake(priorityMutex, portMAX_DELAY);
    Serial.println("[WITH-PIP] LOW: Locked mutex");
    vTaskDelay(pdMS_TO_TICKS(3000));
    Serial.println("[WITH-PIP] LOW: Released mutex");
    xSemaphoreGive(priorityMutex);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void InversionMedPIP(void *pvParameters) {
  while (1) {
    Serial.println("[WITH-PIP] MEDIUM: Running workload...");
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void InversionHighPIP(void *pvParameters) {
  TickType_t waitStart, waitEnd;
  while (1) {
    vTaskDelay(pdMS_TO_TICKS(500));
    waitStart = xTaskGetTickCount();

    if (xSemaphoreTake(priorityMutex, pdMS_TO_TICKS(5000)) == pdTRUE) {
      waitEnd = xTaskGetTickCount();
      Serial.print("[WITH-PIP] HIGH: Acquired mutex after ");
      Serial.print((waitEnd - waitStart) * portTICK_PERIOD_MS);
      Serial.println(" ms (dengan PIP → cepat!)");
      xSemaphoreGive(priorityMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}

// =============================================
// DEADLOCK PREVENTION DEMO (sudah ada)
// =============================================
void DeadlockTaskA(void *pvParameters) {
  while (1) {
    xSemaphoreTake(resource1, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(500));

    if (xSemaphoreTake(resource2, pdMS_TO_TICKS(1000)) == pdTRUE) {
      Serial.println("[DEADLOCK-A] Acquired both resources");
      xSemaphoreGive(resource2);
    } else {
      Serial.println("[DEADLOCK-A] DEADLOCK PREVENTED (timeout)");
    }
    xSemaphoreGive(resource1);
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void DeadlockTaskB(void *pvParameters) {
  while (1) {
    xSemaphoreTake(resource2, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(500));

    if (xSemaphoreTake(resource1, pdMS_TO_TICKS(1000)) == pdTRUE) {
      Serial.println("[DEADLOCK-B] Acquired both resources");
      xSemaphoreGive(resource1);
    } else {
      Serial.println("[DEADLOCK-B] DEADLOCK PREVENTED (timeout)");
    }
    xSemaphoreGive(resource2);
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
#endif


// =====================================================
// SETUP
// =====================================================

void setup() {
  Serial.begin(115200);

  pinMode(LED_ALARM, OUTPUT);
  pinMode(EMERGENCY_BTN, INPUT_PULLUP);

  ledcSetup(0, 1000, 8);
  ledcAttachPin(BUZZER_PIN, 0);
  setAlarmOutput(false);

  patientQueue = xQueueCreate(1, sizeof(PatientData));
  dataMutex = xSemaphoreCreateMutex();
  emergencySem = xSemaphoreCreateBinary();

  if (patientQueue == NULL || dataMutex == NULL || emergencySem == NULL) {
    Serial.println("[SETUP] Failed to create FreeRTOS objects");
    while (1) {
      delay(1000);
    }
  }

  attachInterrupt(digitalPinToInterrupt(EMERGENCY_BTN), emergencyISR, FALLING);

  xTaskCreate(HeartRateTask, "HeartRateTask", 2048, NULL, 4, &heartRateTaskHandle);
  xTaskCreate(TemperatureTask, "TemperatureTask", 2048, NULL, 3, &temperatureTaskHandle);
  xTaskCreate(OxygenTask, "OxygenTask", 2048, NULL, 4, &oxygenTaskHandle);
  xTaskCreate(AlarmTask, "AlarmTask", 4096, NULL, 5, &alarmTaskHandle);
  xTaskCreate(WiFiTask, "WiFiTask", 8192, NULL, 2, &wifiTaskHandle);
  xTaskCreate(CommandTask, "CommandTask", 6144, NULL, 2, &commandTaskHandle);
  xTaskCreate(MonitoringTask, "MonitoringTask", 3072, NULL, 1, &monitoringTaskHandle);

#if ENABLE_ADVANCED_RTOS_DEMO
  resource1 = xSemaphoreCreateMutex();
  resource2 = xSemaphoreCreateMutex();
  priorityMutex = xSemaphoreCreateMutex();
  inversionSem = xSemaphoreCreateBinary();

  Serial.println("\n========================================");
  Serial.println(" ADVANCED RTOS DEMO STARTED");
  Serial.println("========================================");
  Serial.println("=== PHASE 1: RACE CONDITION DEMO ===");
  Serial.println("UnprotectedCounterA/B (tanpa spinlock) → data corruption");
  Serial.println("ProtectedCounterA/B (dengan spinlock) → data aman");
  Serial.println("=== PHASE 2: PRIORITY INVERSION DEMO ===");
  Serial.println("No-PIP: InversionLow/Med/High (binary sem) → blocking lama");
  Serial.println("With-PIP: InversionLow/Med/High (mutex) → blocking cepat");

  // Race Condition Demo — UNPROTECTED
  xSemaphoreGive(inversionSem);
  xTaskCreate(UnprotectedCounterA, "UnprotCounterA", 2048, NULL, 4, NULL);
  xTaskCreate(UnprotectedCounterA, "UnprotCounterB", 2048, NULL, 4, NULL);

  // Race Condition Demo — PROTECTED
  xTaskCreate(ProtectedCounterTask, "ProtCounterA", 2048, NULL, 3, NULL);
  xTaskCreate(ProtectedCounterTask, "ProtCounterB", 2048, NULL, 3, NULL);

  // Priority Inversion Demo — TANPA PIP (binary semaphore)
  xTaskCreate(InversionLowNoPIP, "NoPIP_Low", 2048, NULL, 1, NULL);
  xTaskCreate(InversionMedNoPIP, "NoPIP_Med", 2048, NULL, 3, NULL);
  xTaskCreate(InversionHighNoPIP, "NoPIP_High", 2048, NULL, 5, NULL);

  // Priority Inversion Demo — DENGAN PIP (mutex)
  xTaskCreate(InversionLowPIP, "PIP_Low", 2048, NULL, 1, NULL);
  xTaskCreate(InversionMedPIP, "PIP_Med", 2048, NULL, 3, NULL);
  xTaskCreate(InversionHighPIP, "PIP_High", 2048, NULL, 5, NULL);

  // Deadlock Prevention Demo
  xTaskCreate(DeadlockTaskA, "DeadlockA", 4096, NULL, 2, NULL);
  xTaskCreate(DeadlockTaskB, "DeadlockB", 4096, NULL, 2, NULL);
#endif

  Serial.println("[SETUP] Smart ICU RTOS system started");
}

void loop() {}
