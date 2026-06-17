# SCRIPT VIDEO DEMO — Smart ICU RTOS
## 7 Segmen × Narasi Lengkap

**Project**: Smart ICU Patient Monitoring System  
**Platform**: ESP32 + FreeRTOS + Wokwi + Flask + React  
**Durasi**: ~14 menit  
**GitHub**: https://github.com/ArdeezYy/Smart-ICU-RTOS

---

# 📋 CHECKLIST SEBELUM REKAM

| No | Persiapan | Status |
|----|-----------|--------|
| 1 | VSCode terbuka, project Smart-ICU-RTOS loaded | ☐ |
| 2 | `src/main.cpp` — pastikan `#define ENABLE_ADVANCED_RTOS_DEMO 1` | ☐ |
| 3 | Terminal: `cd dashboard && python app.py` (Flask running) | ☐ |
| 4 | Browser: `http://localhost:5000` — dashboard terbuka | ☐ |
| 5 | VSCode: Wokwi Start Simulator — Serial Monitor visible | ☐ |
| 6 | Potensiometer di Wokwi bisa diputar via GUI | ☐ |
| 7 | Script video di layar kedua / dihapalkan | ☐ |
| 8 | Mikrofon siap, suara jernih | ☐ |
| 9 | Screen recorder siap (OBS Studio / Windows + Alt + R) | ☐ |
| 10 | Tutup aplikasi lain yang berisik | ☐ |

---

# SEGMEN 1: INTRO & ARSITEKTUR
**Durasi**: 2 menit | **Presenter**: Ketua Kelompok | **Visual**: Project structure di VSCode

### Narasi

**Pembukaan (0:00 - 0:15):**
> "Halo, perkenalkan kami dari kelompok [X]. Tugas besar kami adalah **Smart ICU Patient Monitoring System** — sistem pemantauan pasien ICU berbasis ESP32 dan FreeRTOS."

**Arsitektur Sistem (0:15 - 0:45):**
> "Sistem ini memonitor tiga parameter vital pasien:
> - **BPM** (detak jantung) — 60–150 bpm
> - **Suhu tubuh** — 35–40°C
> - **SpO2** (saturasi oksigen) — 85–100%
>
> Menggunakan **FreeRTOS**, ESP32 menjalankan 7 task secara simultan dengan prioritas berbeda."
>
> *[Arahkan kursor ke `setup()` di main.cpp — tunjukkan 7 baris xTaskCreate]*

**Jelaskan 7 Task (0:45 - 1:30):**
> "Berikut 7 task yang berjalan:
>
> | Task | Prioritas | Periode | Fungsi |
> |---|---|---|---|
> | **AlarmTask** | 5 🔥 | 50 ms | Deteksi kritis, nyalakan LED/buzzer |
> | **HeartRateTask** | 4 | 100 ms | Baca BPM dari potensiometer |
> | **OxygenTask** | 4 | 100 ms | Baca SpO2 dari potensiometer |
> | **TemperatureTask** | 3 | 150 ms | Baca suhu dari potensiometer |
> | **CommandTask** | 2 | 750 ms | Polling perintah dari dashboard |
> | **WiFiTask** | 2 | 2000 ms | Kirim data ke dashboard via HTTP |
> | **MonitoringTask** | 1 | 500 ms | Cetak data ke Serial Monitor |
>
> Prioritas mengikuti **Rate Monotonic Scheduling** — task dengan periode lebih pendek mendapat prioritas lebih tinggi."

**RTOS Primitives (1:30 - 1:50):**
> "FreeRTOS primitives yang digunakan:
> - **Queue** → `patientQueue` — transfer data dari sensor task ke monitoring
> - **Mutex** → `dataMutex` — proteksi shared PatientData dari 7 task
> - **Binary Semaphore** → `emergencySem` — sinyal ISR ke AlarmTask
> - **Spinlock** → `portMUX_TYPE` — proteksi counter di advanced demo"

**Transisi (1:50 - 2:00):**
> "Sekarang kita lihat langsung demo sensor dan alarm. Silakan [Nama Nicholai]."

**[CUT — lanjut Segmen 2]**

---

# SEGMEN 2: DEMO SENSOR & ALARM
**Durasi**: 2 menit | **Presenter**: Nicholai Dandy Nainggolan | **Visual**: Wokwi + Serial Monitor

### Narasi

**Perlihatkan Wokwi (0:00 - 0:15):**
> "Ini adalah **Wokwi simulator**. Ada ESP32, 3 potensiometer, LED merah (alarm), buzzer, dan tombol emergency hijau.
>
> Di Serial Monitor kita lihat data BPM, Temperature, SpO2, dan STATUS yang diperbarui tiap 500ms."

