# SCRIPT VIDEO DEMO — Smart ICU RTOS
## Durasi: 10–15 Menit

---

## PERSIAPAN SEBELUM REKAM

### Yang Harus Dibuka di Layar:
1. **VSCode** — project Smart-ICU-RTOS terbuka, tab `src/main.cpp`
2. **Wokwi Simulator** — sudah running (di VSCode)
3. **Serial Monitor** — visible (bisa di panel bawah VSCode)
4. **Browser** — dashboard terbuka di `http://localhost:5000`
5. **Terminal** — Flask running (`python app.py`) + ngrok (opsional)

### Timing:
| Segmen | Durasi | Moderator |
|--------|--------|-----------|
| Intro & arsitektur | 2 menit | Ketua kelompok |
| Demo 1: Sensor & alarm | 2 menit | Anggota 1 |
| Demo 2: Dashboard & kontrol | 2 menit | Anggota 2 |
| Demo 3: Race condition | 2.5 menit | Anggota 3 |
| Demo 4: Priority inversion | 2.5 menit | Anggota 3 |
| Demo 5: Deadlock prevention | 1.5 menit | Anggota 4 |
| Safety-critical & closing | 1.5 menit | Ketua kelompok |

**Total: ~14 menit** ✅

---

## 🎬 SEGMEN 1: INTRO & ARSITEKTUR (2 menit)
*Presenter: Ketua Kelompok*

### Visual: VSCode → project structure

**Naskah:**
> "Halo, kami dari kelompok [X] akan mendemonstrasikan **Smart ICU Patient Monitoring System** berbasis ESP32 dan FreeRTOS.
>
> Sistem ini memantau 3 parameter vital pasien ICU secara real-time: **BPM (heart rate)**, **suhu tubuh**, dan **SpO2 (saturasi oksigen)**. Menggunakan FreeRTOS, kami menjalankan 7 task dengan prioritas berbeda:
> - AlarmTask prioritas 5 — setiap 50ms
> - HeartRateTask & OxygenTask prioritas 4 — setiap 100ms
> - TemperatureTask prioritas 3 — setiap 150ms
> - WiFiTask & CommandTask prioritas 2 — setiap 750-2000ms
> - MonitoringTask prioritas 1 — setiap 500ms
>
> ArduinoJson 7.4.3 digunakan untuk parsing JSON, dan dashboard React + Flask untuk visualisasi."

### Screenshot / Close-up:
- Tunjukkan `src/main.cpp` di VSCode — 7 task creation di `setup()`

---

## 🎬 SEGMEN 2: DEMO SENSOR & ALARM (2 menit)
*Presenter: Nicholai Dandy*

### Visual: Wokwi simulator + Serial Monitor

**Naskah:**
> "Saya akan mendemonstrasikan pembacaan sensor dan alarm. Di Wokwi, ada 3 potensiometer yang mewakili BPM, suhu, dan SpO2.
>
> [Putar potensiometer BPM ke kanan]
> Lihat di serial monitor: BPM berubah dari 60 menjadi 120.
>
> [Putar potensiometer SpO2 ke kiri hingga <90%]
> Status berubah menjadi **CRITICAL** — alarm LED merah menyala dan buzzer berbunyi. Ini terjadi dalam waktu kurang dari 50ms karena AlarmTask punya prioritas tertinggi.
>
> [Tekan tombol emergency hijau]
> Alarm menyala 3 detik — ini diproses melalui ISR + deferred processing."

### Tunjukkan:
- Serial Monitor: BPM, TEMP, SpO2, STATUS berubah
- Wokwi: LED merah nyala, buzzer bunyi
- Tekan emergency button

---

## 🎬 SEGMEN 3: DASHBOARD & KONTROL (2 menit)
*Presenter: Muhammad Deva Valery*

### Visual: Browser → dashboard

**Naskah:**
> "Sekarang kita lihat dashboard web di browser. Data dari ESP32 dikirim via HTTP POST setiap 2 detik.
>
> [Bukti `http://localhost:5000`]
> Di sini kita lihat BPM, Temperature, SpO2, dan status. Nilai-nilai ini sinkron dengan Wokwi.
>
> Sekarang saya coba ubah nilai lewat dashboard. Saya geser BPM ke 140, SpO2 ke 85 — klik 'Apply Values'.
> [Tunggu command task polling]
> Lihat, ESP32 sekarang pakai nilai dari dashboard — mode **Web Control**. Dan status jadi CRITICAL.
>
> Kalau klik 'Sensor Mode', ESP32 kembali baca potensiometer."

### Tunjukkan:
- Dashboard live
- Slider berubah nilai
- "Set Critical" / "Set Normal"
- "Sensor Mode" — nilai kembali ikut potensiometer
- "Emergency" button

---

## 🎬 SEGMEN 4: DEMO RACE CONDITION (2.5 menit)
*Presenter: Ardika Putra Hadian*

### Visual: Serial Monitor → Advanced RTOS Demo logs

> **Catatan**: Jalankan firmware dengan `#define ENABLE_ADVANCED_RTOS_DEMO 1`

