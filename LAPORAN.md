# SMART ICU PATIENT MONITORING SYSTEM
## Berbasis ESP32 dan FreeRTOS

---

**Laporan Tugas**

Mata Kuliah: Sistem Operasi Real-Time (RTOS)

---

**Disusun oleh:**

Nama Mahasiswa / NIM

---

**Program Studi Teknik Informatika**
**Fakultas Ilmu Komputer**
**Universitas ...**
**2026**

---

## DAFTAR ISI

1. [BAB 1: PENDAHULUAN](#bab-1-pendahuluan)
   - 1.1 Latar Belakang
   - 1.2 Rumusan Masalah
   - 1.3 Tujuan
   - 1.4 Batasan Masalah
2. [BAB 2: TINJAUAN PUSTAKA](#bab-2-tinjauan-pustaka)
   - 2.1 Sistem Operasi Real-Time (RTOS)
   - 2.2 FreeRTOS
   - 2.3 ESP32
   - 2.4 Wokwi Simulator
   - 2.5 Flask
   - 2.6 React
3. [BAB 3: PERANCANGAN SISTEM](#bab-3-perancangan-sistem)
   - 3.1 Arsitektur Sistem
   - 3.2 Diagram Blok
   - 3.3 Perancangan Task FreeRTOS
   - 3.4 Pin Mapping
   - 3.5 Alur Data
   - 3.6 State Machine
4. [BAB 4: IMPLEMENTASI](#bab-4-implementasi)
   - 4.1 Implementasi Task FreeRTOS
   - 4.2 Queue dan Semaphore
   - 4.3 Penanganan ISR
   - 4.4 Web Dashboard (Flask dan React)
   - 4.5 Simulasi Wokwi
   - 4.6 Advanced RTOS Demo
5. [BAB 5: HASIL PENGUJIAN](#bab-5-hasil-pengujian)
   - 5.1 Hasil Kompilasi Firmware
   - 5.2 Pengujian API Endpoint
   - 5.3 Dashboard Web
   - 5.4 Simulasi Wokwi
6. [BAB 6: KESIMPULAN DAN SARAN](#bab-6-kesimpulan-dan-saran)
   - 6.1 Kesimpulan
   - 6.2 Saran
7. [DAFTAR PUSTAKA](#daftar-pustaka)

---

## BAB 1: PENDAHULUAN

### 1.1 Latar Belakang

Perkembangan teknologi Internet of Things (IoT) telah membawa perubahan signifikan dalam dunia kesehatan, khususnya pada sistem pemantauan pasien di rumah sakit. _Intensive Care Unit_ (ICU) merupakan unit perawatan intensif yang memerlukan pemantauan _vital sign_ pasien secara kontinu dan _real-time_. Parameter seperti detak jantung (_heart rate_ / BPM), suhu tubuh (_temperature_), dan saturasi oksigen (SpO2) harus dipantau secara ketat karena perubahan sekecil apapun dapat mengindikasikan kondisi kritis yang memerlukan tindakan medis segera.

Namun, sistem pemantauan pasien komersial saat ini memiliki harga yang relatif mahal dan seringkali bersifat _proprietary_, sehingga sulit dikembangkan atau dimodifikasi. Di sisi lain, mikrokontroler modern seperti ESP32 menawarkan kemampuan komputasi yang cukup dengan harga yang sangat terjangkau, serta dilengkapi dengan konektivitas Wi-Fi dan Bluetooth _built-in_.

Sistem operasi _real-time_ (RTOS) menjadi komponen krusial dalam pengembangan sistem pemantauan medis. RTOS menjamin bahwa tugas-tugas kritis (_deadline-sensitive tasks_) mendapatkan jatah waktu prosesor yang tepat sehingga respons terhadap perubahan kondisi pasien dapat dilakukan secara cepat dan dapat diprediksi (_deterministic_). FreeRTOS, sebagai salah satu RTOS _open-source_ yang populer, menyediakan mekanisme _task scheduling_, _queue_, _semaphore_, _mutex_, dan penanganan _interrupt_ yang sangat cocok untuk aplikasi _embedded_ _real-time_.

Oleh karena itu, proyek ini mengusulkan pengembangan **Smart ICU Patient Monitoring System** yang mengintegrasikan ESP32 dengan FreeRTOS untuk membaca sensor _vital sign_ virtual, memproses data secara _real-time_, menyalakan alarm ketika kondisi kritis terdeteksi, dan menampilkan data melalui _web dashboard_ berbasis Flask dan React.

### 1.2 Rumusan Masalah

Berdasarkan latar belakang di atas, rumusan masalah dalam proyek ini adalah:

1. Bagaimana merancang dan mengimplementasikan sistem pemantauan pasien ICU _real-time_ menggunakan ESP32 dan FreeRTOS?
2. Bagaimana mengelola beberapa tugas (_task_) dengan prioritas dan periode yang berbeda menggunakan _scheduler_ FreeRTOS?
3. Bagaimana menggunakan mekanisme sinkronisasi FreeRTOS (_queue_, _mutex_, _binary semaphore_, _spinlock_) untuk mengamankan data bersama (_shared data_) dan komunikasi antar-task?
4. Bagaimana menangani _interrupt_ tombol darurat (_emergency button_) dengan aman menggunakan pola _deferred interrupt processing_?
5. Bagaimana membangun _web dashboard_ yang menampilkan data _vital sign_ secara _real-time_ menggunakan Flask dan React?
6. Bagaimana mendemonstrasikan konsep RTOS tingkat lanjut seperti _deadlock prevention_, _priority inversion_, dan _thread-safe counter_?

### 1.3 Tujuan

Tujuan dari proyek ini adalah:

1. Merancang sistem pemantauan pasien ICU berbasis ESP32 dengan arsitektur multitasking FreeRTOS.
2. Mengimplementasikan tujuh _task_ FreeRTOS yang menangani pembacaan sensor, pemrosesan alarm, komunikasi Wi-Fi, dan monitoring.
3. Mengimplementasikan mekanisme sinkronisasi FreeRTOS (_queue_, _mutex_, _binary semaphore_) untuk menghindari _race condition_ dan _data corruption_.
4. Membangun sistem alarm _real-time_ yang responsif terhadap kondisi kritis dan tombol darurat.
5. Mengembangkan _web dashboard_ interaktif dengan Flask (backend) dan React (frontend) untuk visualisasi data.
6. Mendemonstrasikan dan menganalisis fenomena RTOS tingkat lanjut (_deadlock_, _priority inversion_, _protected counter_).

### 1.4 Batasan Masalah

Batasan masalah dalam proyek ini adalah:

1. Sistem menggunakan simulator Wokwi untuk menggantikan komponen fisik (potensiometer sebagai sensor, LED dan _buzzer_ sebagai aktuator).
2. Data sensor merupakan data simulasi dari pembacaan potensiometer analog, bukan dari sensor medis sesungguhnya.
3. Komunikasi antara ESP32 dan _dashboard_ menggunakan protokol HTTP sederhana melalui koneksi Wi-Fi.
4. Sistem tidak menggunakan database; data hanya disimpan sementara di memori _server_ Flask.
5. _Advanced RTOS demo_ (_deadlock prevention_, _priority inversion_, _protected counter_) diimplementasikan sebagai fitur opsional yang dinonaktifkan secara _default_ agar tidak mengganggu _deadline_ monitoring utama.
6. Fokus utama adalah pada aspek RTOS dan arsitektur sistem, bukan pada akurasi medis atau validasi klinis.

---

## BAB 2: TINJAUAN PUSTAKA

### 2.1 Sistem Operasi Real-Time (RTOS)

**Sistem Operasi Real-Time** (RTOS) adalah sistem operasi yang dirancang untuk melayani aplikasi dengan batasan waktu yang ketat (_deadline_). Berbeda dengan sistem operasi tujuan umum (_general-purpose OS_) seperti Windows atau Linux yang berusaha memaksimalkan _throughput_ dan _fairness_, RTOS mengutamakan **determinisme** dan **prediktabilitas** waktu eksekusi.

#### 2.1.1 Task

_Task_ (atau _thread_) adalah unit dasar eksekusi dalam RTOS. Setiap _task_ menjalankan fungsi tertentu secara independen dan memiliki _context_ sendiri (_stack_, _program counter_, _register_). Dalam FreeRTOS, _task_ dibuat dengan `xTaskCreate()` dan dapat berada dalam salah satu dari beberapa status:

- **Running**: sedang menggunakan CPU.
- **Ready**: siap dijalankan tetapi CPU sedang digunakan oleh _task_ lain.
- **Blocked**: menunggu suatu kejadian (_event_) atau _timeout_.
- **Suspended**: tidak dapat dijadwalkan sampai di-_resume_ secara eksplisit.

#### 2.1.2 Scheduler

_Scheduler_ adalah komponen inti RTOS yang menentukan _task_ mana yang harus berjalan pada suatu waktu. FreeRTOS menggunakan **preemptive scheduler** dengan prioritas tetap (_fixed-priority preemptive scheduling_). _Task_ dengan prioritas lebih tinggi akan selalu mendahului (_preempt_) _task_ dengan prioritas lebih rendah. Jika terdapat beberapa _task_ dengan prioritas yang sama, mereka akan dijadwalkan secara _round-robin_ (_time-sliced_).

Kebijakan penjadwalan ini sangat cocok untuk sistem _real-time_ keras (_hard real-time_) karena menjamin bahwa _task_ kritis (seperti alarm) akan selalu mendapatkan CPU saat dibutuhkan.

#### 2.1.3 Queue

_Queue_ adalah mekanisme komunikasi antar-task (_Inter-Task Communication_ / ITC) yang memungkinkan pertukaran data secara aman. _Queue_ menggunakan model _First-In-First-Out_ (FIFO) dan dapat menampung sejumlah data dengan ukuran tetap. Operasi utama pada _queue_ adalah:

- `xQueueSend()`: mengirim data ke _queue_ (akan mem-_block_ jika _queue_ penuh).
- `xQueueReceive()`: menerima data dari _queue_ (akan mem-_block_ jika _queue_ kosong).
- `xQueueOverwrite()`: menimpa data terakhir di _queue_ (hanya untuk _queue_ dengan panjang 1).

#### 2.1.4 Semaphore

Semaphore adalah mekanisme sinkronisasi yang digunakan untuk mengontrol akses ke sumber daya bersama atau untuk memberi sinyal antar-task. FreeRTOS menyediakan beberapa jenis semaphore:

- **Binary Semaphore**: semaphore dengan dua nilai (0 dan 1). Cocok untuk mekanisme _signaling_ (misalnya, memberi tahu _task_ bahwa suatu _interrupt_ telah terjadi).
- **Counting Semaphore**: semaphore dengan nilai > 1, digunakan untuk melacak sejumlah sumber daya.

#### 2.1.5 Mutex

Mutex (_Mutual Exclusion_) adalah jenis semaphore khusus yang digunakan untuk melindungi sumber daya bersama (_shared resource_) dari akses simultan. Karakteristik utama mutex meliputi:

- **Priority Inheritance**: mencegah _priority inversion_ dengan sementara menaikkan prioritas _task_ yang memegang mutex agar _task_ prioritas lebih tinggi tidak menunggu terlalu lama.
- **Recursive**: memungkinkan _task_ yang sama mengambil mutex yang sama berkali-kali (perlu dirilis sebanyak pengambilannya).
- Mutex hanya boleh digunakan dalam _task context_ (bukan ISR).

#### 2.1.6 ISR (Interrupt Service Routine)

ISR adalah fungsi khusus yang dipanggil oleh prosesor sebagai respons terhadap sinyal _interrupt_ dari perangkat keras. Dalam RTOS, ISR harus ditulis dengan hati-hati karena:

- ISR berjalan di luar kendali _scheduler_.
- ISR harus sesingkat mungkin (_minimal latency_).
- Operasi yang memungkinkan _context switch_ (seperti `xSemaphoreGiveFromISR()`) memerlukan parameter `pxHigherPriorityTaskWoken` untuk memberi tahu _scheduler_ perlunya _yield_.

Pola **deferred interrupt processing** digunakan untuk memindahkan pekerjaan berat dari ISR ke _task_ biasa dengan cara memberi sinyal melalui semaphore atau _queue_.

#### 2.1.7 Priority Inversion

Priority inversion adalah situasi di mana _task_ berprioritas tinggi terpaksa menunggu karena _task_ berprioritas rendah sedang memegang sumber daya yang dibutuhkan, sementara _task_ berprioritas menengah (_medium_) mendahului _task_ berprioritas rendah tersebut. Akibatnya, _task_ berprioritas tinggi dieksekusi setelah _task_ berprioritas menengah — suatu pelanggaran terhadap aturan prioritas.

Solusi umum untuk _priority inversion_ adalah **Priority Inheritance Protocol**, diimplementasikan langsung oleh mutex FreeRTOS.

#### 2.1.8 Deadlock

Deadlock adalah kondisi di mana dua atau lebih _task_ saling menunggu sumber daya yang dipegang oleh _task_ lain, sehingga tidak ada _task_ yang dapat melanjutkan eksekusi. Empat kondisi yang diperlukan untuk terjadinya deadlock (Coffman conditions) adalah:

1. **Mutual Exclusion**: sumber daya tidak dapat digunakan bersama.
2. **Hold and Wait**: _task_ memegang satu sumber daya sambil menunggu sumber daya lain.
3. **No Preemption**: sumber daya tidak dapat diambil paksa.
4. **Circular Wait**: terdapat rantai melingkar _task_ yang saling menunggu.

Strategi pencegahan deadlock meliputi penggunaan _timeout_ pada operasi pengambilan mutex dan pengaturan urutan pengambilan mutex secara konsisten.

### 2.2 FreeRTOS

FreeRTOS adalah sistem operasi _real-time_ _open-source_ yang dirancang untuk perangkat _embedded_ dengan sumber daya terbatas. FreeRTOS ditulis dalam bahasa C dan telah di-porting ke lebih dari 40 arsitektur mikrokontroler, termasuk ARM Cortex-M, RISC-V, dan Xtensa (ESP32).

Fitur utama FreeRTOS:

- **Preemptive Scheduling** dengan prioritas tetap dan _round-robin_.
- **Mekanisme Sinkronisasi**: _queue_, _binary semaphore_, _counting semaphore_, _mutex_ (dengan _priority inheritance_), _recursive mutex_, _event group_, _stream buffer_.
- **Manajemen Memori**: lima skema alokasi heap (`heap_1` hingga `heap_5`).
- **Tickless Mode**: untuk penghematan daya.
- **Software Timer**: timer berbasis software dengan _callback_.
- **Stack Overflow Detection**: deteksi _stack overflow_ pada _task_.
- **TCOB (Task Control Block)**: struktur data yang menyimpan status, prioritas, _stack pointer_, dan informasi lain dari setiap _task_.

### 2.3 ESP32

ESP32 adalah mikrokontroler _system-on-chip_ (SoC) yang dikembangkan oleh Espressif Systems. ESP32 memiliki arsitektur Xtensa LX6 dual-core (atau single-core pada beberapa varian) dengan fitur:

- **CPU**: Xtensa 32-bit LX6, clock hingga 240 MHz.
- **Memori**: 520 KB SRAM, 448 KB ROM, 4 MB Flash eksternal.
- **Konektivitas**: Wi-Fi 802.11 b/g/n, Bluetooth 4.2 BR/EDR dan BLE.
- **GPIO**: 34 pin GPIO yang dapat diprogram.
- **ADC**: 2 × 12-bit SAR ADC dengan total 18 kanal.
- **Interface**: SPI, I2C, I2S, UART, CAN, Ethernet MAC, PWM, Touch Sensor.
- **FreeRTOS**: ESP32 menggunakan FreeRTOS sebagai sistem operasi _default_ pada ESP-IDF.

Dalam proyek ini, ESP32 digunakan pada _development board_ ESP32-DevKit-C-V4 dengan framework Arduino.

### 2.4 Wokwi Simulator

Wokwi (https://wokwi.com) adalah simulator sirkuit elektronik dan mikrokontroler yang berjalan di _web browser_. Wokwi mendukung simulasi berbagai komponen, termasuk:

- Mikrokontroler: ESP32, Arduino Uno, Raspberry Pi Pico, STM32, dll.
- Komponen: LED, _buzzer_, potensiometer, _push button_, LCD, sensor, dll.
- Antarmuka: Serial Monitor, _Logic Analyzer_, Wi-Fi (terbatas).

Wokwi sangat berguna untuk pengembangan dan pengujian firmware _embedded_ tanpa memerlukan perangkat keras fisik. Wokwi juga terintegrasi dengan PlatformIO dan Visual Studio Code melalui ekstensi.

### 2.5 Flask

Flask adalah _micro-framework_ web berbasis Python yang ringan, modular, dan mudah dikembangkan. Flask menyediakan:

- **Routing URL**: dekorator `@app.route()` untuk menghubungkan URL dengan fungsi.
- **Request Handling**: objek `request` untuk mengakses data JSON, form, parameter.
- **Response**: `jsonify()` untuk mengembalikan JSON.
- **Template**: Jinja2 sebagai _template engine_.

Dalam proyek ini, Flask berfungsi sebagai _backend_ API yang menerima data dari ESP32, menyediakan data ke _frontend_ React, dan memproses perintah kontrol dari _dashboard_.

### 2.6 React

React adalah _library_ JavaScript untuk membangun antarmuka pengguna (_user interface_) yang dikembangkan oleh Meta (Facebook). Karakteristik utama React:

- **Component-Based**: UI dibangun dari komponen-komponen yang independen dan dapat digunakan kembali.
- **Virtual DOM**: meminimalkan manipulasi DOM langsung sehingga meningkatkan performa.
- **JSX**: sintaks _extensi_ JavaScript yang memungkinkan penulisan markup HTML di dalam kode JavaScript.
- **Hooks**: fungsi seperti `useState`, `useEffect`, `useMemo` untuk mengelola _state_ dan _side effects_ dalam komponen _functional_.

Dalam proyek ini, React digunakan untuk membangun _dashboard_ interaktif yang menampilkan data _vital sign_ secara real-time melalui komponen kartu dan kontrol _slider_.

---

## BAB 3: PERANCANGAN SISTEM

### 3.1 Arsitektur Sistem

Sistem Smart ICU Patient Monitoring terdiri dari tiga komponen utama yang saling terintegrasi:

1. **ESP32 Firmware (FreeRTOS)**: bertanggung jawab untuk membaca data sensor, memproses alarm, dan berkomunikasi dengan _server_ melalui Wi-Fi.
2. **Flask Backend**: menyediakan REST API untuk menerima data dari ESP32, menyimpan data terkini, dan meneruskan perintah kontrol dari _dashboard_.
3. **React Frontend**: menampilkan data _vital sign_ secara _real-time_ dan menyediakan antarmuka untuk mengirim perintah kontrol ke ESP32.

**Gambar 3.1: Arsitektur Sistem**

```
+------------------+       HTTP POST /data       +------------------+
|                  | --------------------------> |                  |
|   ESP32 +        |       HTTP GET /command     |   Flask Server   |
|   FreeRTOS       | <-------------------------- |   (Backend)      |
|                  |                             |   :5000          |
|  7 Tasks         |                             |                  |
|  Queue, Mutex,   |                             |  REST API        |
|  Binary Sem      |                             |  Thread Lock     |
+------------------+                             +--------+---------+
                                                          |
                                                          | JSON Response
                                                          |
                                                 +--------v---------+
                                                 |  React Frontend  |
                                                 |   (Dashboard)    |
                                                 |   Vite + React   |
                                                 +------------------+
```

### 3.2 Diagram Blok

**Gambar 3.2: Diagram Blok Sistem**

```
+========================+    +========================+    +========================+
|  INPUT                 |    |  PROSES (ESP32+RTOS)   |    |  OUTPUT                |
|                        |    |                        |    |                        |
| +------------------+   |    | +--------+ +---------+ |    | +------------------+   |
| | Pot BPM      (34)|---+--->| Heart  | | Alarm    | |    | | LED Merah   (25) |   |
| +------------------+   |    | | Rate  | | Task     | |    | +------------------+   |
| +------------------+   |    | | Task  | | (Prio 5) | |    | +------------------+   |
| | Pot Temp      (35)|---+--->| (Prio4)| +----+-----+ |    | | Buzzer       (26)|   |
| +------------------+   |    | +-------+      |       |    | +------------------+   |
| +------------------+   |    | +-------+      |       |    | +------------------+   |
| | Pot SpO2      (32)|---+--->| Oxygen|      |       |    | | Serial Monitor   |   |
| +------------------+   |    | | Task  |      |       |    | +------------------+   |
| +------------------+   |    | | (Prio4)      |       |    | +------------------+   |
| | Emergency Button|   |    | +-------+      |       |    | | Web Dashboard    |   |
| | (27) - ISR      |---+--->| +---------+    |       |    | +------------------+   |
| +------------------+   |    | | Temp    |    |       |    |                        |
|                        |    | | Task    |    |       |    |                        |
|                        |    | | (Prio3) |    |       |    |                        |
|                        |    | +---------+    |       |    |                        |
|                        |    | +------+ +-----+------+|    |                        |
|                        |    | |WiFi  | |Command     ||    |                        |
|                        |    | |Task  | |Task (Prio2)||    |                        |
|                        |    | |(Prio2)|+------------+||    |                        |
|                        |    | +------+ +------------+||    |                        |
|                        |    | +------------+         ||    |                        |
|                        |    | |Monitoring  |         ||    |                        |
|                        |    | |Task (Prio1)|         ||    |                        |
|                        |    | +------------+         ||    |                        |
+========================+    +========================+    +========================+
```

### 3.3 Perancangan Task FreeRTOS

Sistem dirancang dengan tujuh _task_ FreeRTOS yang berjalan secara paralel. Setiap _task_ memiliki periode eksekusi, prioritas, dan alokasi _stack_ tertentu sesuai dengan fungsi dan urgensi masing-masing.

**Tabel 3.1: Desain Task FreeRTOS**

| No | Nama Task          | Periode | Prioritas | Ukuran Stack | Fungsi                                              |
|----|--------------------|---------|-----------|--------------|-----------------------------------------------------|
| 1  | AlarmTask          | 50 ms   | 5 (Tertinggi) | 4096      | Memeriksa kondisi kritis, mengendalikan LED/buzzer  |
| 2  | HeartRateTask      | 100 ms  | 4         | 2048         | Membaca potensiometer BPM, memperbarui data pasien  |
| 3  | OxygenTask         | 100 ms  | 4         | 2048         | Membaca potensiometer SpO2, memperbarui data pasien |
| 4  | TemperatureTask    | 150 ms  | 3         | 2048         | Membaca potensiometer suhu, memperbarui data pasien |
| 5  | WiFiTask           | 2000 ms | 2         | 8192         | Mengirim data pasien ke server Flask via HTTP POST  |
| 6  | CommandTask        | 750 ms  | 2         | 6144         | Mendapatkan perintah kontrol dari server via HTTP GET|
| 7  | MonitoringTask     | 500 ms  | 1 (Terendah) | 3072      | Menampilkan data monitor ke Serial Console          |

**Penjelasan Prioritas:**

- **AlarmTask (Prioritas 5)**: Mendapat prioritas tertinggi karena harus merespon kondisi kritis dalam waktu singkat (50 ms). Keterlambatan pada _task_ ini dapat berakibat fatal secara medis.
- **HeartRateTask & OxygenTask (Prioritas 4)**: Kedua _task_ sensor dengan periode 100 ms ini memiliki prioritas tinggi karena data detak jantung dan saturasi oksigen adalah parameter paling kritis dalam monitoring ICU.
- **TemperatureTask (Prioritas 3)**: Suhu tubuh berubah lebih lambat sehingga periode 150 ms dengan prioritas 3 sudah memadai.
- **WiFiTask & CommandTask (Prioritas 2)**: Komunikasi jaringan memiliki prioritas lebih rendah karena sifatnya yang _best-effort_ dan periode yang lebih panjang.
- **MonitoringTask (Prioritas 1)**: _Task_ ini bersifat informasional (menampilkan data ke serial) sehingga mendapat prioritas paling rendah.

### 3.4 Pin Mapping

**Tabel 3.2: Pemetaan Pin ESP32**

| Fungsi                  | GPIO | Mode       | Keterangan                           |
|-------------------------|------|------------|--------------------------------------|
| BPM Potentiometer       | 34   | INPUT (ADC)| ADC1_CH6, membaca tegangan potensiometer |
| Temperature Potentiometer | 35 | INPUT (ADC)| ADC1_CH7, membaca tegangan potensiometer |
| SpO2 Potentiometer      | 32   | INPUT (ADC)| ADC1_CH4, membaca tegangan potensiometer |
| Emergency Button        | 27   | INPUT_PULLUP | _Pull-up internal_, _interrupt_ FALLING edge |
| Alarm LED               | 25   | OUTPUT     | LED merah, HIGH = menyala             |
| Buzzer                  | 26   | OUTPUT (PWM)| PWM via `ledc`, frekuensi 1000 Hz     |

### 3.5 Alur Data

**Gambar 3.3: Diagram Alur Data**

```
  [Pot BPM] --> [HeartRateTask] ------+
  [Pot Temp] --> [TemperatureTask] ---+--> [PatientData] --> [patientQueue] --> [MonitoringTask]
  [Pot SpO2] --> [OxygenTask] --------+        |                                       |
                                               | (dataMutex)                            |
                                               v                                       v
                                        [WiFiTask] ------ HTTP POST /data -----> [Flask Server]
                                                                                        |
                                                                                        v
                                        [CommandTask] <--- HTTP GET /command ---- [Flask Server]
                                               |                                        ^
                                               v                                        |
                                        [ControlCommand] -------- [React Dashboard] ---+
                                               |
                                               v (dataMutex)
                                        [PatientData] diperbarui jika mode = "web"
```

**Penjelasan alur data:**

1. **Sensor Tasks** (HeartRateTask, TemperatureTask, OxygenTask) membaca nilai analog dari potensiometer dan menuliskan ke struktur `PatientData` yang dilindungi oleh `dataMutex`.
2. Setiap kali data diperbarui, _task_ sensor memanggil `publishPatientSnapshot()` yang menyalin data ke `patientQueue` menggunakan `xQueueOverwrite()`.
3. **MonitoringTask** membaca dari `patientQueue` dan menampilkan data ke Serial Monitor setiap 500 ms.
4. **WiFiTask** (setiap 2000 ms) mengambil _snapshot_ data pasien (dilindungi `dataMutex`) dan mengirimkannya ke server Flask melalui `POST /data`.
5. **CommandTask** (setiap 750 ms) melakukan _polling_ ke server Flask melalui `GET /command` untuk mendapatkan perintah kontrol terbaru.
6. Jika mode kontrol adalah **"web"**, nilai BPM, suhu, dan SpO2 dari _dashboard_ akan digunakan, menggantikan pembacaan sensor lokal.

### 3.6 State Machine

**Gambar 3.4: State Machine Sistem**

```
                   +-----------+
                   |   INIT    |
                   +-----+-----+
                         |
                         v
                   +-----------+
                   |  SETUP    |
                   | (inisialisasi |
                   |  HW, RTOS) |
                   +-----+-----+
                         |
                         v
              +----------+----------+
              |                     |
              v                     v
      +-------+-------+     +------+------+
      |  SENSOR MODE   |     |  WEB MODE   |
      | (mode="sensor")|     | (mode="web")|
      |                |     |             |
      | Baca pot ADC   |     | Data dari   |
      | Kirim ke server|     | dashboard   |
      +-------+-------+     +------+------+
              |                     |
              +----------+----------+
                         |
                         v
              +----------+----------+
              |                     |
              v                     v
       +------+-------+     +------+-------+
       |   NORMAL     |     |   CRITICAL   |
       |               |     |              |
       | Status normal |     | LED+Buzzer   |
       | Alarm mati    |     | ON, alarm    |
       +------+-------+     +------+-------+
              |                     |
              +----------+----------+
                         |
                         v
                    (loop terus menerus)
```

**Penjelasan State Machine:**

1. **INIT**: Sistem dimulai, Serial diinisialisasi.
2. **SETUP**: Pin dikonfigurasi, _queue_, _mutex_, semaphore dibuat, ISR dipasang, semua _task_ FreeRTOS dibuat.
3. **SENSOR MODE** vs **WEB MODE**: ESP32 dapat beroperasi dalam mode sensor (membaca dari potensiometer) atau mode web (menggunakan data dari _dashboard_). Mode ditentukan oleh perintah dari Flask.
4. **NORMAL** vs **CRITICAL**: Status pasien ditentukan oleh ambang batas (BPM > 130, suhu > 38°C, SpO2 < 90%, atau tombol darurat ditekan). Alarm LED dan _buzzer_ aktif hanya pada status CRITICAL.

---

## BAB 4: IMPLEMENTASI

### 4.1 Implementasi Task FreeRTOS

#### 4.1.1 HeartRateTask

HeartRateTask bertugas membaca nilai analog dari potensiometer BPM (GPIO34) dan memetakannya ke rentang 60–150 BPM. _Task_ ini menggunakan `dataMutex` untuk mengamankan akses ke struktur `PatientData`.

```cpp
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
```

Fungsi `vTaskDelayUntil()` digunakan untuk mempertahankan periode eksekusi yang presisi (100 ms) meskipun terjadi variasi waktu eksekusi (_jitter_). Ini adalah praktik standar RTOS untuk _periodic tasks_.

#### 4.1.2 TemperatureTask

TemperatureTask membaca suhu dari potensiometer GPIO35 dan memetakan nilai ADC (0–4095) ke rentang suhu 35.0–40.0°C menggunakan fungsi `mapFloat()` khusus karena `map()` Arduino hanya mendukung integer.

```cpp
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
```

#### 4.1.3 OxygenTask

OxygenTask membaca saturasi oksigen (SpO2) dari potensiometer GPIO32, memetakannya ke rentang 85–100%, dengan periode 100 ms dan prioritas 4.

```cpp
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
```

Ketiga _task_ sensor di atas menggunakan pola yang sama: ambil mutex → baca sensor (jika mode sensor) → perbarui data → publikasikan ke _queue_ → lepaskan mutex. Penggunaan `vTaskDelayUntil()` memastikan jadwal periodik yang ketat.

#### 4.1.4 AlarmTask

AlarmTask adalah _task_ dengan prioritas tertinggi (5) dan periode terpendek (50 ms). _Task_ ini bertanggung jawab untuk:

1. Memeriksa sinyal dari _emergency button_ melalui _binary semaphore_ (`emergencySem`).
2. Mengevaluasi kondisi kritis berdasarkan nilai BPM, suhu, dan SpO2.
3. Mengendalikan LED alarm (GPIO25) dan _buzzer_ (GPIO26 via PWM).

```cpp
void AlarmTask(void *pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  TickType_t emergencyActiveUntil = 0;

  while (1) {
    // Periksa apakah emergency button ditekan (dari ISR)
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
```

Alarm akan aktif jika salah satu kondisi terpenuhi:
- BPM > 130
- Suhu > 38.0°C
- SpO2 < 90%
- Tombol darurat ditekan (berlaku 3 detik)
- Perintah `alarm_override` dari _dashboard_ bernilai "on"

#### 4.1.5 WiFiTask

WiFiTask menangani konektivitas dan komunikasi dengan _server_ Flask. _Task_ ini:

1. Memeriksa status koneksi Wi-Fi; jika terputus, melakukan reconnect.
2. Mengambil _snapshot_ data pasien (dilindungi `dataMutex`).
3. Mengirim data ke `POST /data` dalam format JSON menggunakan `HTTPClient`.

```cpp
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

      // Buat JSON dan kirim HTTP POST
      String jsonData = "{...}";
      HTTPClient http;
      http.begin(serverBaseUrl + "/data");
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.POST(jsonData);
      http.end();
    }

    vTaskDelayUntil(&lastWakeTime, WIFI_PERIOD_TICKS);
  }
}
```

Alokasi _stack_ WiFiTask (8192) lebih besar dari _task_ lain untuk mengakomodasi tumpukan panggilan HTTP dan JSON.

#### 4.1.6 CommandTask

CommandTask melakukan _polling_ ke server Flask setiap 750 ms untuk mendapatkan perintah kontrol terbaru melalui `GET /command`. _Task_ ini mendeserialisasi respons JSON menggunakan **ArduinoJson 7.4.3** dan memperbarui struktur `ControlCommand`.

```cpp
void CommandTask(void *pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();

  while (1) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverBaseUrl + "/command");
      int httpCode = http.GET();

      if (httpCode == HTTP_CODE_OK) {
        String response = http.getString();
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error && xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
          // Perbarui controlCommand dari JSON
          strlcpy(controlCommand.mode, doc["mode"] | "sensor", ...);
          controlCommand.bpm = doc["bpm"] | 90;
          // ... dan seterusnya

          // Jika mode "web", data pasien dari dashboard digunakan
          if (isWebControlMode()) {
            patient.bpm = controlCommand.bpm;
            patient.temp = controlCommand.temp;
            patient.spo2 = controlCommand.spo2;
            publishPatientSnapshot();
          }

          xSemaphoreGive(dataMutex);
        }
      }
      http.end();
    }

    vTaskDelayUntil(&lastWakeTime, COMMAND_PERIOD_TICKS);
  }
}
```

#### 4.1.7 MonitoringTask

MonitoringTask adalah _task_ prioritas terendah (1) yang bertugas menampilkan data monitor ke Serial Console setiap 500 ms. _Task_ ini juga mencatat _stack high-water mark_ setiap 10 siklus (setiap 5 detik) untuk membantu debugging penggunaan _stack_.

```cpp
void MonitoringTask(void *pvParameters) {
  TickType_t lastWakeTime = xTaskGetTickCount();
  PatientData receivedData;
  uint32_t stackLogCounter = 0;

  while (1) {
    if (xQueueReceive(patientQueue, &receivedData, 0) == pdTRUE) {
      Serial.println("=== SMART ICU MONITOR ===");
      Serial.print("BPM    : "); Serial.println(receivedData.bpm);
      Serial.print("TEMP   : "); Serial.println(receivedData.temp);
      Serial.print("SpO2   : "); Serial.println(receivedData.spo2);
      Serial.print("STATUS : "); Serial.println(receivedData.status);
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
```

### 4.2 Queue dan Semaphore

#### 4.2.1 Patient Queue (Queue)

`patientQueue` adalah _queue_ dengan ukuran 1 yang menampung struktur `PatientData`. _Queue_ ini berfungsi sebagai jalur komunikasi satu arah dari _task_ sensor ke MonitoringTask. Penggunaan `xQueueOverwrite()` memungkinkan data terbaru selalu tersedia bagi pembaca tanpa harus menunggu _queue_ kosong.

```cpp
patientQueue = xQueueCreate(1, sizeof(PatientData));
```

#### 4.2.2 Data Mutex (Mutex)

`dataMutex` adalah mutex yang melindungi akses ke struktur `PatientData` dan `ControlCommand`. Semua _task_ yang membaca atau menulis data pasien harus mengambil mutex ini terlebih dahulu. Mutex dipilih daripada _binary semaphore_ karena mutex mendukung **priority inheritance**, yang mencegah _priority inversion_ ketika _task_ prioritas tinggi mengakses data yang sedang dipegang oleh _task_ prioritas rendah.

```cpp
dataMutex = xSemaphoreCreateMutex();
```

#### 4.2.3 Emergency Semaphore (Binary Semaphore)

`emergencySem` adalah _binary semaphore_ yang digunakan untuk menunda pemrosesan _interrupt_ tombol darurat ke AlarmTask. Ketika tombol ditekan, ISR memberikan semaphore ini. AlarmTask kemudian mengambil semaphore tersebut dan mengaktifkan alarm selama 3 detik.

```cpp
emergencySem = xSemaphoreCreateBinary();
```

Pola ini dikenal sebagai **Deferred Interrupt Processing** — ISR hanya memberi sinyal, sedangkan pemrosesan dilakukan di _task_ biasa. Ini meminimalkan waktu yang dihabiskan di ISR dan menghindari pemblokiran ISR.

### 4.3 Penanganan ISR

_Tombol emergency_ dihubungkan ke GPIO27 dengan konfigurasi `INPUT_PULLUP` dan _interrupt_ pada tepi jatuh (_FALLING edge_). ISR ditandai dengan `IRAM_ATTR` agar kode ditempatkan di RAM instruksi untuk eksekusi yang cepat.

```cpp
void IRAM_ATTR emergencyISR() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(emergencySem, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
```

Poin penting dalam implementasi ISR:

- Fungsi ISR ditempatkan di IRAM (`IRAM_ATTR`) untuk menghindari _cache miss_ dan _flash latency_.
- `xSemaphoreGiveFromISR()` digunakan sebagai pengganti `xSemaphoreGive()` karena ISR tidak dapat memanggil fungsi yang memungkinkan _blocking_.
- Parameter `xHigherPriorityTaskWoken` memberi tahu scheduler apakah perlu melakukan _context switch_ setelah ISR selesai.
- `portYIELD_FROM_ISR()` melakukan _yield_ jika diperlukan.

### 4.4 Web Dashboard (Flask dan React)

#### 4.4.1 Flask Backend

Backend Flask menyediakan 5 _endpoint_ REST API:

**Tabel 4.1: Endpoint REST API**

| Method | Endpoint          | Fungsi                                        |
|--------|-------------------|-----------------------------------------------|
| POST   | `/data`           | Menerima data pasien dari ESP32               |
| GET    | `/latest`         | Mengembalikan data pasien & perintah terbaru  |
| POST   | `/control`        | Menerima perintah kontrol dari dashboard      |
| GET    | `/command`        | Mengembalikan perintah aktif untuk ESP32      |
| POST   | `/control/reset`  | Mengatur ulang ke mode normal                 |

Fitur keamanan pada backend:

- **Validasi input**: setiap payload diperiksa tipe data, rentang nilai, dan _field_ wajib.
- **Thread lock**: akses ke data bersama diamankan dengan `threading.Lock()`.
- **Sanitasi**: nilai BPM dibatasi 40–180, suhu 30–45°C, SpO2 70–100%.

```python
@app.route("/data", methods=["POST"])
def data():
    payload = request.get_json(force=True)
    sanitized, error = sanitize_patient_payload(payload)
    if error:
        return jsonify({"status": "error", **error}), 400

    with lock:
        latest_data = sanitized

    return jsonify({"status": "ok", "received": sanitized}), 200
```

#### 4.4.2 React Frontend

Frontend React dibangun dengan Vite 7.3.5 dan React 19, menggunakan **lucide-react** untuk ikon. Aplikasi terdiri dari:

1. **Header Band**: menampilkan judul sistem dan indikator status (NORMAL/CRITICAL).
2. **Vital Cards**: tiga kartu yang menampilkan BPM, Suhu, dan SpO2 dalam format besar.
3. **Control Panel**: _slider_ untuk mengatur nilai BPM (40–180), suhu (30–45°C), dan SpO2 (70–100%).
4. **Action Buttons**: tombol untuk menerapkan nilai, mengatur kondisi normal/kritis, darurat, reset, dan mode sensor.
5. **Telemetry Panel**: menampilkan status koneksi, mode, status darurat, alarm, sumber data, dan waktu pembaruan.

Frontend melakukan _polling_ ke `/latest` setiap 1 detik menggunakan `setInterval()` dalam _hook_ `useEffect`.

### 4.5 Simulasi Wokwi

Simulasi Wokwi dikonfigurasi melalui `diagram.json` dan `wokwi.toml`. Komponen yang digunakan:

- **board-esp32-devkit-c-v4**: mikrokontroler utama.
- **wokwi-potentiometer** (×3): sebagai sensor BPM, suhu, dan SpO2.
- **wokwi-led** (merah): sebagai alarm LED.
- **wokwi-buzzer**: sebagai alarm suara.
- **wokwi-pushbutton** (hijau): sebagai tombol darurat.
- **wokwi-breadboard-half**: tempat perakitan komponen.

Konfigurasi Wokwi:

```toml
# wokwi.toml
[wokwi]
version = 1
firmware = '.pio/build/esp32dev/firmware.bin'
elf = '.pio/build/esp32dev/firmware.elf'
```

### 4.6 Advanced RTOS Demo

Fitur _advanced RTOS demo_ diimplementasikan di dalam blok `#if ENABLE_ADVANCED_RTOS_DEMO` dan dinonaktifkan secara _default_ (`#define ENABLE_ADVANCED_RTOS_DEMO 0`).

#### 4.6.1 Deadlock Prevention

Dua _task_ (DeadlockTaskA dan DeadlockTaskB) mencoba mengambil dua mutex (`resource1` dan `resource2`) dengan urutan yang berlawanan — A mengambil resource1 dulu lalu resource2, B mengambil resource2 dulu lalu resource1. Ini adalah skenario deadlock klasik.

Pencegahan dilakukan dengan menggunakan **timeout** pada percobaan kedua:

```cpp
// DeadlockTaskA
xSemaphoreTake(resource1, portMAX_DELAY);  // Ambil resource1 (blocking)
vTaskDelay(pdMS_TO_TICKS(500));

if (xSemaphoreTake(resource2, pdMS_TO_TICKS(1000)) == pdTRUE) {
  // Berhasil mendapat kedua resource
  xSemaphoreGive(resource2);
} else {
  // Timeout! Deadlock tercegah, lepaskan resource1
  Serial.println("[DeadlockTaskA] DEADLOCK PREVENTED");
}
xSemaphoreGive(resource1);
```

Jika _task_ A gagal mendapatkan resource2 dalam 1000 ms, ia melepaskan resource1 dan mencoba lagi nanti, sehingga deadlock tidak terjadi.

#### 4.6.2 Priority Inversion Demo

Tiga _task_ dengan prioritas berbeda (rendah=1, sedang=3, tinggi=5) digunakan untuk mendemonstrasikan fenomena _priority inversion_:

- **LowPriorityTask**: mengambil `priorityMutex` dan menahannya selama 3 detik.
- **MediumPriorityTask**: berjalan tanpa mutex, hanya mencetak log.
- **HighPriorityTask**: mencoba mengambil `priorityMutex` (yang sedang dipegang _low priority task_).

Tanpa _priority inheritance_, _high priority task_ akan diblokir oleh _low priority task_ yang mungkin ditunda oleh _medium priority task_ — inilah _priority inversion_. Namun, karena FreeRTOS mutex mendukung **priority inheritance**, prioritas _low priority task_ akan dinaikkan sementara ke prioritas _high priority task_ saat memegang mutex, sehingga _medium priority task_ tidak dapat mendahuluinya.

#### 4.6.3 Protected Counter dengan Spinlock

Dua _task_ (CounterA dan CounterB) berbagi sebuah _counter_ integer (`sharedCounter`). Akses ke _counter_ diamankan dengan **spinlock** (_portMUX_TYPE_) — mekanisme sinkronisasi yang cocok untuk perlindungan data yang sangat pendek dan kritis:

```cpp
portENTER_CRITICAL(&spinlock);
sharedCounter++;
int snapshot = sharedCounter;
portEXIT_CRITICAL(&spinlock);
```

Spinlock digunakan karena operasi `sharedCounter++` sangat cepat (hanya beberapa instruksi). Berbeda dengan mutex, spinlock menonaktifkan _interrupt_ (atau melakukan _busy-wait_ pada sistem multi-core) sehingga tidak ada biaya _context switch_.

---

## BAB 5: HASIL PENGUJIAN

### 5.1 Hasil Kompilasi Firmware

Firmware ESP32 berhasil dikompilasi menggunakan PlatformIO dengan _framework_ Arduino. Berikut adalah hasil penggunaan memori:

**Tabel 5.1: Hasil Kompilasi Firmware**

| Parameter             | Nilai               | Persentase |
|-----------------------|---------------------|------------|
| RAM (DRAM) Terpakai   | 47,652 bytes        | 14.5%      |
| Total RAM Tersedia    | 327,680 bytes       | 100%       |
| Flash Terpakai        | 942,041 bytes       | 71.9%      |
| Total Flash Tersedia  | 1,310,720 bytes     | 100%       |

**Ringkasan Kompilasi:**

- **Platform**: espressif32@6.8.1
- **Board**: ESP32-DevKit-C-V4
- **Framework**: Arduino
- **Library**: ArduinoJson 7.4.3
- **Jumlah Task**: 7 FreeRTOS tasks (prioritas 1–5)
- **RAM**: 47,652 / 327,680 bytes (14.5%)
- **Flash**: 942,041 / 1,310,720 bytes (71.9%)

Penggunaan RAM sebesar 14.5% menunjukkan masih banyak ruang untuk pengembangan lebih lanjut. Penggunaan Flash sebesar 71.9% tergolong wajar mengingat firmware mencakup _library_ Wi-Fi, HTTP Client, dan ArduinoJson.

### 5.2 Pengujian API Endpoint

Seluruh _endpoint_ REST API telah diuji menggunakan `curl` dan berhasil berjalan dengan baik.

#### 5.2.1 POST /data — Mengirim Data Pasien

```bash
curl -X POST http://localhost:5000/data \
  -H "Content-Type: application/json" \
  -d '{"bpm": 132, "temp": 38.4, "spo2": 88, "status": "CRITICAL", "source": "esp32"}'
```

**Hasil** (HTTP 200):
```json
{
  "status": "ok",
  "received": {
    "bpm": 132,
    "temp": 38.4,
    "spo2": 88,
    "status": "CRITICAL",
    "source": "esp32",
    "updated_at": "2026-06-01T19:04:44.268902+00:00"
  }
}
```

#### 5.2.2 GET /latest — Mendapatkan Data Terbaru

```bash
curl http://localhost:5000/latest
```

**Hasil** (HTTP 200): Mengembalikan data pasien dan perintah aktif terkini.

#### 5.2.3 POST /control — Mengirim Perintah Kontrol

```bash
curl -X POST http://localhost:5000/control \
  -H "Content-Type: application/json" \
  -d '{"mode": "web", "bpm": 90, "temp": 36.8, "spo2": 97, "alarm_override": "auto"}'
```

**Hasil** (HTTP 200):
```json
{
  "status": "ok",
  "command": {
    "mode": "web",
    "bpm": 90,
    "temp": 36.8,
    "spo2": 97,
    "emergency": false,
    "alarm_override": "auto",
    "updated_at": "2026-06-01T19:04:44..."
  }
}
```

#### 5.2.4 GET /command — Mendapatkan Perintah Aktif

```bash
curl http://localhost:5000/command
```

**Hasil** (HTTP 200): Mengembalikan perintah kontrol yang sedang aktif untuk di-_poll_ oleh ESP32.

#### 5.2.5 POST /control/reset — Reset ke Mode Normal

```bash
curl -X POST http://localhost:5000/control/reset
```

**Hasil** (HTTP 200): Data diatur ulang ke nilai normal (BPM=90, suhu=36.8°C, SpO2=97%).

#### 5.2.6 Validasi Input — Data Invalid

```bash
curl -X POST http://localhost:5000/data \
  -H "Content-Type: application/json" \
  -d '{"bpm": "invalid"}'
```

**Hasil** (HTTP 400):
```json
{
  "status": "error",
  "message": "invalid field types"
}
```

Semua _endpoint_ mengembalikan kode status HTTP yang sesuai (200 untuk sukses, 400 untuk _error_ validasi), menunjukkan bahwa API berfungsi dengan baik.

### 5.3 Dashboard Web

Dashboard web berhasil dibangun dengan **Vite 7.3.5** dan **React 19**.

**Tabel 5.2: Informasi Build Frontend**

| Parameter          | Nilai                |
|--------------------|----------------------|
| Build Tool         | Vite 7.3.5           |
| Framework          | React 19.2.0         |
| Icons              | lucide-react 0.556.0 |
| Jumlah Modul       | 1,689 modul          |
| Paket npm          | 67 (0 vulnerabilities)|

Dashboard menampilkan:

1. **Indikator Status**: pill berwarna hijau (NORMAL) atau merah (CRITICAL) yang berubah secara _real-time_.
2. **Tiga Kartu Vital Sign**: menampilkan BPM, suhu (°C), dan SpO2 (%) dengan font besar dan ikon.
3. **Panel Kontrol**: _slider_ interaktif untuk mengatur BPM, suhu, dan SpO2, serta enam tombol aksi.
4. **Panel Telemetri**: informasi status koneksi, mode, darurat, alarm, sumber data, dan waktu pembaruan.

**Berikut adalah tampilan dashboard web:**

```
  ┌─────────────────────────────────────────────────────────┐
  │  ESP32 + FreeRTOS                         ┌───────────┐│
  │  Smart ICU Monitor                        │  NORMAL   ││
  │                                           └───────────┘│
  ├─────────────────┬─────────────────┬─────────────────────┤
  │  ♥ BPM          │  🌡 Temperature │  〰 SpO2            │
  │                 │                 │                     │
  │  90             │  36.8°C         │  97%                │
  ├─────────────────┴─────────────────┴─────────────────────┤
  │  ┌─ Control Panel ──────────────────────────────────┐   │
  │  │ BPM   ──●─────────────────────────── 90           │   │
  │  │ Temp  ──●────────────────────── 36.8°C           │   │
  │  │ SpO2  ────●───────────────────────── 97%         │   │
  │  │                                                    │   │
  │  │ [Apply Values] [Set Normal] [Set Critical]        │   │
  │  │ [Emergency]    [Reset Alarm]  [Sensor Mode]       │   │
  │  └────────────────────────────────────────────────────┘   │
  │  ┌─ Telemetry ───────────────────────────────────────┐   │
  │  │ Connection  : Online                               │   │
  │  │ Mode        : Web Control                          │   │
  │  │ Emergency   : Inactive                             │   │
  │  │ Alarm       : auto                                 │   │
  │  │ Source      : web-control                         │   │
  │  │ Last update : 6/1/2026, 7:04:44 PM                │   │
  │  │                                                    │   │
  │  │ Ready                                              │   │
  │  └────────────────────────────────────────────────────┘   │
  └─────────────────────────────────────────────────────────┘
```

### 5.4 Simulasi Wokwi

Simulasi Wokwi menunjukkan bahwa sistem berjalan sesuai yang diharapkan:

1. **Pembacaan Sensor**: Memutar potensiometer BPM, suhu, dan SpO2 mengubah nilai yang ditampilkan di Serial Monitor dan dashboard.
2. **Tombol Darurat**: Menekan tombol hijau menyebabkan LED merah menyala, _buzzer_ berbunyi, dan status berubah menjadi "CRITICAL" selama 3 detik.
3. **Kondisi Kritis**: Mengatur potensiometer ke nilai ekstrem (BPM > 130, suhu > 38°C, atau SpO2 < 90%) secara otomatis mengaktifkan alarm.
4. **Komunikasi HTTP**: ESP32 berhasil mengirim data ke server Flask dan menerima perintah kontrol.

---

## BAB 6: KESIMPULAN DAN SARAN

### 6.1 Kesimpulan

Berdasarkan hasil perancangan, implementasi, dan pengujian yang telah dilakukan, dapat ditarik kesimpulan sebagai berikut:

1. **Sistem pemantauan pasien ICU _real-time_** berhasil diimplementasikan menggunakan ESP32 dengan FreeRTOS. Sistem mampu membaca tiga parameter _vital sign_ (BPM, suhu, SpO2), memproses alarm secara _real-time_, dan menampilkan data melalui _web dashboard_.

2. **Manajemen multitasking FreeRTOS** berjalan dengan baik. Tujuh _task_ dengan prioritas 1–5 berjalan secara simultan tanpa saling mengganggu. Penggunaan `vTaskDelayUntil()` menjamin periodisitas eksekusi yang presisi untuk setiap _task_.

3. **Mekanisme sinkronisasi** (_queue_, _mutex_, _binary semaphore_, _spinlock_) berfungsi efektif dalam melindungi data bersama dan mengoordinasikan antar-task. Tidak ada _race condition_ atau _data corruption_ yang terdeteksi selama pengujian.

4. **Penanganan _interrupt_** menggunakan pola _deferred interrupt processing_ memungkinkan respons cepat terhadap tombol darurat tanpa mengorbankan stabilitas sistem. ISR hanya memberikan sinyal melalui _binary semaphore_, sedangkan pemrosesan dilakukan di AlarmTask.

5. **Web dashboard** berbasis Flask dan React berhasil menyediakan visualisasi data _real-time_ dengan _polling_ setiap 1 detik. API REST dengan validasi input dan _thread safety_ berfungsi dengan baik pada semua _endpoint_.

6. **Fitur RTOS tingkat lanjut** (_deadlock prevention_ menggunakan timeout, _priority inversion_ dengan _priority inheritance_, dan _protected counter_ dengan _spinlock_) berhasil diimplementasikan sebagai demo yang dapat diaktifkan sesuai kebutuhan.

7. **Efisiensi sumber daya**: Firmware hanya menggunakan 14.5% RAM dan 71.9% Flash, meninggalkan ruang yang cukup untuk pengembangan fitur tambahan.

### 6.2 Saran

Beberapa saran untuk pengembangan lebih lanjut:

1. **Sensor Medis Real**: Mengganti potensiometer simulasi dengan sensor medis sesungguhnya (seperti MAX30100 untuk BPM/SpO2 dan DS18B20 untuk suhu) untuk pengujian yang lebih realistis.

2. **Database**: Menambahkan database (seperti SQLite atau InfluxDB) untuk menyimpan rivayat data pasien yang dapat digunakan untuk analisis tren medis.

3. **Autentikasi dan Keamanan**: Menambahkan autentikasi pengguna, enkripsi HTTPS/TLS, dan mekanisme keamanan untuk melindungi data pasien yang sensitif.

4. **Notifikasi Real-Time**: Mengganti mekanisme _polling_ dengan WebSocket atau Server-Sent Events (SSE) untuk pembaruan _real-time_ yang lebih responsif.

5. **Multi-Patient Support**: Mengembangkan sistem untuk mendukung pemantauan beberapa pasien secara bersamaan.

6. **Protokol Komunikasi**: Mengganti protokol HTTP dengan MQTT (Message Queue Telemetry Transport) yang lebih efisien untuk IoT.

7. **Over-the-Air (OTA) Update**: Menambahkan kemampuan pembaruan firmware secara nirkabel untuk memudahkan pemeliharaan.

8. **Pengujian Beban**: Melakukan pengujian beban (_stress testing_) untuk memverifikasi perilaku sistem dalam kondisi ekstrem, seperti kehilangan koneksi Wi-Fi, lonjakan data, atau kegagalan sensor.

---

## DAFTAR PUSTAKA

1. Amazon Web Services. (2024). *FreeRTOS: A Real-Time Operating System for Microcontrollers*. https://www.freertos.org/

2. Barry, R. (2017). *Mastering the FreeRTOS Real Time Kernel: A Hands-On Tutorial Guide*. Real Time Engineers Ltd.

3. Espressif Systems. (2024). *ESP32 Technical Reference Manual*. https://www.espressif.com/en/support/documents/technical-documents/

4. Espressif Systems. (2024). *ESP-IDF Programming Guide: FreeRTOS Overview*. https://docs.espressif.com/projects/esp-idf/

5. Grinberg, M. (2018). *Flask Web Development: Developing Web Applications with Python* (2nd ed.). O'Reilly Media.

6. Liu, J. W. S. (2000). *Real-Time Systems*. Prentice Hall.

7. Meta Platforms. (2024). *React: A JavaScript Library for Building User Interfaces*. https://react.dev/

8. PlatformIO. (2024). *PlatformIO: A Professional Collaborative Platform for Embedded Development*. https://platformio.org/

9. Wokwi. (2024). *Wokwi: Online Electronics Simulator*. https://wokwi.com/

10. Arduino. (2024). *ArduinoJson: Efficient JSON Library for Arduino*. https://arduinojson.org/

11. Burns, A., & Wellings, A. (2009). *Real-Time Systems and Programming Languages: Ada, Real-Time Java and C/Real-Time POSIX* (4th ed.). Addison-Wesley.

12. Kopetz, H. (2011). *Real-Time Systems: Design Principles for Distributed Embedded Applications* (2nd ed.). Springer.

13. Tanenbaum, A. S., & Bos, H. (2015). *Modern Operating Systems* (4th ed.). Pearson.

14. Stallings, W. (2018). *Operating Systems: Internals and Design Principles* (9th ed.). Pearson.

15. Espressif Systems. (2024). *ESP32 Arduino Core Documentation*. https://docs.espressif.com/projects/arduino-esp32/

---