**Demokan BPM (0:15 - 0:45):**
> *[Putar potensiometer BPM ke kanan — driver berlogo "BPM" di Wokwi]*
>
> "Saya putar potensiometer BPM ke kanan. Lihat di Serial Monitor:
>
> - Sebelum: `BPM: 72`
> - Sesudah: `BPM: 115`
>
> **HeartRateTask** membaca GPIO34 via `analogRead()`, memetakan 0–4095 ke rentang 60–150 BPM, dan memperbarui `patient.bpm` setiap 100ms."

**Demokan Suhu (0:45 - 1:00):**
> *[Putar potensiometer suhu]*
>
> "Potensiometer suhu: dari 36.5°C naik ke 38.2°C. Kalau lebih dari 38°C, status jadi CRITICAL."

**Demokan Alarm (1:00 - 1:30):**
> *[Putar potensiometer SpO2 ke kiri — turunkan di bawah 90%]*
>
> "Nah, SpO2 sekarang 87% — di bawah threshold 90%. **Alarm langsung aktif!**
>
> LED merah menyala. Buzzer bunyi. Status di Serial Monitor berubah jadi **CRITICAL**.
>
> Ini terjadi dalam waktu kurang dari 50ms karena **AlarmTask punya prioritas tertinggi** — tidak peduli task lain sedang apa."

**Demokan Emergency Button (1:30 - 1:50):**
> *[Klik tombol emergency hijau di Wokwi]*
>
> "Sekarang saya tekan **tombol emergency** — terhubung ke GPIO27 sebagai FALLING edge interrupt.
>
> ISR (`emergencyISR`) sangat singkat — hanya memberi binary semaphore. AlarmTask yang melakukan proses berat: menyalakan LED, buzzer, update status, dan mencetak log. Ini pola **Deferred Interrupt Processing**."

**Tunjukkan Serial Monitor (1:50 - 2:00):**
> "Serial Monitor menampilkan:
> ```
> [ISR] Emergency button triggered
> BPM    : 105
> TEMP   : 37.2
> SpO2   : 87
> STATUS : CRITICAL
> ```"

**[CUT — lanjut Segmen 3]**

---

# SEGMEN 3: DASHBOARD & KONTROL WEB
**Durasi**: 2 menit | **Presenter**: Muhammad Deva Valery | **Visual**: Browser dashboard

### Narasi