**Naskah:**
> "Saya akan mendemonstrasikan **race condition** — masalah klasik di sistem multitasking.
>
> [Arahkan ke serial monitor]
>
> **SEBELUM proteksi**: Dua task `UnprotCounterA` dan `UnprotCounterB` berbagi counter tanpa spinlock.
>
> Lihat urutan angkanya: 1, 2, 3, 3, 4, 5, 5... Angka 3 muncul dua kali! Dan angka 6 hilang.
>
> Ini karena task A membaca counter=3, lalu task B juga membaca counter=3 sebelum A sempat menulis. Keduanya menulis 4 — satu increment hilang! Ini adalah **race condition**.
>
> **SESUDAH proteksi**: Sekarang dengan spinlock `portENTER_CRITICAL`.
>
> Angka berurutan sempurna: 1, 2, 3, 4, 5, 6, 7, 8... Tidak ada lost update. Spinlock memastikan operasi read-modify-write atomik.
>
> Jadi **mutex/spinlock sangat penting** untuk shared data di RTOS."

### Tunjukkan:
- Log `[RACE-UNPROTECTED]` — nilai duplikat atau loncat
- Log `[RACE-PROTECTED]` — urut sempurna
- Jelaskan mekanisme read-modify-write

---

## 🎬 SEGMEN 5: DEMO PRIORITY INVERSION (2.5 menit)
*Presenter: Ardika Putra Hadian*

**Naskah:**
> "Sekarang demo **priority inversion**. Ini terjadi ketika task prioritas tinggi terblokir oleh task prioritas rendah yang memegang resource bersama.
>
> **SEBELUM priority inheritance**: Kita pakai binary semaphore.
> - Low (prioritas 1) pegang semaphore
> - Med (prioritas 3) jalan terus
> - High (prioritas 5) menunggu semaphore
>
> [Lihat log] High butuh **3412ms** — padahal Low cuma pegang 3000ms. Kelebihan 412ms karena Low di-preempt oleh Med!
>
> **SESUDAH priority inheritance**: Kita pakai FreeRTOS mutex.
> - Low pegang mutex
> - High mau ambil mutex → FreeRTOS **naikkan prioritas Low ke 5**
> - Med (prioritas 3) tidak bisa preempt Low lagi
> - High dapat mutex dalam **3024ms** — hanya 24ms overhead! **95% lebih cepat!**
>
> Inilah kenapa kita pakai `xSemaphoreCreateMutex()` (bukan binary semaphore) untuk shared data — karena mutex mendukung **priority inheritance**."

### Tunjukkan:
- Log `[NO-PIP]` — timing ~3400ms
- Log `[WITH-PIP]` — timing ~3000ms
- Perbandingan angka

---

## 🎬 SEGMEN 6: DEMO DEADLOCK PREVENTION (1.5 menit)
*Presenter: [Anggota 4]*

**Naskah:**
> "Demo terakhir: **deadlock prevention**. Dua task mengambil resource dengan urutan berlawanan — A ambil R1 lalu R2; B ambil R2 lalu R1 — ini deadlock klasik.
>
> Tapi kita cegah dengan **timeout**: `xSemaphoreTake(resource2, pdMS_TO_TICKS(1000))` — kalau 1 detik gagal, lepaskan resource pertama.
>
> [Lihat log]
> `[DEADLOCK-A] DEADLOCK PREVENTED (timeout)` — task A sadar akan deadlock dan mundur. Ini mekanisme **deadlock avoidance**."

### Tunjukkan:
- Log `DEADLOCK PREVENTED (timeout)` muncul bergantian

---

## 🎬 SEGMEN 7: SAFETY-CRITICAL & CLOSING (1.5 menit)
*Presenter: Ketua Kelompok*

**Naskah:**
> "Tema kontemporer yang kami pilih adalah **Safety-Critical Systems**. Sistem ICU ini menerapkan 10 prinsip safety-critical:
>
> 1. Alarm prioritas tertinggi (5) — deadline 50ms
> 2. ISR singkat + deferred processing — emergency button
> 3. Deadlock prevention — timeout berdasarkan
> 4. Priority inheritance — mutex FreeRTOS
> 5. Stack monitoring — `uxTaskGetStackHighWaterMark`
> 6. Race condition prevention — mutex & spinlock
> 7. Non-blocking WiFi — prioritas rendah
>
> **Kesimpulan**: Dengan FreeRTOS, kita mendapatkan:
> - ✅ Alarm respons 50ms — **dijamin** oleh scheduler
> - ✅ HTTP tidak blokir sensor — **task terpisah**
> - ✅ Sinkronisasi aman — **mutex, queue, semaphore**
> - ✅ CPU utilization 23.6% — **sisa banyak buat fitur**
>
> Terima kasih. Ada pertanyaan?"

---

## 📋 CHECKLIST SEBELUM REKAM

| **No** | **Item** | **Check** |
|--------|----------|-----------|
| 1 | Firmware dengan `ENABLE_ADVANCED_RTOS_DEMO = 1` (sudah di commit) | ☐ |
| 2 | Flask berjalan (`python app.py`) | ☐ |
| 3 | Dashboard terverifikasi (`http://localhost:5000` buka, data masuk) | ☐ |
| 4 | Wokwi running di VSCode | ☐ |
| 5 | Serial Monitor terbuka | ☐ |
| 6 | Browser dashboard terbuka | ☐ |
| 7 | Potensiometer siap diputar | ☐ |
| 8 | Script video siap di layar kedua (atau dihapal) | ☐ |
| 9 | Suara mikrofon teruji | ☐ |
| 10 | Screen recorder siap (OBS Studio / built-in Windows) | ☐ |

## 📤 SETELAH REKAM

1. Upload video ke Google Drive / YouTube (unlisted)
2. Update `LAPORAN.md` Lampiran D dengan link video
3. Commit & push ke GitHub

---

*Script selesai. Durasi total: ~14 menit*