**Perlihatkan Dashboard (0:00 - 0:20):**
> *[Buka browser ke http://localhost:5000]*
>
> "Ini **React dashboard** yang dibangun dengan Vite 7.3.5 dan React 19. Data dari ESP32 dikirim tiap 2 detik ke `POST /data`, lalu dashboard polling `GET /latest` tiap 1 detik.
>
> Ada 3 **Vital Cards**: BPM ❤️, Temperature 🌡️, SpO2 🌊 — dengan nilai besar dan ikon dari lucide-react."

**Tunjukkan Sinkronisasi (0:20 - 0:50):**
> *[Putar potensiometer BPM di Wokwi — dashboard ikut berubah]*
>
> "Sekarang saya putar potensiometer BPM — dashboard langsung berubah. Data ESP32 → Flask → React, semuanya real-time.
>
> Status **NORMAL** (hijau) atau **CRITICAL** (merah) di badge atas — sama dengan yang di Serial Monitor."

**Demokan Web Control (0:50 - 1:20):**
> *[Geser slider BPM ke 140, SpO2 ke 85, klik "Apply Values"]*
>
> "Sekarang saya ubah nilai pasien dari dashboard. **BPM 140, SpO2 85** — klik Apply Values.
>
> CommandTask di ESP32 polling `GET /command` setiap 750ms. Mode berubah ke **web** — ESP32 pakai nilai dari dashboard, bukan dari potensiometer.
>
> Status juga ikut berubah — **CRITICAL** karena nilai di luar batas."

**Demokan Tombol-tombol (1:20 - 1:50):**
> *[Klik tombol-tombol satu per satu]*
>
> "Ada beberapa tombol:
> - **Set Normal**: mengirim nilai normal (BPM 90, Temp 36.8, SpO2 97)
> - **Set Critical**: mengirim nilai kritis (BPM 132, Temp 38.4, SpO2 88)
> - **Emergency**: mengaktifkan emergency dari dashboard
> - **Reset Alarm**: mengembalikan ke normal
> - **Sensor Mode**: mengembalikan ESP32 ke mode baca potensiometer"

**Tutup Segmen (1:50 - 2:00):**
> *[Klik "Sensor Mode"]*
>
> "Klik **Sensor Mode** — ESP32 sekarang baca langsung dari potensiometer lagi.
>
> Selanjutnya [Nama Ardika] akan mendemonstrasikan **race condition**."

**[CUT — lanjut Segmen 4]**

---

# SEGMEN 4: DEMO RACE CONDITION — BEFORE & AFTER
**Durasi**: 2.5 menit | **Presenter**: Ardika Putra Hadian | **Visual**: Serial Monitor Advanced Demo

### Narasi

**Perkenalan (0:00 - 0:20):**
> "Saya akan mendemonstrasikan **Race Condition** — masalah yang terjadi ketika dua task mengakses shared resource tanpa proteksi.
>
> Firmware yang saya jalankan sekarang punya `#define ENABLE_ADVANCED_RTOS_DEMO 1` — demo RTOS tingkat lanjut sudah aktif."

**Phase 1: Tanpa Proteksi (0:20 - 1:10):**
> *[Arahkan ke Serial Monitor, scroll ke bagian [RACE-UNPROTECTED]]*
>
> "Pertama, **tanpa proteksi**. Dua task `UnprotCounterA` dan `UnprotCounterB` berbagi `sharedCounter`. Kodenya:
>
> ```
> temp = sharedCounter;     // Baca
> delay(2ms);               // Kasih waktu task lain baca juga!
> sharedCounter = temp + 1; // Tulis
> ```
>
> *[Bacakan log dengan intonasi]*
>
> | Output | Analisis |
> |---|---|
> | `UnprotCounterA counter: 1` | Task A: baca 0 → tulis 1 👍 |
> | `UnprotCounterB counter: 2` | Task B: baca 1 → tulis 2 👍 |
> | `UnprotCounterA counter: 3` | Task A: baca 2 → tulis 3 👍 |
> | **`UnprotCounterB counter: 3`** | **❌ Task B juga baca 2 → tulis 3! LOST UPDATE!** |
> | `UnprotCounterA counter: 4` | Task A: baca 3 → tulis 4 👍 |
> | `UnprotCounterB counter: 5` | Task B: baca 4 → tulis 5 👍 |
> | **`UnprotCounterA counter: 5`** | **❌ Task A juga baca 4 → tulis 5! LOST UPDATE!** |
>
> Perhatikan: angka 3 muncul dua kali, angka 5 juga dua kali, sementara angka 6 tidak pernah muncul. **Dua increment hilang!** Ini adalah race condition."

**Phase 2: Dengan Proteksi (1:10 - 2:00):**
> *[Arahkan ke bagian [RACE-PROTECTED]]*
>
> "Sekarang **dengan proteksi spinlock**. Codenza:
>
> ```
> portENTER_CRITICAL(&spinlock);  // Kunci! Tak ada task lain bisa jalan
> sharedCounter++;
> portEXIT_CRITICAL(&spinlock);   // Buka
> ```
>
> *[Bacakan log]*
>
> | Output | Analisis |
> |---|---|
> | `ProtCounterA counter: 1` | Atomik: 0 → 1 ✅ |
> | `ProtCounterB counter: 2` | Atomik: 1 → 2 ✅ |
> | `ProtCounterA counter: 3` | Atomik: 2 → 3 ✅ |
> | `ProtCounterB counter: 4` | Atomik: 3 → 4 ✅ |
> | `ProtCounterA counter: 5` | Atomik: 4 → 5 ✅ |
> | `ProtCounterB counter: 6` | Atomik: 5 → 6 ✅ |
>
> **Berurutan sempurna!** Tidak ada satu pun angka yang hilang atau duplikat."

**Perbandingan & Kesimpulan (2:00 - 2:30):**
> "Mari kita bandingkan:
>
> | Aspek | Tanpa Spinlock ❌ | Dengan Spinlock ✅ |
> |---|---|---|
> | Urutan | 1,2,3,3,4,5,5 | 1,2,3,4,5,6,7 |
> | Lost update | ✅ Terjadi | ❌ Tidak |
> | Aman untuk data pasien? | ❌ **TIDAK!** | ✅ **YA!** |
>
> Inilah kenapa sistem produksi seperti kita harus selalu memproteksi shared data dengan **mutex, spinlock, atau queue**. Tanpa proteksi, data pasien BPM atau SpO2 bisa corrupt — dan itu berbahaya di konteks ICU!"

**[CUT — lanjut Segmen 5]**

---

# SEGMEN 5: DEMO PRIORITY INVERSION — BEFORE & AFTER
**Durasi**: 2.5 menit | **Presenter**: Ardika Putra Hadian | **Visual**: Serial Monitor Advanced Demo

### Narasi

**Pengantar (0:00 - 0:20):**
> "Sekarang **Priority Inversion** — fenomena di mana task prioritas tinggi terblokir oleh task prioritas rendah karena masalah resource sharing.
>
> Ada 3 task: **Low** (prio 1), **Medium** (prio 3), **High** (prio 5). High membutuhkan resource yang sedang dipegang Low."

**Phase 1: Binary Semaphore — Tanpa PIP (0:20 - 1:15):**
> *[Arahkan ke log [NO-PIP]]*
>
> "Pertama dengan **binary semaphore** — tanpa priority inheritance.
>
> *[Bacakan log]*
> ```
> [NO-PIP] LOW: Locked semaphore            ← Low pegang resource
> [NO-PIP] MEDIUM: Running workload...      ← Medium jalan (preempt Low!)
> [NO-PIP] MEDIUM: Running workload...      ← Medium terus jalan
> [NO-PIP] HIGH: Acquired after 3412 ms     ← ❌ Baru dapat setelah 3412ms!
> [NO-PIP] LOW: Released semaphore          ← Low baru lepas
> ```
>
> Analisis:
> - Low hanya butuh 3000ms pegang resource
> - Tapi High baru dapat setelah **3412ms** — kelebihan 412ms!
> - Kenapa? Karena Medium (prio 3) preempt Low (prio 1) — Low tidak bisa lepas resource
> - High (prio 5) terpaksa menunggu Medium selesai — **ini priority inversion!**
>
> Di sistem ICU, 412ms keterlambatan alarm adalah **risiko keselamatan pasien**."

**Phase 2: FreeRTOS Mutex — Dengan PIP (1:15 - 2:10):**
> *[Arahkan ke log [WITH-PIP]]*
>
> "Sekarang dengan **FreeRTOS mutex** — yang mendukung priority inheritance.
>
> *[Bacakan log]*
> ```
> [WITH-PIP] LOW: Locked mutex              ← Low pegang mutex
> [WITH-PIP] MEDIUM: Running workload...    ← Medium jalan
> [WITH-PIP] HIGH: Acquired after 3024 ms   ← ✅ Dapat dalam 3024ms!
> [WITH-PIP] LOW: Released mutex           ← Low lepas
> ```
>
> Analisis:
> - High dapat mutex dalam **3024ms** — hanya 24ms overhead!
> - Kenapa beda? Karena saat High menunggu mutex milik Low, FreeRTOS **menaikkan prioritas Low menjadi 5**
> - Medium (prio 3) tidak bisa preempt Low (sekarang prio 5) — Low cepat selesai dan lepas mutex
> - Baru setelah itu prioritas Low kembali ke 1
>
> Priority inheritance membuat **waktu blocking 95% lebih cepat!** (dari 412ms → 24ms)"

**Perbandingan (2:10 - 2:30):**
> *[Tunjukkan tabel]*
>
> | Metrik | Binary Sem (NoPIP) | Mutex (PIP) |
> |---|---|---|
> | Waktu High menunggu | 3412 ms | 3024 ms |
> | Overhead | 412 ms ❌ | 24 ms ✅ |
> | Efektivitas | — | **95% lebih cepat** |
>
> Inilah kenapa proyek kami menggunakan `xSemaphoreCreateMutex()` untuk semua shared data — **bukan binary semaphore**. Mutex menyediakan priority inheritance yang merupakan fitur keamanan penting di sistem safety-critical."

**[CUT — lanjut Segmen 6]**

---

# SEGMEN 6: DEMO DEADLOCK PREVENTION
**Durasi**: 1.5 menit | **Presenter**: Anggota 4 | **Visual**: Serial Monitor Advanced Demo

### Narasi

**Pengantar (0:00 - 0:20):**
> "Selanjutnya **Deadlock Prevention**. Deadlock adalah kondisi di mana dua task saling menunggu resource satu sama lain — tidak ada yang bisa jalan.
>
> Kondisi ini dikenal dengan **Circular Wait**: Task A pegang Resource1, butuh Resource2. Task B pegang Resource2, butuh Resource1."

**Skenario (0:20 - 0:50):**
> *[Arahkan ke kode di VSCode]*
>
> "Kodenya seperti ini:
> - **DeadlockTaskA**: ambil resource1 → delay 500ms → ambil resource2 (timeout 1000ms)
> - **DeadlockTaskB**: ambil resource2 → delay 500ms → ambil resource1 (timeout 1000ms)
>
> Tanpa pencegahan, kedua task akan saling menunggu selamanya."

**Hasil (0:50 - 1:20):**
> *[Arahkan ke log [DEADLOCK...]]*
>
> "Tapi kita cegah dengan **timeout**:
> ```
> [DEADLOCK-A] DEADLOCK PREVENTED (timeout)
> [DEADLOCK-B] Acquired both resources
> ```
>
> Ketika TaskA gagal mendapatkan resource2 dalam 1000ms, ia:
> 1. Melepas resource1 — mengakhiri circular wait
> 2. Mencetak `DEADLOCK PREVENTED`
> 3. Menunggu 2 detik lalu mencoba lagi
>
> TaskB yang tadinya menunggu resource1, sekarang bisa mendapatkannya dan melanjutkan eksekusi."

**Kesimpulan (1:20 - 1:30):**
> "Dengan timeout sederhana, deadlock berhasil dicegah. Ini adalah teknik **deadlock avoidance** yang ringan dan efektif — tidak perlu algoritma Banker yang kompleks."

**[CUT — lanjut Segmen 7]**

---

# SEGMEN 7: SAFETY-CRITICAL & CLOSING
**Durasi**: 1.5 menit | **Presenter**: Ketua Kelompok | **Visual**: LAPORAN.md BAB 6.8 atau slide

### Narasi

**Safety-Critical Integration (0:00 - 0:50):**
> "Tema kontemporer yang kami pilih adalah **Safety-Critical Systems**. Sistem ICU adalah safety-critical karena kegagalan bisa membahayakan nyawa pasien.
>
> Berikut 10 implementasi safety-critical di proyek kami:
>
> | No | Prinsip | Implementasi |
> |----|---------|-------------|
> | 1 | **Alarm prioritas tertinggi** | AlarmTask = prioritas 5, periode 50ms |
> | 2 | **ISR singkat** | emergencyISR hanya beri semaphore, bukan proses berat |
> | 3 | **Deadlock prevention** | Timeout 1000ms pada `xSemaphoreTake` |
> | 4 | **Priority inheritance** | `xSemaphoreCreateMutex()` untuk shared data |
> | 5 | **Race condition prevention** | `dataMutex` + `spinlock` |
> | 6 | **Stack monitoring** | `uxTaskGetStackHighWaterMark()` tiap 5 siklus |
> | 7 | **Non-blocking I/O** | WiFiTask prioritas rendah — HTTP tidak blokir alarm |
> | 8 | **HTTP timeout** | `http.setTimeout(1000)` — tidak hang |
> | 9 | **Memory safety** | `char status[20]` bukan `String` — cegah fragmentasi |
> | 10 | **Schedulability verified** | U = 0.236 < LUB 0.735 — semua task feasible |

**Kesimpulan (0:50 - 1:20):**
> "**Kesimpulan proyek:**
>
> ✅ **7 FreeRTOS task** berjalan simultan dengan prioritas rate-monotonic
> ✅ **Alarm respons dalam 50ms** — dijamin oleh scheduler preemptive
> ✅ **HTTP tidak blokir sensor** — WiFiTask prioritas rendah
> ✅ **Mutex, queue, semaphore** — sinkronisasi aman tanpa race condition
> ✅ **Priority inversion** — dicegah dengan priority inheritance (95% lebih cepat)
> ✅ **Deadlock** — dicegah dengan timeout
> ✅ **CPU utilization 23.6%** — masih banyak sisa untuk fitur tambahan
> ✅ **RAM 14.5% (47KB)**, **Flash 71.9% (942KB)**
> ✅ **Safety-Critical Systems** — 10 prinsip terintegrasi penuh

**Penutup (1:20 - 1:30):**
> "Terima kasih. Kami mempersilakan pertanyaan dari dosen dan rekan-rekan.
>
> Source code tersedia di: **https://github.com/ArdeezYy/Smart-ICU-RTOS**"

**[FADE OUT]**

---

# 📤 SETELAH REKAM

1. Upload video ke Google Drive atau YouTube (unlisted)
2. Buka `LAPORAN.md` — cari bagian **Lampiran D: Video Demo**
3. Ganti `[Link Video]` dengan URL video yang sudah diupload
4. Commit & push ke GitHub:
   ```bash
   git add -A
   git commit -m "Update video demo link in LAPORAN.md"
   git push origin main
   ```

---

*Script selesai. Durasi: ~14 menit — Selamat syuting! 🎬*
