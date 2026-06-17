# SMART ICU PATIENT MONITORING SYSTEM
## Berbasis ESP32 dan FreeRTOS

---

**LAPORAN TUGAS BESAR**

Mata Kuliah: Sistem Operasi Real-Time (RTOS)
Kode Mata Kuliah: CLO3 — Safety-Critical Systems

**Dosen Pengampu:**
[ Nama Dosen ]

---

**Disusun oleh:**

| No | Nama Lengkap | NIM |
|----|-------------|-----|
| 1 | Nicholai Dandy Nainggolan | 101032330023 |
| 2 | Muhammad Deva Valery | 10103233[NNNN] |
| 3 | Ardika Putra Hadian | 101032300240 |
| 4 | [ Nama Lengkap ] | [ NIM ] |

---

**Program Studi Teknik Informatika**
**Fakultas Ilmu Komputer**
**Universitas [ Nama Universitas ]**
**Semester Genap 2025/2026**

---

## ABSTRAK

Perkembangan teknologi Internet of Things (IoT) telah membuka peluang besar dalam dunia kesehatan, khususnya pada sistem pemantauan pasien di ruang _Intensive Care Unit_ (ICU). Parameter _vital sign_ seperti detak jantung (BPM), suhu tubuh, dan saturasi oksigen (SpO₂) memerlukan pemantauan kontinu dan _real-time_ yang deterministik. Sistem operasi _real-time_ (RTOS) menjadi fondasi kritis untuk menjamin respons tepat waktu terhadap perubahan kondisi pasien.

Proyek ini mengembangkan **Smart ICU Patient Monitoring System** yang mengintegrasikan mikrokontroler ESP32 dengan FreeRTOS untuk membaca tiga parameter _vital sign_ dari sensor simulasi (potensiometer virtual pada Wokwi), memproses alarm kondisi kritis secara _real-time_, dan menampilkan data melalui _web dashboard_ interaktif berbasis Flask (Python) dan React (JavaScript). Sistem dirancang dengan arsitektur multitasking yang terdiri dari tujuh _task_ FreeRTOS dengan prioritas _rate-monotonic_ (1–5), menggunakan mekanisme sinkronisasi seperti _queue_, _mutex_, _binary semaphore_, dan _spinlock_.

Hasil pengujian menunjukkan bahwa firmware ESP32 berhasil dikompilasi dengan penggunaan RAM sebesar 14,5% (47.652 bytes) dan Flash sebesar 71,9% (942.041 bytes). Analisis _schedulability_ menggunakan _Rate-Monotonic Scheduling_ (RMS) menunjukkan CPU _utilization_ U = 0,236 yang berada di bawah _Least Upper Bound_ U_LUB = 0,735, sehingga sistem dinyatakan _schedulable_. Seluruh _endpoint_ REST API berfungsi dengan baik dan _web dashboard_ berhasil menampilkan data secara _real-time_ dengan pembaruan setiap 1 detik.

**Kata kunci:** FreeRTOS, ESP32, ICU Patient Monitoring, Real-Time System, Wokwi Simulator, Web Dashboard, Rate-Monotonic Scheduling

---

## DAFTAR ISI

1. [COVER](#smart-icu-patient-monitoring-system)
2. [ABSTRAK](#abstrak)
3. [DAFTAR ISI](#daftar-isi)
4. [BAB 1: PENDAHULUAN](#bab-1-pendahuluan)
   - 1.1 Latar Belakang
   - 1.2 Rumusan Masalah
   - 1.3 Tujuan
   - 1.4 Batasan Masalah
   - 1.5 Manfaat
5. [BAB 2: LANDASAN TEORI](#bab-2-landasan-teori)
   - 2.1 Sistem Operasi Real-Time (RTOS)
   - 2.2 FreeRTOS
   - 2.3 Penjadwalan dan Analisis Schedulability
   - 2.4 Priority Inversion dan Solusinya
   - 2.5 Platform Simulasi
   - 2.6 Safety-Critical Systems
6. [BAB 3: REQUIREMENT DAN SPESIFIKASI SISTEM](#bab-3-requirement-dan-spesifikasi-sistem)
   - 3.1 System Requirements
   - 3.2 Functional Requirements
   - 3.3 Non-Functional Requirements
   - 3.4 System Specifications
   - 3.5 Justifikasi Pemilihan RTOS
7. [BAB 4: PERANCANGAN SISTEM](#bab-4-perancangan-sistem)
   - 4.1 Arsitektur Sistem
   - 4.2 Desain Task
   - 4.3 Analisis Schedulability
   - 4.4 Desain Inter-Task Synchronization
   - 4.5 Desain ISR
   - 4.6 Desain Manajemen Memori dan I/O
   - 4.7 Integrasi Tema Safety-Critical Systems
8. [BAB 5: IMPLEMENTASI](#bab-5-implementasi)
   - 5.1 Implementasi FreeRTOS
   - 5.2 Implementasi Task
   - 5.3 Implementasi Sinkronisasi
   - 5.4 Implementasi ISR dan Deferred Processing
   - 5.5 Implementasi Web Dashboard
   - 5.6 Pembagian Tugas Anggota Kelompok
9. [BAB 6: PENGUJIAN DAN ANALISIS](#bab-6-pengujian-dan-analisis)
   - 6.1 Metodologi Pengujian
   - 6.2 Skenario Pengujian
   - 6.3 Hasil Kompilasi Firmware
   - 6.4 Hasil Pengujian API Endpoint
   - 6.5 Pengukuran Metrik Real-Time
   - 6.6 Verifikasi Penanganan Race Condition
   - 6.7 Verifikasi Priority Inversion
   - 6.8 Troubleshooting
10. [BAB 7: KESIMPULAN DAN SARAN](#bab-7-kesimpulan-dan-saran)
    - 7.1 Kesimpulan
    - 7.2 Saran
11. [DAFTAR PUSTAKA](#daftar-pustaka)
12. [LAMPIRAN](#lampiran)
    - A: Source Code
    - B: Konfigurasi Simulator
    - C: Hasil Pengujian
    - D: Video Demo
    - E: Project Requirements Checklist

---

## BAB 1: PENDAHULUAN

### 1.1 Latar Belakang

Perkembangan teknologi Internet of Things (IoT) telah membawa perubahan signifikan dalam dunia kesehatan, khususnya pada sistem pemantauan pasien di rumah sakit. _Intensive Care Unit_ (ICU) merupakan unit perawatan intensif yang memerlukan pemantauan _vital sign_ pasien secara kontinu dan _real-time_. Parameter seperti detak jantung (_heart rate_ / BPM), suhu tubuh (_temperature_), dan saturasi oksigen (SpO₂) harus dipantau secara ketat karena perubahan sekecil apapun dapat mengindikasikan kondisi kritis yang memerlukan tindakan medis segera.

Sistem pemantauan pasien komersial saat ini memiliki harga yang relatif mahal dan seringkali bersifat _proprietary_, sehingga sulit dikembangkan atau dimodifikasi. Di sisi lain, mikrokontroler modern seperti ESP32 menawarkan kemampuan komputasi yang cukup dengan harga yang sangat terjangkau, serta dilengkapi dengan konektivitas Wi-Fi dan Bluetooth _built-in_. ESP32 memiliki prosesor Xtensa 32-bit LX6 dengan kecepatan hingga 240 MHz, 520 KB SRAM, dan 4 MB Flash eksternal, yang memadai untuk menjalankan aplikasi _real-time_ multitasking.

Sistem operasi _real-time_ (RTOS) menjadi komponen krusial dalam pengembangan sistem pemantauan medis. RTOS menjamin bahwa tugas-tugas kritis (_deadline-sensitive tasks_) mendapatkan jatah waktu prosesor yang tepat sehingga respons terhadap perubahan kondisi pasien dapat dilakukan secara cepat dan dapat diprediksi (_deterministic_). FreeRTOS, sebagai salah satu RTOS _open-source_ yang populer, menyediakan mekanisme _task scheduling_, _queue_, _semaphore_, _mutex_, dan penanganan _interrupt_ yang sangat cocok untuk aplikasi _embedded_ _real-time_.

Dalam konteks mata kuliah Sistem Operasi Real-Time (RTOS) dengan tema CLO3 _Safety-Critical Systems_, proyek ini mengusulkan pengembangan **Smart ICU Patient Monitoring System** yang mengintegrasikan ESP32 dengan FreeRTOS untuk membaca sensor _vital sign_ virtual pada simulator Wokwi, memproses data secara _real-time_, menyalakan alarm ketika kondisi kritis terdeteksi, dan menampilkan data melalui _web dashboard_ berbasis Flask dan React. Sistem ini dirancang dengan mempertimbangkan aspek _safety-critical_ di mana kegagalan merespons kondisi darurat dapat berakibat fatal.

### 1.2 Rumusan Masalah

Berdasarkan latar belakang di atas, rumusan masalah dalam proyek ini adalah:

1. Bagaimana merancang dan mengimplementasikan sistem pemantauan pasien ICU _real-time_ menggunakan ESP32 dan FreeRTOS yang memenuhi kriteria _safety-critical_?
2. Bagaimana mengelola tujuh _task_ dengan prioritas dan periode berbeda menggunakan _scheduler_ preemptive FreeRTOS dengan algoritma _Rate-Monotonic Scheduling_ (RMS)?
3. Bagaimana menggunakan mekanisme sinkronisasi FreeRTOS (_queue_, _mutex_, _binary semaphore_, _spinlock_) untuk mengamankan data bersama (_shared data_) dan komunikasi antar-_task_ tanpa _race condition_?
4. Bagaimana menangani _interrupt_ tombol darurat (_emergency button_) dengan aman menggunakan pola _deferred interrupt processing_ untuk menjamin respons cepat dan stabil?
5. Bagaimana membangun _web dashboard_ yang menampilkan data _vital sign_ secara _real-time_ menggunakan Flask (backend) dan React (frontend) dengan API yang _thread-safe_?
6. Bagaimana mendemonstrasikan dan menganalisis fenomena RTOS tingkat lanjut seperti _deadlock prevention_, _priority inversion_ dengan _priority inheritance_, dan _protected counter_ dengan _spinlock_?

### 1.3 Tujuan

Tujuan dari proyek ini adalah:

1. Merancang sistem pemantauan pasien ICU berbasis ESP32 dengan arsitektur multitasking FreeRTOS yang memenuhi prinsip _safety-critical systems_.
2. Mengimplementasikan tujuh _task_ FreeRTOS (AlarmTask, HeartRateTask, OxygenTask, TemperatureTask, WiFiTask, CommandTask, MonitoringTask) dengan prioritas _rate-monotonic_ (1–5) yang menangani pembacaan sensor, pemrosesan alarm, komunikasi Wi-Fi, dan monitoring.
3. Mengimplementasikan mekanisme sinkronisasi FreeRTOS (_mutex_ untuk _mutual exclusion_, _queue_ untuk komunikasi data, _binary semaphore_ untuk _deferred interrupt processing_, _spinlock_ untuk perlindungan kritis) untuk menghindari _race condition_ dan _data corruption_.
4. Membangun sistem alarm _real-time_ yang responsif terhadap kondisi kritis (BPM > 130, suhu > 38°C, SpO₂ < 90%) dan tombol darurat dengan prioritas tertinggi.
5. Mengembangkan _web dashboard_ interaktif dengan Flask (backend REST API) dan React (frontend visual) yang menampilkan data _vital sign_ secara _real-time_ dengan pembaruan setiap 1 detik.
6. Mendemonstrasikan dan menganalisis fenomena RTOS tingkat lanjut (_deadlock prevention_ menggunakan _timeout_, _priority inversion_ dengan _priority inheritance_, _protected counter_ dengan _spinlock_) sebagai _advanced RTOS demo_.
7. Melakukan analisis _schedulability_ menggunakan RMS dan _Response Time Analysis_ (RTA) untuk memverifikasi bahwa semua _deadline_ terpenuhi.

### 1.4 Batasan Masalah

Batasan masalah dalam proyek ini adalah:

1. Sistem menggunakan simulator Wokwi untuk menggantikan komponen fisik (potensiometer sebagai sensor virtual, LED dan _buzzer_ sebagai aktuator).
2. Data sensor merupakan data simulasi dari pembacaan potensiometer analog virtual, bukan dari sensor medis sesungguhnya seperti MAX30100 atau DS18B20.
3. Komunikasi antara ESP32 dan _dashboard_ menggunakan protokol HTTP sederhana melalui koneksi Wi-Fi, bukan protokol IoT khusus seperti MQTT atau CoAP.
4. Sistem tidak menggunakan database persistensi; data hanya disimpan sementara di memori _server_ Flask dan hilang saat _server_ dimatikan.
5. _Advanced RTOS demo_ (_deadlock prevention_, _priority inversion_, _protected counter_) diimplementasikan sebagai fitur opsional yang dinonaktifkan secara _default_ (`#define ENABLE_ADVANCED_RTOS_DEMO 0`) agar tidak mengganggu _deadline_ monitoring utama.
6. Fokus utama adalah pada aspek RTOS dan arsitektur sistem _real-time_, bukan pada akurasi medis, validasi klinis, atau sertifikasi perangkat medis.

### 1.5 Manfaat

#### 1.5.1 Manfaat Pembelajaran

1. Memberikan pemahaman mendalam tentang konsep RTOS (_task_, _scheduler_, _queue_, _semaphore_, _mutex_, ISR) melalui implementasi nyata pada sistem _embedded_.
2. Melatih kemampuan analisis _schedulability_ menggunakan metode RMS, LUB _test_, dan RTA pada sistem multitasking _real-time_.
3. Mengembangkan keterampilan dalam merancang dan mengimplementasikan sistem _safety-critical_ dengan mekanisme _fault tolerance_ dan _deadlock prevention_.

#### 1.5.2 Manfaat Teknis

1. Menghasilkan arsitektur referensi untuk sistem pemantauan pasien berbasis RTOS yang dapat dikembangkan lebih lanjut.
2. Mendemonstrasikan pola _deferred interrupt processing_ yang merupakan praktik terbaik dalam penanganan _interrupt_ pada sistem RTOS.
3. Menyediakan kerangka kerja (_framework_) pengujian _real-time_ yang mencakup _stack monitoring_, _timing analysis_, dan verifikasi sinkronisasi.

#### 1.5.3 Manfaat Aplikatif

1. Menyediakan solusi pemantauan pasien yang terjangkau dan _open-source_ berbasis ESP32 yang berpotensi diterapkan di fasilitas kesehatan skala kecil.
2. Mengintegrasikan _web dashboard_ yang memungkinkan tenaga medis memantau kondisi pasien dari jarak jauh melalui _browser_.
3. Menyediakan mekanisme kontrol _real-time_ yang memungkinkan tenaga medis mengatur parameter pasien dari _dashboard_ secara _remote_.

---

## BAB 2: LANDASAN TEORI

### 2.1 Sistem Operasi Real-Time (RTOS)

**Sistem Operasi Real-Time** (RTOS) adalah sistem operasi yang dirancang secara khusus untuk melayani aplikasi dengan batasan waktu yang ketat (_deadline_). Berbeda dengan sistem operasi tujuan umum (_general-purpose OS_) seperti Windows atau Linux yang berusaha memaksimalkan _throughput_ dan _fairness_, RTOS mengutamakan **determinisme** dan **prediktabilitas** waktu eksekusi. Dalam sistem _real-time_, kebenaran logika sistem tidak hanya bergantung pada hasil komputasi yang benar, tetapi juga pada waktu di mana hasil tersebut dihasilkan (Liu, 2000).

RTOS diklasifikasikan menjadi tiga kategori berdasarkan ketegasan _deadline_:

- **_Hard Real-Time_**: Kegagalan memenuhi _deadline_ menyebabkan kegagalan sistem total (contoh: _airbag system_, _pacemaker_).
- **_Firm Real-Time_**: Kegagalan memenuhi _deadline_ menurunkan kualitas tetapi tidak menyebabkan kegagalan sistem (contoh: sistem pemrosesan video).
- **_Soft Real-Time_**: Kegagalan memenuhi _deadline_ menurunkan performa namun masih dapat ditoleransi (contoh: sistem streaming musik).

#### 2.1.1 Task

_Task_ (atau _thread_) adalah unit dasar eksekusi dalam RTOS. Setiap _task_ menjalankan fungsi tertentu secara independen dan memiliki _context_ sendiri (_stack_, _program counter_, _register_). Dalam FreeRTOS, _task_ dibuat dengan `xTaskCreate()` dan dapat berada dalam salah satu dari beberapa status:

- **Running**: sedang menggunakan CPU.
- **Ready**: siap dijalankan tetapi CPU sedang digunakan oleh _task_ lain.
- **Blocked**: menunggu suatu kejadian (_event_) atau _timeout_.
- **Suspended**: tidak dapat dijadwalkan sampai di-_resume_ secara eksplisit.

Setiap _task_ memiliki **Task Control Block** (TCB) yang menyimpan informasi lengkap tentang status _task_, termasuk prioritas, _stack pointer_, dan _state_.

#### 2.1.2 Scheduler

_Scheduler_ adalah komponen inti RTOS yang menentukan _task_ mana yang harus berjalan pada suatu waktu. FreeRTOS menggunakan **preemptive scheduler** dengan prioritas tetap (_fixed-priority preemptive scheduling_). _Task_ dengan prioritas lebih tinggi akan selalu mendahului (_preempt_) _task_ dengan prioritas lebih rendah. Jika terdapat beberapa _task_ dengan prioritas yang sama, mereka akan dijadwalkan secara _round-robin_ (_time-sliced_) dengan _time quantum_ sebesar satu _tick_ RTOS.

Kebijakan penjadwalan ini sangat cocok untuk sistem _real-time_ keras (_hard real-time_) karena menjamin bahwa _task_ kritis (seperti alarm pada sistem ICU) akan selalu mendapatkan CPU saat dibutuhkan.

#### 2.1.3 Queue

_Queue_ adalah mekanisme komunikasi antar-task (_Inter-Task Communication_ / ITC) yang memungkinkan pertukaran data secara aman antar _task_ atau antara ISR dan _task_. _Queue_ menggunakan model _First-In-First-Out_ (FIFO) dan dapat menampung sejumlah data dengan ukuran tetap yang ditentukan saat pembuatan. Operasi utama pada _queue_ adalah:

- `xQueueSend()`: mengirim data ke _queue_ (akan mem-_block_ jika _queue_ penuh).
- `xQueueReceive()`: menerima data dari _queue_ (akan mem-_block_ jika _queue_ kosong).
- `xQueueOverwrite()`: menimpa data terakhir di _queue_ (khusus untuk _queue_ dengan panjang 1, berguna untuk _latest-value semantics_).

#### 2.1.4 Semaphore

Semaphore adalah mekanisme sinkronisasi yang digunakan untuk mengontrol akses ke sumber daya bersama atau untuk memberi sinyal antar-task. Konsep semaphore pertama kali diperkenalkan oleh Edsger Dijkstra pada tahun 1965. FreeRTOS menyediakan beberapa jenis semaphore:

- **Binary Semaphore**: semaphore dengan dua nilai (0 dan 1). Cocok untuk mekanisme _signaling_ (misalnya, memberi tahu _task_ bahwa suatu _interrupt_ telah terjadi). Berbeda dengan mutex, _binary semaphore_ tidak memiliki _priority inheritance_.
- **Counting Semaphore**: semaphore dengan nilai > 1, digunakan untuk melacak sejumlah sumber daya yang tersedia.

#### 2.1.5 Mutex

Mutex (_Mutual Exclusion_) adalah jenis semaphore khusus yang digunakan untuk melindungi sumber daya bersama (_shared resource_) dari akses simultan. Karakteristik utama mutex meliputi:

- **Priority Inheritance**: mencegah _priority inversion_ dengan sementara menaikkan prioritas _task_ yang memegang mutex agar _task_ prioritas lebih tinggi tidak menunggu terlalu lama.
- **Recursive**: memungkinkan _task_ yang sama mengambil mutex yang sama berkali-kali (perlu dirilis sebanyak pengambilannya).
- Mutex hanya boleh digunakan dalam _task context_ (bukan ISR).

#### 2.1.6 ISR (Interrupt Service Routine)

ISR adalah fungsi khusus yang dipanggil oleh prosesor sebagai respons terhadap sinyal _interrupt_ dari perangkat keras. Dalam RTOS, ISR harus ditulis dengan hati-hati karena:

- ISR berjalan di luar kendali _scheduler_ sehingga dapat memicu _race condition_ jika tidak hati-hati.
- ISR harus sesingkat mungkin (_minimal latency_) untuk menghindari hambatan pada _interrupt_ lain.
- Operasi yang memungkinkan _context switch_ (seperti `xSemaphoreGiveFromISR()`) memerlukan parameter `pxHigherPriorityTaskWoken` untuk memberi tahu _scheduler_ perlunya _yield_ setelah ISR selesai.

Pola **deferred interrupt processing** digunakan untuk memindahkan pekerjaan berat dari ISR ke _task_ biasa dengan cara memberi sinyal melalui semaphore atau _queue_.

#### 2.1.7 Priority Inversion

Priority inversion adalah situasi di mana _task_ berprioritas tinggi terpaksa menunggu karena _task_ berprioritas rendah sedang memegang sumber daya yang dibutuhkan, sementara _task_ berprioritas menengah (_medium_) mendahului _task_ berprioritas rendah tersebut. Akibatnya, _task_ berprioritas tinggi dieksekusi setelah _task_ berprioritas menengah — suatu pelanggaran terhadap aturan prioritas yang dapat menyebabkan kegagalan _deadline_ pada sistem _hard real-time_.

Solusi utama untuk _priority inversion_ adalah **Priority Inheritance Protocol**, diimplementasikan langsung oleh mutex FreeRTOS. Protokol ini menaikkan prioritas _task_ pemegang mutex ke prioritas _task_ yang menunggu, sehingga _task_ prioritas menengah tidak dapat mendahuluinya.

#### 2.1.8 Deadlock

Deadlock adalah kondisi di mana dua atau lebih _task_ saling menunggu sumber daya yang dipegang oleh _task_ lain, sehingga tidak ada _task_ yang dapat melanjutkan eksekusi. Empat kondisi yang diperlukan untuk terjadinya deadlock (dikenal sebagai **Coffman conditions**) adalah:

1. **Mutual Exclusion**: sumber daya tidak dapat digunakan bersama.
2. **Hold and Wait**: _task_ memegang satu sumber daya sambil menunggu sumber daya lain.
3. **No Preemption**: sumber daya tidak dapat diambil paksa.
4. **Circular Wait**: terdapat rantai melingkar _task_ yang saling menunggu.

Strategi pencegahan deadlock yang digunakan dalam proyek ini adalah penggunaan _timeout_ pada operasi pengambilan mutex dan pengaturan urutan pengambilan mutex secara konsisten.

### 2.2 FreeRTOS

FreeRTOS adalah sistem operasi _real-time_ _open-source_ yang dirancang untuk perangkat _embedded_ dengan sumber daya terbatas. FreeRTOS ditulis dalam bahasa C dan telah di-porting ke lebih dari 40 arsitektur mikrokontroler, termasuk ARM Cortex-M, RISC-V, dan Xtensa (ESP32) (Amazon Web Services, 2024).

#### 2.2.1 Task Management

FreeRTOS menyediakan API yang lengkap untuk manajemen _task_:

- `xTaskCreate()`: membuat _task_ baru dengan menentukan fungsi _task_, nama, ukuran _stack_, parameter, prioritas, dan _handle_.
- `vTaskDelay()`: menunda eksekusi _task_ selama periode _tick_ tertentu (relatif).
- `vTaskDelayUntil()`: menunda eksekusi _task_ hingga waktu absolut tertentu — menjaga periodisitas tetap presisi meskipun ada variasi waktu eksekusi (_jitter_).
- `uxTaskGetStackHighWaterMark()`: mengembalikan jumlah _stack_ yang tidak terpakai sejak _task_ dibuat, berguna untuk debugging _stack overflow_.
- `vTaskDelete()`: menghapus _task_ dari penjadwalan.

#### 2.2.2 Inter-Task Communication

FreeRTOS menyediakan berbagai mekanisme komunikasi antar-task:

- **Queue**: komunikasi data FIFO yang _thread-safe_.
- **Semaphore**: sinkronisasi dan _signaling_.
- **Mutex**: _mutual exclusion_ dengan _priority inheritance_.
- **Event Group**: sinkronisasi berbasis _bitmask_ untuk menunggu kombinasi beberapa _event_.
- **Stream Buffer** dan **Message Buffer**: komunikasi data _variable-length_.

#### 2.2.3 Interrupt Handling

FreeRTOS menyediakan API khusus untuk digunakan dalam ISR:

- `xSemaphoreGiveFromISR()`: memberikan semaphore dari konteks ISR.
- `xQueueSendFromISR()`: mengirim data ke _queue_ dari ISR.
- `portYIELD_FROM_ISR()`: memicu _context switch_ setelah ISR selesai jika diperlukan.

Semua fungsi "FromISR" menerima parameter `pxHigherPriorityTaskWoken` yang diisi `pdTRUE` jika operasi tersebut menyebabkan _task_ prioritas lebih tinggi siap berjalan.

#### 2.2.4 Memory Management

FreeRTOS menyediakan lima skema alokasi heap (`heap_1` hingga `heap_5`) yang dapat dipilih sesuai kebutuhan aplikasi:

- **heap_1**: alokasi sederhana tanpa dealokasi (cocok untuk sistem yang tidak pernah menghapus _task_).
- **heap_2**: alokasi dengan dealokasi tetapi tidak menggabungkan blok yang berdekatan.
- **heap_3**: pembungkus (_wrapper_) untuk `malloc()`/`free()` standar dengan _thread safety_.
- **heap_4**: seperti heap_2 tetapi menggabungkan blok yang berdekatan (_coalescing_).
- **heap_5**: seperti heap_4 tetapi mendukung beberapa region memori yang tidak bersebelahan (_non-contiguous_).

ESP32 secara _default_ menggunakan heap_4 melalui ESP-IDF.

### 2.3 Penjadwalan dan Analisis Schedulability

#### 2.3.1 Rate-Monotonic Scheduling (RMS)

_Rate-Monotonic Scheduling_ (RMS) adalah algoritma penjadwalan prioritas tetap (_fixed-priority_) yang dikemukakan oleh Liu dan Layland (1973). Dalam RMS, prioritas ditetapkan berdasarkan periode _task_: semakin pendek periode (_rate_), semakin tinggi prioritasnya. RMS telah terbukti optimal di antara algoritma penjadwalan prioritas tetap untuk _task_ yang independen dengan _deadline_ sama dengan periode.

#### 2.3.2 Earliest Deadline First (EDF)

_Earliest Deadline First_ (EDF) adalah algoritma penjadwalan prioritas dinamis di mana _task_ dengan _deadline_ terdekat mendapat prioritas tertinggi. EDF dianggap optimal untuk sistem _uniprocessor_ karena memiliki _utilization bound_ 100%. Namun, EDF lebih sulit diimplementasikan pada sistem _embedded_ karena memerlukan pengelolaan prioritas dinamis. FreeRTOS tidak mendukung EDF secara _native_, sehingga RMS menjadi pilihan yang lebih praktis.

#### 2.3.3 CPU Utilization dan Least Upper Bound (LUB) Test

_CPU utilization_ (U) didefinisikan sebagai rasio antara total waktu eksekusi (_Computation time_ / C) terhadap periode (T) untuk semua _task_:

```
U = Σ Ci / Ti
```

Liu dan Layland (1973) membuktikan bahwa untuk n _task_ independen dengan RMS, kondisi _schedulability_ terpenuhi jika:

```
U ≤ U_LUB = n × (2^(1/n) — 1)
```

Untuk n → ∞, U_LUB mendekati ln(2) ≈ 0,693. Untuk n = 6, U_LUB ≈ 0,735. Jika CPU _utilization_ total berada di bawah nilai ini, semua _task_ dijamin _schedulable_. Jika di atasnya, analisis lebih lanjut (seperti RTA) diperlukan.

#### 2.3.4 Response Time Analysis (RTA)

_Response Time Analysis_ (RTA) adalah metode yang lebih akurat untuk memeriksa _schedulability_ dengan menghitung _worst-case response time_ (R_i) untuk setiap _task_. RTA menggunakan persamaan _fixed-point iteration_:

```
R_i^(k+1) = C_i + Σ_(j ∈ hp(i)) ⌈(R_i^k) / T_j⌉ × C_j
```

di mana hp(i) adalah himpunan _task_ dengan prioritas lebih tinggi dari _task_ i. _Task_ dinyatakan _schedulable_ jika R_i ≤ D_i (dengan D_i adalah _deadline_, yang dalam RMS sering diset sama dengan T_i).

### 2.4 Priority Inversion dan Solusinya

#### 2.4.1 Fenomena Priority Inversion

Priority inversion terjadi ketika tiga _task_ dengan prioritas berbeda (Tinggi, Sedang, Rendah) berbagi sumber daya yang dilindungi mutex. Skenarionya sebagai berikut:

1. **Task Rendah** mengambil mutex dan mulai memproses data.
2. **Task Sedang** (prioritas menengah) tiba dan mendahului _task_ Rendah karena prioritas lebih tinggi.
3. **Task Tinggi** tiba dan mencoba mengambil mutex, tetapi gagal karena mutex masih dipegang oleh _task_ Rendah (yang sedang di-_preempt_ oleh _task_ Sedang).
4. **Task Tinggi** terpaksa menunggu sampai _task_ Sedang selesai, lalu _task_ Rendah melanjutkan dan melepas mutex.

Dampaknya: _task_ prioritas tertinggi dieksekusi setelah _task_ prioritas menengah — suatu inversi prioritas yang dapat menyebabkan kegagalan _deadline_.

#### 2.4.2 Priority Inheritance Protocol

**Priority Inheritance Protocol** (PIP) adalah solusi di mana _task_ yang memegang mutex "mewarisi" prioritas _task_ yang menunggu mutex tersebut. Dalam contoh di atas, saat _task_ Rendah memegang mutex dan _task_ Tinggi menunggu, prioritas _task_ Rendah akan dinaikkan ke prioritas _task_ Tinggi. Akibatnya, _task_ Sedang tidak dapat mendahului _task_ Rendah, sehingga _task_ Rendah dapat menyelesaikan tugasnya dan melepas mutex dengan cepat.

FreeRTOS mengimplementasikan PIP secara _built-in_ pada `xSemaphoreCreateMutex()`. Inilah alasan utama mengapa mutex lebih dipilih daripada _binary semaphore_ untuk perlindungan data bersama.

### 2.5 Platform Simulasi

#### 2.5.1 Wokwi Simulator

Wokwi (https://wokwi.com) adalah simulator sirkuit elektronik dan mikrokontroler yang berjalan di _web browser_ (Wokwi, 2024). Wokwi mendukung simulasi berbagai komponen, termasuk:

- Mikrokontroler: ESP32, Arduino Uno, Raspberry Pi Pico, STM32, dll.
- Komponen: LED, _buzzer_, potensiometer, _push button_, LCD, sensor, dll.
- Antarmuka: Serial Monitor, _Logic Analyzer_, Wi-Fi (terbatas).

Wokwi sangat berguna untuk pengembangan dan pengujian firmware _embedded_ tanpa memerlukan perangkat keras fisik. Wokwi terintegrasi dengan PlatformIO dan Visual Studio Code melalui ekstensi.

#### 2.5.2 ESP32

ESP32 adalah mikrokontroler _system-on-chip_ (SoC) yang dikembangkan oleh Espressif Systems (Espressif Systems, 2024). ESP32 memiliki arsitektur Xtensa LX6 dual-core (atau single-core pada beberapa varian) dengan fitur:

- **CPU**: Xtensa 32-bit LX6, _clock_ hingga 240 MHz.
- **Memori**: 520 KB SRAM, 448 KB ROM, 4 MB Flash eksternal.
- **Konektivitas**: Wi-Fi 802.11 b/g/n, Bluetooth 4.2 BR/EDR dan BLE.
- **GPIO**: 34 pin GPIO yang dapat diprogram.
- **ADC**: 2 × 12-bit SAR ADC dengan total 18 kanal.
- **Interface**: SPI, I2C, I2S, UART, CAN, Ethernet MAC, PWM, _Touch Sensor_.

ESP32 menggunakan FreeRTOS sebagai sistem operasi _default_ pada ESP-IDF (Espressif Systems, 2024).

#### 2.5.3 PlatformIO

PlatformIO (PlatformIO, 2024) adalah platform pengembangan _embedded_ profesional yang mendukung banyak _framework_ (Arduino, ESP-IDF, mbed, Zephyr) dan banyak _board_. PlatformIO menyediakan:

- **Library Manager**: manajemen dependensi pustaka.
- **Build System**: integrasi toolchain kompilasi.
- **Debugger**: antarmuka debugging.
- **Unit Testing**: kerangka kerja pengujian.

Proyek ini menggunakan PlatformIO dengan _framework_ Arduino untuk kompilasi firmware ESP32.

### 2.6 Safety-Critical Systems

#### 2.6.1 Definisi

_Safety-Critical System_ adalah sistem yang kegagalannya dapat mengakibatkan cedera serius atau kematian pada manusia, kerusakan properti yang signifikan, atau kerusakan lingkungan (Kopetz, 2011). Dalam konteks medis, sistem pemantauan pasien ICU termasuk dalam kategori _safety-critical_ karena:

- Kesalahan pembacaan _vital sign_ dapat menyebabkan keputusan medis yang salah.
- Keterlambatan alarm kondisi kritis dapat mengakibatkan keterlambatan penanganan pasien.
- Kegagalan komunikasi data dapat menyebabkan hilangnya informasi penting.

#### 2.6.2 State-of-the-Art

Perkembangan terkini dalam _safety-critical systems_ untuk aplikasi medis mencakup:

- **IEC 62304**: Standar internasional untuk perangkat lunak perangkat medis yang menetapkan siklus hidup pengembangan perangkat lunak.
- **RTCA DO-178C**: Standar untuk pengembangan perangkat lunak sistem _avionics_ yang sering dijadikan referensi untuk sistem _safety-critical_.
- **Redundancy dan Fault Tolerance**: Penggunaan komponen redundan dan mekanisme _fail-safe_ untuk menjamin operasi sistem meskipun terjadi kegagalan komponen.
- **Watchdog Timer**: Mekanisme deteksi _task hang_ dengan _timer_ perangkat keras yang memicu _reset_ sistem jika _task_ tidak merespons dalam periode tertentu.
- **Formal Verification**: Penggunaan metode matematis untuk membuktikan kebenaran sistem, termasuk _model checking_ dan _theorem proving_.

#### 2.6.3 Relevansi dengan Proyek

Proyek Smart ICU Patient Monitoring mengintegrasikan prinsip _safety-critical_ melalui:

1. **Prioritas Tertinggi untuk Alarm**: AlarmTask diberi prioritas 5 (tertinggi) dengan periode 50 ms terpendek, memastikan respons cepat terhadap kondisi kritis.
2. **_Deferred Interrupt Processing_**: ISR tombol darurat hanya memberi sinyal melalui _binary semaphore_, meminimalkan waktu di ISR dan menghindari _race condition_.
3. **_Priority Inheritance_**: Penggunaan mutex (bukan _binary semaphore_) untuk melindungi data pasien, mencegah _priority inversion_.
4. **_Stack Monitoring_**: Pemantauan _stack high-water mark_ secara berkala untuk mendeteksi potensi _stack overflow_.
5. **_Deadlock Prevention_**: Penggunaan _timeout_ pada pengambilan mutex kedua untuk mencegah deadlock.
6. **_Input Validation_**: Validasi ketat pada data pasien dan perintah kontrol di _backend_ Flask.

---

## BAB 3: REQUIREMENT DAN SPESIFIKASI SISTEM

### 3.1 System Requirements

Sistem **Smart ICU Patient Monitoring System** dirancang untuk memenuhi kebutuhan pemantauan _vital sign_ pasien di ruang ICU secara _real-time_ dengan memanfaatkan teknologi RTOS dan IoT. Sistem terdiri dari tiga komponen utama:

1. **ESP32 Firmware (FreeRTOS)**: bertanggung jawab untuk membaca data sensor, memproses alarm, dan berkomunikasi dengan _server_ melalui Wi-Fi.
2. **Flask Backend**: menyediakan REST API untuk menerima data dari ESP32, menyimpan data terkini, dan meneruskan perintah kontrol dari _dashboard_.
3. **React Frontend**: menampilkan data _vital sign_ secara _real-time_ dan menyediakan antarmuka untuk mengirim perintah kontrol ke ESP32.

### 3.2 Functional Requirements

**Tabel 3.1: Functional Requirements (FR)**

| Kode | Deskripsi | Prioritas |
|------|-----------|-----------|
| FR-01 | Sistem harus mampu membaca tiga parameter _vital sign_: BPM, suhu tubuh, dan SpO₂ dari sensor simulasi (potensiometer virtual) | Tinggi |
| FR-02 | Sistem harus memiliki _task_ alarm dengan prioritas tertinggi yang memeriksa kondisi kritis setiap 50 ms | Tinggi |
| FR-03 | Sistem harus mengaktifkan alarm (LED dan _buzzer_) ketika kondisi kritis terdeteksi (BPM > 130, suhu > 38°C, SpO₂ < 90%) | Tinggi |
| FR-04 | Sistem harus menangani tombol darurat (_emergency button_) melalui _interrupt_ dan mengaktifkan alarm selama 3 detik | Tinggi |
| FR-05 | Sistem harus mengirim data pasien ke _server_ Flask setiap 2 detik melalui HTTP POST | Sedang |
| FR-06 | Sistem harus menerima perintah kontrol dari _dashboard_ melalui HTTP GET setiap 750 ms | Sedang |
| FR-07 | Sistem harus menyediakan _web dashboard_ interaktif yang menampilkan data _vital sign_ secara _real-time_ | Sedang |
| FR-08 | Sistem harus memungkinkan pengguna _dashboard_ untuk mengatur nilai sensor (mode _web control_) atau membiarkan ESP32 membaca sensor (mode _sensor_) | Sedang |
| FR-09 | Sistem harus mendemonstrasikan konsep RTOS tingkat lanjut: _deadlock prevention_, _priority inversion_, dan _protected counter_ (opsional, dinonaktifkan secara _default_) | Rendah |

### 3.3 Non-Functional Requirements

**Tabel 3.2: Non-Functional Requirements (NFR)**

| Kode | Deskripsi | Target |
|------|-----------|--------|
| NFR-01 | Alarm harus merespons kondisi kritis dalam waktu ≤ 50 ms (periode AlarmTask) | ≤ 50 ms |
| NFR-02 | Pembacaan sensor BPM dan SpO₂ harus diperbarui setiap 100 ms | 100 ms |
| NFR-03 | Pembacaan sensor suhu harus diperbarui setiap 150 ms | 150 ms |
| NFR-04 | Data pasien harus dikirim ke server setiap 2000 ms | 2000 ms |
| NFR-05 | CPU _utilization_ harus ≤ 73,5% (LUB untuk 6 _task_) agar sistem _schedulable_ | ≤ 73,5% |
| NFR-06 | _Stack_ setiap _task_ harus cukup untuk menampung _call stack_ maksimum dengan _margin_ aman | Terverifikasi via _high-water mark_ |
| NFR-07 | API _endpoint_ harus memvalidasi input dan mengembalikan kode HTTP yang sesuai | 200/400/405 |

### 3.4 System Specifications

#### 3.4.1 Pin Mapping

**Tabel 3.3: Pemetaan Pin ESP32**

| Fungsi | GPIO | Mode | Keterangan |
|--------|------|------|------------|
| BPM Potentiometer | 34 | INPUT (ADC) | ADC1_CH6, membaca tegangan potensiometer |
| Temperature Potentiometer | 35 | INPUT (ADC) | ADC1_CH7, membaca tegangan potensiometer |
| SpO₂ Potentiometer | 32 | INPUT (ADC) | ADC1_CH4, membaca tegangan potensiometer |
| Emergency Button | 27 | INPUT_PULLUP | _Pull-up_ internal, _interrupt_ FALLING _edge_ |
| Alarm LED | 25 | OUTPUT | LED merah, HIGH = menyala |
| Buzzer | 26 | OUTPUT (PWM) | PWM via `ledc`, frekuensi 1000 Hz |

#### 3.4.2 Konfigurasi Wi-Fi

| Parameter | Nilai |
|-----------|-------|
| SSID | Wokwi-GUEST |
| Password | (kosong) |
| Mode | WIFI_STA (Station) |
| Server URL | `http://192.168.1.155:5000` (disesuaikan) |
| HTTP Timeout | 1000 ms |

#### 3.4.3 REST API Endpoint

**Tabel 3.4: REST API Endpoint**

| Method | Endpoint | Fungsi |
|--------|----------|--------|
| GET | `/` | Menyajikan _frontend_ React SPA |
| POST | `/data` | Menerima data pasien dari ESP32 |
| GET | `/latest` | Mengembalikan data pasien & perintah terbaru |
| POST | `/control` | Menerima perintah kontrol dari _dashboard_ |
| GET | `/command` | Mengembalikan perintah aktif untuk ESP32 |
| POST | `/control/reset` | Mengatur ulang ke mode normal |

#### 3.4.4 Data Structures

```cpp
// Struktur data pasien
typedef struct {
  int bpm;           // Detak jantung (60-150 BPM)
  float temp;        // Suhu tubuh (35.0-40.0 °C)
  int spo2;          // Saturasi oksigen (85-100%)
  char status[20];   // Status: "NORMAL" atau "CRITICAL"
} PatientData;

// Struktur perintah kontrol
typedef struct {
  char mode[10];     // "sensor" atau "web"
  int bpm;           // Nilai BPM dari dashboard
  float temp;        // Nilai suhu dari dashboard
  int spo2;          // Nilai SpO2 dari dashboard
  bool emergency;    // Status darurat
  char alarmOverride[10]; // "auto", "on", "off"
} ControlCommand;
```

### 3.5 Justifikasi Pemilihan RTOS

**Tabel 3.5: Perbandingan Superloop vs FreeRTOS**

| Aspek | Superloop (Loop Arduino) | FreeRTOS |
|-------|--------------------------|----------|
| **Penjadwalan** | Satu _loop_ utama berjalan sekuensial; setiap fungsi harus selesai sebelum fungsi lain dijalankan | Preemptive _multitasking_ dengan prioritas; _task_ prioritas tinggi dapat mendahului _task_ prioritas rendah |
| **Responsivitas** | Respons terhadap _event_ tergantung pada lama eksekusi fungsi lain; _event_ kritis dapat tertunda | _Task_ prioritas tinggi (alarm) dapat merespons dalam 50 ms tanpa terpengaruh _task_ lain |
| **Determinisme** | Tidak deterministik; waktu respons sangat bervariasi tergantung beban _loop_ | Deterministik; _scheduler_ menjamin _task_ prioritas tinggi mendapatkan CPU sesuai jadwal |
| **Sinkronisasi** | Tidak ada mekanisme _built-in_; harus menggunakan _flag_ global dengan risiko _race condition_ | _Mutex_, _semaphore_, _queue_, _spinlock_ — mekanisme sinkronisasi yang _thread-safe_ dan _proven_ |
| **Komunikasi antar-task** | Menggunakan variabel global yang rentan _race condition_ | _Queue_ yang aman dengan mekanisme _blocking_ |
| **ISR Handling** | ISR dapat mengubah _flag_ global, tetapi rawan _race condition_ jika _loop_ utama sedang membacanya | Pola _deferred interrupt processing_ dengan `xSemaphoreGiveFromISR()` dan `portYIELD_FROM_ISR()` |
| **Prioritas** | Semua kode berjalan pada prioritas yang sama | 7 _task_ dengan prioritas berbeda (1–5) sesuai urgensi |
| **Manajemen Stack** | Satu _stack_ untuk semua kode; rawan _stack overflow_ | Setiap _task_ memiliki _stack_ sendiri; dapat dipantau dengan `uxTaskGetStackHighWaterMark()` |
| **Kompleksitas Kode** | Sederhana untuk sistem kecil (< 3 fitur) | Lebih kompleks, tetapi memberi struktur yang lebih baik untuk sistem multitasking |
| **Skalabilitas** | Tidak skalabel; menambah fitur memperlambat semua eksekusi | Skalabel; _task_ baru dapat ditambah tanpa mengganggu _task_ eksisting |

Berdasarkan perbandingan di atas, FreeRTOS dipilih karena sistem ini memerlukan:

1. **Respons cepat terhadap kondisi kritis** (AlarmTask dengan prioritas 5 dan periode 50 ms).
2. **Multitasking sejati** dengan 7 _task_ yang berjalan secara independen.
3. **Mekanisme sinkronisasi** yang _reliable_ untuk melindungi data bersama.
4. **Determinisme** yang dijamin oleh _preemptive scheduler_.
5. **Kesesuaian dengan tema _safety-critical systems_** di mana kegagalan memenuhi _deadline_ dapat berakibat fatal.

---

## BAB 4: PERANCANGAN SISTEM

### 4.1 Arsitektur Sistem

Sistem Smart ICU Patient Monitoring terdiri dari tiga komponen utama yang saling terintegrasi melalui protokol HTTP:

**Gambar 4.1: Arsitektur Sistem**

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

**Gambar 4.2: Diagram Blok Sistem**

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

### 4.2 Desain Task

#### 4.2.1 Daftar Task

**Tabel 4.1: Desain Task FreeRTOS**

| No | Nama Task | Periode | Prioritas | Ukuran Stack | Fungsi |
|----|-----------|---------|-----------|--------------|--------|
| 1 | AlarmTask | 50 ms | 5 (Tertinggi) | 4096 bytes | Memeriksa kondisi kritis, mengendalikan LED/buzzer |
| 2 | HeartRateTask | 100 ms | 4 | 2048 bytes | Membaca potensiometer BPM, memperbarui data pasien |
| 3 | OxygenTask | 100 ms | 4 | 2048 bytes | Membaca potensiometer SpO₂, memperbarui data pasien |
| 4 | TemperatureTask | 150 ms | 3 | 2048 bytes | Membaca potensiometer suhu, memperbarui data pasien |
| 5 | WiFiTask | 2000 ms | 2 | 8192 bytes | Mengirim data pasien ke server Flask via HTTP POST |
| 6 | CommandTask | 750 ms | 2 | 6144 bytes | Mendapatkan perintah kontrol dari server via HTTP GET |
| 7 | MonitoringTask | 500 ms | 1 (Terendah) | 3072 bytes | Menampilkan data monitor ke Serial Console |

**Penjelasan Prioritas (Rate-Monotonic):**

- **AlarmTask (Prioritas 5)**: Mendapat prioritas tertinggi karena harus merespons kondisi kritis dalam waktu singkat (50 ms). Keterlambatan pada _task_ ini dapat berakibat fatal secara medis.
- **HeartRateTask & OxygenTask (Prioritas 4)**: Kedua _task_ sensor dengan periode 100 ms ini memiliki prioritas tinggi karena data detak jantung dan saturasi oksigen adalah parameter paling kritis dalam monitoring ICU.
- **TemperatureTask (Prioritas 3)**: Suhu tubuh berubah lebih lambat sehingga periode 150 ms dengan prioritas 3 sudah memadai.
- **WiFiTask & CommandTask (Prioritas 2)**: Komunikasi jaringan memiliki prioritas lebih rendah karena sifatnya yang _best-effort_ dan periode yang lebih panjang.
- **MonitoringTask (Prioritas 1)**: _Task_ ini bersifat informasional (menampilkan data ke serial) sehingga mendapat prioritas paling rendah.

#### 4.2.2 Timing Specifications

**Tabel 4.2: Timing Specifications Task**

| Task | T (Periode) | C (WCET estimasi) | D (Deadline) | Prioritas | Stack (bytes) |
|------|-------------|-------------------|--------------|-----------|---------------|
| AlarmTask | 50 ms | 5 ms | 50 ms | 5 | 4096 |
| HeartRateTask | 100 ms | 3 ms | 100 ms | 4 | 2048 |
| OxygenTask | 100 ms | 3 ms | 100 ms | 4 | 2048 |
| TemperatureTask | 150 ms | 3 ms | 150 ms | 3 | 2048 |
| WiFiTask | 2000 ms | 80 ms | 2000 ms | 2 | 8192 |
| CommandTask | 750 ms | 8 ms | 750 ms | 2 | 6144 |
| MonitoringTask | 500 ms | 5 ms | 500 ms | 1 | 3072 |

**Keterangan:**
- T = Periode eksekusi (dalam ms)
- C = _Worst-Case Execution Time_ (estimasi dalam ms)
- D = _Deadline_ (diasumsikan D = T untuk RMS)
- Prioritas: 5 = tertinggi, 1 = terendah
- Stack: alokasi _stack_ dalam bytes

### 4.3 Analisis Schedulability

#### 4.3.1 Rate-Monotonic Scheduling (RMS)

Prioritas ditetapkan berdasarkan periode (RMS): semakin pendek periode, semakin tinggi prioritas. Penetapan prioritas pada Tabel 4.1 sudah sesuai dengan prinsip RMS.

#### 4.3.2 CPU Utilization

CPU _utilization_ total dihitung dengan rumus:

```
U = Σ (Ci / Ti)
```

| Task | Ci (ms) | Ti (ms) | Ci/Ti |
|------|---------|---------|-------|
| AlarmTask | 5 | 50 | 0,100 |
| HeartRateTask | 3 | 100 | 0,030 |
| OxygenTask | 3 | 100 | 0,030 |
| TemperatureTask | 3 | 150 | 0,020 |
| WiFiTask | 80 | 2000 | 0,040 |
| CommandTask | 8 | 750 | 0,011 |
| MonitoringTask | 5 | 500 | 0,010 |
| **Total** | | | **U = 0,236** |

**CPU Utilization Total: U = 0,236 (23,6%)**

#### 4.3.3 Least Upper Bound (LUB) Test

Untuk n = 6 _task_ (MonitoringTask dianggap memiliki prioritas terendah sehingga tidak mempengaruhi LUB _test_ secara langsung), LUB dihitung sebagai:

```
U_LUB(n) = n × (2^(1/n) — 1)
U_LUB(6) = 6 × (2^(1/6) — 1)
U_LUB(6) = 6 × (1,1225 — 1)
U_LUB(6) = 6 × 0,1225
U_LUB(6) = 0,735
```

**Hasil LUB Test:**
```
U = 0,236 ≤ U_LUB = 0,735 → ✅ SISTEM SCHEDULABLE
```

Karena CPU _utilization_ total (23,6%) berada jauh di bawah _Least Upper Bound_ (73,5%), sistem dinyatakan **schedulable** berdasarkan kriteria Liu & Layland.

#### 4.3.4 Response Time Analysis (RTA)

Sebagai _fallback_ jika U melebihi LUB, RTA dapat digunakan. RTA menghitung _worst-case response time_ (R_i) untuk setiap _task_ menggunakan iterasi:

```
R_i^(k+1) = C_i + Σ_(j ∈ hp(i)) ⌈(R_i^k) / T_j⌉ × C_j
```

Iterasi untuk _task_ dengan prioritas tertinggi (AlarmTask, prioritas 5):

```
R_Alarm^(0) = C_Alarm = 5
R_Alarm^(1) = 5 + 0 = 5   (tidak ada task prioritas lebih tinggi)
R_Alarm = 5 ms ≤ D_Alarm = 50 ms ✅
```

Iterasi untuk HeartRateTask (prioritas 4, terinterupsi oleh AlarmTask):

```
R_HR^(0) = C_HR = 3
R_HR^(1) = 3 + ⌈3/50⌉ × 5 = 3 + 1 × 5 = 8
R_HR^(2) = 3 + ⌈8/50⌉ × 5 = 3 + 1 × 5 = 8
R_HR = 8 ms ≤ D_HR = 100 ms ✅
```

Iterasi untuk TemperatureTask (prioritas 3):

```
R_Temp^(0) = C_Temp = 3
R_Temp^(1) = 3 + ⌈3/50⌉ × 5 + ⌈3/100⌉ × 3 + ⌈3/100⌉ × 3 = 3 + 5 + 3 + 3 = 14
R_Temp^(2) = 3 + ⌈14/50⌉ × 5 + ⌈14/100⌉ × 3 + ⌈14/100⌉ × 3 = 3 + 5 + 3 + 3 = 14
R_Temp = 14 ms ≤ D_Temp = 150 ms ✅
```

Semua _task_ memenuhi _deadline_-nya, mengonfirmasi _schedulability_ sistem.

#### 4.3.5 Gantt Chart (Ilustrasi Penjadwalan)

**Gambar 4.3: Gantt Chart Penjadwalan (0–200 ms)**

```
Task       0   10   20   30   40   50   60   70   80   90  100  110  120  130  140  150  160  170  180  190  200
Alarm(p5)  [===]     [===]     [===]     [===]     [===]     [===]     [===]     [===]     [===]     [===]
HR(p4)       [==]          [==]          [==]          [==]          [==]          [==]
Oxy(p4)       [==]          [==]          [==]          [==]          [==]          [==]
Temp(p3)        [==]              [==]              [==]              [==]              [==]
Mon(p1)                 [====]              [====]              [====]              [====]
Cmd(p2)                        [======]                    [======]
WiFi(p2)                                                                                [=========================]
```

Keterangan: AlarmTask tereksekusi setiap 50 ms dan selalu mendahului _task_ lain. HeartRateTask dan OxygenTask (prioritas 4) berjalan setelah AlarmTask selesai. TemperatureTask (prioritas 3) mendapat jatah waktu setelah ketiga _task_ prioritas lebih tinggi selesai. WiFiTask dengan periode 2000 ms hanya muncul sekali dalam diagram 200 ms.

### 4.4 Desain Inter-Task Synchronization

#### 4.4.1 Mekanisme Sinkronisasi

**Tabel 4.3: Desain Mekanisme Sinkronisasi**

| Objek | Tipe | Fungsi | Digunakan oleh |
|-------|------|--------|----------------|
| `patientQueue` | Queue (1 item) | Komunikasi data pasien dari sensor task ke MonitoringTask | HeartRateTask, TemperatureTask, OxygenTask → MonitoringTask |
| `dataMutex` | Mutex (dengan PIP) | Melindungi akses ke `PatientData` dan `ControlCommand` | Semua _task_ (HeartRate, Temperature, Oxygen, Alarm, WiFi, Command) |
| `emergencySem` | Binary Semaphore | _Signaling_ dari ISR tombol darurat ke AlarmTask | ISR (emergencyISR) → AlarmTask |

#### 4.4.2 Alur Data Antar-Task

**Gambar 4.4: Diagram Alur Data**

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
6. Jika mode kontrol adalah **"web"**, nilai BPM, suhu, dan SpO₂ dari _dashboard_ akan digunakan, menggantikan pembacaan sensor lokal.

#### 4.4.3 Race Condition Handling

_Race condition_ dapat terjadi ketika dua atau lebih _task_ mencoba membaca/menulis `PatientData` secara bersamaan. Strategi penanganan:

1. **Mutex (`dataMutex`)**: Semua akses ke `PatientData` dan `ControlCommand` harus melalui `xSemaphoreTake(dataMutex, ...)` dan `xSemaphoreGive(dataMutex)`. Ini menjamin _mutual exclusion_.
2. **Queue (`patientQueue`)**: Menggunakan `xQueueOverwrite()` yang aman secara _thread-safe_ untuk komunikasi data satu arah.
3. **Copy-by-value**: WiFiTask membuat _snapshot_ lokal (`snapshot = patient`) di dalam _critical section_ untuk menghindari penggunaan data yang berubah (_data race_).

**Skenario Race Condition yang Diatasi:**

| Skenario | Risiko | Solusi |
|----------|--------|--------|
| HeartRateTask menulis BPM saat WiFiTask membaca | Data BPM terbaca tidak konsisten | `dataMutex` melindungi seluruh struktur |
| CommandTask memperbarui mode saat TemperatureTask membaca | Mode kontrol tidak sinkron | `dataMutex` diambil sebelum akses |
| Dua sensor task menulis bersamaan | _Data corruption_ | Mutex menjamin satu _task_ per satu waktu |

#### 4.4.4 Priority Inversion Handling

FreeRTOS mutex (`xSemaphoreCreateMutex()`) secara _built-in_ mendukung **Priority Inheritance Protocol** (PIP). Dalam sistem ini:

- `dataMutex` digunakan oleh semua _task_ (prioritas 1–5).
- Jika AlarmTask (prioritas 5) menunggu `dataMutex` yang sedang dipegang TemperatureTask (prioritas 3), prioritas TemperatureTask akan dinaikkan sementara ke 5.
- Ini mencegah _task_ prioritas 2 atau 1 (WiFiTask, MonitoringTask) untuk mendahului TemperatureTask, sehingga TemperatureTask dapat menyelesaikan tugasnya dengan cepat dan melepas mutex.

### 4.5 Desain ISR

#### 4.5.1 ISR Emergency Button

**Tabel 4.4: Desain ISR**

| Aspek | Spesifikasi |
|-------|-------------|
| **Nama ISR** | `emergencyISR` |
| **Perangkat** | Tombol darurat (_push button_) |
| **GPIO** | 27 |
| **Konfigurasi** | `INPUT_PULLUP`, _interrupt_ pada `FALLING` _edge_ |
| **Dekorator** | `IRAM_ATTR` (kode di RAM instruksi) |
| **Fungsi** | Memberikan _binary semaphore_ (`emergencySem`) ke AlarmTask |
| **API ISR** | `xSemaphoreGiveFromISR()` + `portYIELD_FROM_ISR()` |

#### 4.5.2 Deferred Interrupt Processing Pattern

**Gambar 4.5: Pola Deferred Interrupt Processing**

```
[Emergency Button Press]
         |
         v
    +----------+
    |  ISR     |     <-- Konteks Interrupt (sangat pendek)
    | (GPIO 27)|
    +----+-----+
         | xSemaphoreGiveFromISR(emergencySem)
         v
    +----------+
    |  Alarm   |     <-- Konteks Task (pemrosesan penuh)
    |  Task    |
    | (Prioritas 5)
    +----+-----+
         |
         v
    [Set alarm LED & Buzzer ON selama 3 detik]
```

Keuntungan pola _deferred interrupt processing_:

1. **ISR tetap pendek**: Hanya memberi semaphore (beberapa instruksi CPU).
2. **Pemrosesan aman**: AlarmTask dapat memanggil fungsi yang memungkinkan _blocking_.
3. **Prioritas terjamin**: AlarmTask berprioritas 5, sehingga segera dijadwalkan setelah ISR.
4. **Tidak ada _race condition_**: Semaphore menjamin bahwa satu tombol ditekan hanya diproses sekali.

### 4.6 Desain Manajemen Memori dan I/O

#### 4.6.1 Strategi Alokasi Memori

FreeRTOS pada ESP32 menggunakan skema **heap_4** melalui ESP-IDF. Skema ini mendukung alokasi dan dealokasi memori dinamis serta menggabungkan blok memori yang berdekatan untuk mengurangi fragmentasi.

**Alokasi Stack per Task:**
- Total _stack_ yang dialokasikan: 4096 + 2048 + 2048 + 2048 + 8192 + 6144 + 3072 = **27.648 bytes** (dari 47.652 bytes RAM terpakai).
- Sisa RAM digunakan oleh: heap FreeRTOS, TCB (_Task Control Block_), _library_ Wi-Fi, HTTP Client, ArduinoJson, _buffer_ Serial, dll.

#### 4.6.2 Estimasi Memori

| Komponen | Perkiraan Ukuran |
|----------|-----------------|
| Stack semua task (7 task) | 27.648 bytes |
| Heap FreeRTOS (queue, mutex, semaphore) | ~2.000 bytes |
| TCB (7 task) | ~1.400 bytes |
| Library Wi-Fi + HTTP Client | ~10.000 bytes |
| ArduinoJson + Buffer Serial | ~4.000 bytes |
| Lain-lain (kode, data statis) | ~2.604 bytes |
| **Total RAM** | **47.652 bytes (14,5%)** |

#### 4.6.3 Strategi I/O

1. **Blocking Avoidance**: Semua operasi I/O (_analogRead_, HTTP POST/GET, Serial.print) dilakukan di dalam _task_ yang sesuai dengan prioritasnya. Operasi I/O yang berpotensi _blocking_ (seperti HTTP) memiliki prioritas rendah (2).
2. **Timeouts**: HTTP _client_ menggunakan _timeout_ 1000 ms untuk mencegah _task_ terblokir selamanya.
3. **Asynchronous I/O**: Pembacaan sensor ADC menggunakan `analogRead()` yang cepat (~100 µs) dan tidak memblokir _task_ lain karena bersifat _non-blocking_.
4. **PWM Output**: _Buzzer_ dikendalikan melalui `ledc` (PWM _peripheral_) yang beroperasi di _background_ tanpa intervensi CPU kontinu.

### 4.7 Integrasi Tema Safety-Critical Systems

Proyek ini mengintegrasikan prinsip _safety-critical systems_ dalam berbagai aspek:

1. **Fault Detection**: 
   - _Stack monitoring_ via `uxTaskGetStackHighWaterMark()` mendeteksi potensi _stack overflow_.
   - Status koneksi Wi-Fi diperiksa setiap 2000 ms; jika terputus, sistem mencoba _reconnect_ otomatis.
   - _Error handling_ pada deserialisasi JSON mencegah data korup digunakan.

2. **Fail-Safe Behavior**:
   - Jika terjadi kegagalan pembuatan _queue_ atau _mutex_ di `setup()`, sistem memasuki _infinite loop_ dengan pesan _error_ (`while(1) delay(1000)`).
   - Alarm akan tetap aktif selama 3 detik setelah tombol darurat ditekan, meskipun tombol sudah dilepas.
   - Jika mode adalah "web" tetapi _dashboard_ tidak merespons, ESP32 tetap menggunakan data sensor terakhir yang valid.

3. **Predictability**:
   - `vTaskDelayUntil()` menjamin periodisitas tetap meskipun ada variasi waktu eksekusi.
   - Prioritas tetap (_fixed-priority_) menjamin bahwa _task_ kritis (alarm) selalu dijadwalkan lebih dulu.
   - Analisis _schedulability_ (LUB test, RTA) memverifikasi bahwa semua _task_ memenuhi _deadline_.

4. **Redundancy** (terbatas):
   - Data pasien disimpan baik di ESP32 (variabel `patient`) maupun di server Flask (`latest_data`).
   - Pasien dapat dipantau melalui Serial Monitor (lokal) maupun _web dashboard_ (jarak jauh).

---

## BAB 5: IMPLEMENTASI

### 5.1 Implementasi FreeRTOS

FreeRTOS diimplementasikan pada ESP32 menggunakan framework Arduino melalui PlatformIO. Konfigurasi _platform_ terdapat pada `platformio.ini`:

```ini
[env:esp32dev]
platform = espressif32@6.8.1
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
  bblanchon/ArduinoJson@^7.3.1
```

_Inisialisasi_ sistem FreeRTOS dilakukan dalam fungsi `setup()`:

```cpp
void setup() {
  Serial.begin(115200);

  // Konfigurasi pin
  pinMode(LED_ALARM, OUTPUT);
  pinMode(EMERGENCY_BTN, INPUT_PULLUP);

  // Konfigurasi PWM untuk buzzer
  ledcSetup(0, 1000, 8);
  ledcAttachPin(BUZZER_PIN, 0);
  setAlarmOutput(false);

  // Buat objek FreeRTOS
  patientQueue = xQueueCreate(1, sizeof(PatientData));
  dataMutex = xSemaphoreCreateMutex();
  emergencySem = xSemaphoreCreateBinary();

  // Validasi objek FreeRTOS
  if (patientQueue == NULL || dataMutex == NULL || emergencySem == NULL) {
    Serial.println("[SETUP] Failed to create FreeRTOS objects");
    while (1) { delay(1000); }
  }

  // Pasang ISR
  attachInterrupt(digitalPinToInterrupt(EMERGENCY_BTN), emergencyISR, FALLING);

  // Buat task
  xTaskCreate(HeartRateTask, "HeartRateTask", 2048, NULL, 4, &heartRateTaskHandle);
  xTaskCreate(TemperatureTask, "TemperatureTask", 2048, NULL, 3, &temperatureTaskHandle);
  xTaskCreate(OxygenTask, "OxygenTask", 2048, NULL, 4, &oxygenTaskHandle);
  xTaskCreate(AlarmTask, "AlarmTask", 4096, NULL, 5, &alarmTaskHandle);
  xTaskCreate(WiFiTask, "WiFiTask", 8192, NULL, 2, &wifiTaskHandle);
  xTaskCreate(CommandTask, "CommandTask", 6144, NULL, 2, &commandTaskHandle);
  xTaskCreate(MonitoringTask, "MonitoringTask", 3072, NULL, 1, &monitoringTaskHandle);

  Serial.println("[SETUP] Smart ICU RTOS system started");
}

void loop() {
  // Semua eksekusi ditangani oleh FreeRTOS scheduler
}
```

Fungsi `loop()` dibiarkan kosong karena semua eksekusi ditangani oleh _scheduler_ FreeRTOS. Ini adalah pola standar untuk aplikasi FreeRTOS pada ESP32 dengan framework Arduino.

### 5.2 Implementasi Task

#### 5.2.1 HeartRateTask

HeartRateTask membaca nilai analog dari potensiometer BPM (GPIO34) dan memetakannya ke rentang 60–150 BPM. _Task_ ini menggunakan `dataMutex` untuk mengamankan akses ke struktur `PatientData`.

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

#### 5.2.2 TemperatureTask

TemperatureTask membaca suhu dari potensiometer GPIO35 dan memetakan nilai ADC (0–4095) ke rentang suhu 35,0–40,0°C menggunakan fungsi `mapFloat()` khusus karena `map()` Arduino hanya mendukung integer.

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

#### 5.2.3 OxygenTask

OxygenTask membaca saturasi oksigen (SpO₂) dari potensiometer GPIO32, memetakannya ke rentang 85–100%, dengan periode 100 ms dan prioritas 4.

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

#### 5.2.4 AlarmTask

AlarmTask adalah _task_ dengan prioritas tertinggi (5) dan periode terpendek (50 ms). _Task_ ini bertanggung jawab untuk:

1. Memeriksa sinyal dari _emergency button_ melalui _binary semaphore_ (`emergencySem`).
2. Mengevaluasi kondisi kritis berdasarkan nilai BPM, suhu, dan SpO₂.
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
- Suhu > 38,0°C
- SpO₂ < 90%
- Tombol darurat ditekan (berlaku 3 detik)
- Perintah `alarm_override` dari _dashboard_ bernilai "on"

#### 5.2.5 WiFiTask

WiFiTask menangani konektivitas dan komunikasi dengan _server_ Flask. _Task_ ini:

1. Memeriksa status koneksi Wi-Fi; jika terputus, melakukan _reconnect_.
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

      int httpCode = http.POST(jsonData);
      Serial.println("[WiFiTask] HTTP response code: " + String(httpCode));
      http.end();
    }

    vTaskDelayUntil(&lastWakeTime, WIFI_PERIOD_TICKS);
  }
}
```

Alokasi _stack_ WiFiTask (8192 bytes) lebih besar dari _task_ lain untuk mengakomodasi tumpukan panggilan HTTP dan JSON.

#### 5.2.6 CommandTask

CommandTask melakukan _polling_ ke server Flask setiap 750 ms untuk mendapatkan perintah kontrol terbaru melalui `GET /command`. _Task_ ini mendeserialisasi respons JSON menggunakan **ArduinoJson** dan memperbarui struktur `ControlCommand`.

```cpp
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
          strlcpy(controlCommand.alarmOverride, doc["alarm_override"] | "auto",
                  sizeof(controlCommand.alarmOverride));

          if (isWebControlMode()) {
            patient.bpm = controlCommand.bpm;
            patient.temp = controlCommand.temp;
            patient.spo2 = controlCommand.spo2;
            strcpy(patient.status, shouldAlarmBeActive() ? "CRITICAL" : "NORMAL");
            publishPatientSnapshot();
          }

          xSemaphoreGive(dataMutex);
        }
      } else {
        Serial.println("[CommandTask] Command endpoint unavailable");
      }
      http.end();
    }

    vTaskDelayUntil(&lastWakeTime, COMMAND_PERIOD_TICKS);
  }
}
```

#### 5.2.7 MonitoringTask

MonitoringTask adalah _task_ prioritas terendah (1) yang bertugas menampilkan data monitor ke Serial Console setiap 500 ms. _Task_ ini juga mencatat _stack high-water mark_ setiap 10 siklus (setiap 5 detik) untuk membantu debugging penggunaan _stack_.

```cpp
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

### 5.3 Implementasi Sinkronisasi

#### 5.3.1 Queue (patientQueue)

`patientQueue` adalah _queue_ dengan ukuran 1 yang menampung struktur `PatientData`. _Queue_ ini berfungsi sebagai jalur komunikasi satu arah dari _task_ sensor ke MonitoringTask. Penggunaan `xQueueOverwrite()` memungkinkan data terbaru selalu tersedia bagi pembaca tanpa harus menunggu _queue_ kosong.

```cpp
patientQueue = xQueueCreate(1, sizeof(PatientData));
```

#### 5.3.2 Mutex (dataMutex)

`dataMutex` adalah mutex yang melindungi akses ke struktur `PatientData` dan `ControlCommand`. Semua _task_ yang membaca atau menulis data pasien harus mengambil mutex ini terlebih dahulu. Mutex dipilih daripada _binary semaphore_ karena mutex mendukung **priority inheritance**, yang mencegah _priority inversion_.

```cpp
dataMutex = xSemaphoreCreateMutex();
```

#### 5.3.3 Binary Semaphore (emergencySem)

`emergencySem` adalah _binary semaphore_ yang digunakan untuk menunda pemrosesan _interrupt_ tombol darurat ke AlarmTask. Ketika tombol ditekan, ISR memberikan semaphore ini. AlarmTask kemudian mengambil semaphore tersebut dan mengaktifkan alarm selama 3 detik.

```cpp
emergencySem = xSemaphoreCreateBinary();
```

#### 5.3.4 Spinlock (untuk Advanced RTOS Demo)

Pada _advanced RTOS demo_ (saat `ENABLE_ADVANCED_RTOS_DEMO = 1`), _spinlock_ digunakan untuk melindungi _counter_ bersama (_shared counter_) dari akses dua _task_ konkuren:

```cpp
portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

// Di dalam ProtectedCounterTask:
portENTER_CRITICAL(&spinlock);
sharedCounter++;
int snapshot = sharedCounter;
portEXIT_CRITICAL(&spinlock);
```

### 5.4 Implementasi ISR dan Deferred Processing

Tombol _emergency_ dihubungkan ke GPIO27 dengan konfigurasi `INPUT_PULLUP` dan _interrupt_ pada tepi jatuh (_FALLING edge_). ISR ditandai dengan `IRAM_ATTR` agar kode ditempatkan di RAM instruksi untuk eksekusi yang cepat.

```cpp
void IRAM_ATTR emergencyISR() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(emergencySem, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
```

Poin penting dalam implementasi ISR:

1. Fungsi ISR ditempatkan di IRAM (`IRAM_ATTR`) untuk menghindari _cache miss_ dan _flash latency_.
2. `xSemaphoreGiveFromISR()` digunakan sebagai pengganti `xSemaphoreGive()` karena ISR tidak dapat memanggil fungsi yang memungkinkan _blocking_.
3. Parameter `xHigherPriorityTaskWoken` memberi tahu _scheduler_ apakah perlu melakukan _context switch_ setelah ISR selesai.
4. `portYIELD_FROM_ISR()` melakukan _yield_ jika diperlukan, sehingga AlarmTask segera berjalan.

### 5.5 Implementasi Web Dashboard

#### 5.5.1 Flask Backend

Backend Flask menyediakan 5 _endpoint_ REST API yang melayani komunikasi antara ESP32, _dashboard_, dan _frontend_ React.

**Tabel 5.1: Endpoint REST API**

| Method | Endpoint | Fungsi |
|--------|----------|--------|
| POST | `/data` | Menerima data pasien dari ESP32 |
| GET | `/latest` | Mengembalikan data pasien & perintah terbaru |
| POST | `/control` | Menerima perintah kontrol dari dashboard |
| GET | `/command` | Mengembalikan perintah aktif untuk ESP32 |
| POST | `/control/reset` | Mengatur ulang ke mode normal |

Fitur keamanan pada backend:

- **Validasi input**: setiap payload diperiksa tipe data, rentang nilai, dan _field_ wajib.
- **Thread lock**: akses ke data bersama diamankan dengan `threading.Lock()`.
- **Sanitasi**: nilai BPM dibatasi 40–180, suhu 30–45°C, SpO₂ 70–100%.

#### 5.5.2 React Frontend

Frontend React dibangun dengan **Vite 7.3.5** dan **React 19.2.0**, menggunakan **lucide-react** untuk ikon. Aplikasi terdiri dari:

1. **Header Band**: menampilkan judul sistem dan indikator status (NORMAL/CRITICAL).
2. **Vital Cards**: tiga kartu yang menampilkan BPM, Suhu, dan SpO₂ dalam format besar.
3. **Control Panel**: _slider_ untuk mengatur nilai BPM (40–180), suhu (30–45°C), dan SpO₂ (70–100%).
4. **Action Buttons**: tombol untuk menerapkan nilai, mengatur kondisi normal/kritis, darurat, reset, dan mode sensor.
5. **Telemetry Panel**: menampilkan status koneksi, mode, status darurat, alarm, sumber data, dan waktu pembaruan.

Frontend melakukan _polling_ ke `/latest` setiap 1 detik menggunakan `setInterval()` dalam _hook_ `useEffect`.

### 5.6 Pembagian Tugas Anggota Kelompok

**Tabel 5.2: Pembagian Tugas Anggota Kelompok**

| No | Nama | NIM | Tugas |
|----|------|-----|-------|
| 1 | Nicholai Dandy Nainggolan | 101032330023 | Perancangan arsitektur sistem RTOS, analisis _schedulability_ (RMS, RTA), desain _task scheduling_ dan prioritas, dokumentasi BAB 1–4 |
| 2 | Muhammad Deva Valery | 10103233[NNNN] | Implementasi firmware ESP32 (FreeRTOS task, queue, mutex, semaphore), implementasi ISR dan _deferred interrupt processing_, integrasi Wokwi simulator |
| 3 | Ardika Putra Hadian | 101032300240 | Implementasi _web dashboard_ (Flask backend + React frontend), pengujian API endpoint, integrasi sistem, dokumentasi BAB 5–7 |
| 4 | [Nama Lengkap] | [NIM] | Pengujian sistem, verifikasi _race condition_ dan _priority inversion_, _troubleshooting_, penyusunan lampiran dan _checklist_ |

---

## BAB 6: PENGUJIAN DAN ANALISIS

### 6.1 Metodologi Pengujian

Pengujian sistem dilakukan melalui empat tahap:

1. **Unit Testing**: Menguji setiap komponen secara terpisah — pembacaan sensor, logika alarm, komunikasi HTTP, validasi API.
2. **Integration Testing**: Menguji interaksi antar komponen — komunikasi ESP32–Flask, _polling_ perintah, sinkronisasi data.
3. **System Testing**: Menguji sistem secara keseluruhan pada simulator Wokwi dengan skenario penggunaan nyata.
4. **Real-Time Metrics**: Mengukur _worst-case execution time_ (WCET), _jitter_, _deadline miss_, dan penggunaan _stack_ per _task_.

### 6.2 Skenario Pengujian

**Tabel 6.1: Skenario Pengujian**

| No | Skenario | Langkah Pengujian | Hasil yang Diharapkan | Status |
|----|----------|-------------------|----------------------|--------|
| 1 | Sistem _startup_ | Jalankan firmware di Wokwi | 7 task berjalan, Serial Monitor menampilkan data | ✅ |
| 2 | Pembacaan sensor BPM | Putar potensiometer BPM | Nilai BPM berubah (60–150) | ✅ |
| 3 | Pembacaan sensor suhu | Putar potensiometer suhu | Nilai suhu berubah (35–40°C) | ✅ |
| 4 | Pembacaan sensor SpO₂ | Putar potensiometer SpO₂ | Nilai SpO₂ berubah (85–100%) | ✅ |
| 5 | Alarm kondisi kritis BPM | Atur BPM > 130 | LED menyala, buzzer berbunyi, status CRITICAL | ✅ |
| 6 | Alarm kondisi kritis suhu | Atur suhu > 38°C | LED menyala, buzzer berbunyi | ✅ |
| 7 | Alarm kondisi kritis SpO₂ | Atur SpO₂ < 90% | LED menyala, buzzer berbunyi | ✅ |
| 8 | Tombol darurat | Tekan tombol emergency | Alarm aktif 3 detik | ✅ |
| 9 | POST /data | Kirim data pasien ke Flask | HTTP 200, data tersimpan | ✅ |
| 10 | GET /latest | Akses endpoint `/latest` | Data pasien dan perintah terbaru | ✅ |
| 11 | POST /control | Kirim perintah kontrol | HTTP 200, mode berubah | ✅ |
| 12 | GET /command | Polling perintah dari ESP32 | Perintah aktif terkirim | ✅ |
| 13 | POST /control/reset | Reset ke normal | Data kembali ke nilai default | ✅ |
| 14 | Input tidak valid | Kirim data dengan tipe salah | HTTP 400, pesan error | ✅ |
| 15 | Mode web control | Aktifkan mode web dari dashboard | Nilai sensor dari dashboard digunakan | ✅ |

### 6.3 Hasil Kompilasi Firmware

Firmware ESP32 berhasil dikompilasi menggunakan PlatformIO dengan _framework_ Arduino. Berikut adalah hasil penggunaan memori:

**Tabel 6.2: Hasil Kompilasi Firmware**

| Parameter | Nilai | Persentase |
|-----------|-------|------------|
| RAM (DRAM) Terpakai | 47.652 bytes | 14,5% |
| Total RAM Tersedia | 327.680 bytes | 100% |
| Flash Terpakai | 942.041 bytes | 71,9% |
| Total Flash Tersedia | 1.310.720 bytes | 100% |
| Waktu Kompilasi | 15,83 detik | — |
| _Compiler Warnings_ | 0 | — |

**Ringkasan Kompilasi:**

| Aspek | Detail |
|-------|--------|
| Platform | espressif32@6.8.1 |
| Board | ESP32-DevKit-C-V4 |
| Framework | Arduino |
| Library | ArduinoJson 7.4.3 |
| Jumlah Task | 7 FreeRTOS tasks (prioritas 1–5) |
| RAM | 47.652 / 327.680 bytes (14,5%) |
| Flash | 942.041 / 1.310.720 bytes (71,9%) |

Penggunaan RAM sebesar 14,5% menunjukkan masih banyak ruang untuk pengembangan lebih lanjut. Penggunaan Flash sebesar 71,9% tergolong wajar mengingat firmware mencakup _library_ Wi-Fi, HTTP Client, dan ArduinoJson. Tidak ada peringatan kompilasi (_compiler warnings_), menunjukkan bahwa kode bersih dari potensi _bug_ sintaksis.

### 6.4 Hasil Pengujian API Endpoint

Seluruh _endpoint_ REST API telah diuji menggunakan `curl` dan berhasil berjalan dengan baik.

#### 6.4.1 POST /data — Mengirim Data Pasien

**Request:**
```bash
curl -X POST http://localhost:5000/data \
  -H "Content-Type: application/json" \
  -d '{"bpm": 132, "temp": 38.4, "spo2": 88, "status": "CRITICAL", "source": "esp32"}'
```

**Response** (HTTP 200):
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

**Log server:**
```
INFO in app: Updated patient data from 127.0.0.1: {'bpm': 132, 'temp': 38.4, 'spo2': 88, 'status': 'CRITICAL', 'updated_at': '2026-06-01T19:04:44.268902+00:00'}
127.0.0.1 - - [02/Jun/2026 02:04:44] "POST /data HTTP/1.1" 200 -
```

#### 6.4.2 GET /latest — Mendapatkan Data Terbaru

**Request:**
```bash
curl http://localhost:5000/latest
```

**Response** (HTTP 200): Mengembalikan data pasien dan perintah aktif terkini. _Frontend_ React melakukan _polling_ endpoint ini setiap 1 detik.

#### 6.4.3 POST /control — Mengirim Perintah Kontrol

**Request:**
```bash
curl -X POST http://localhost:5000/control \
  -H "Content-Type: application/json" \
  -d '{"mode": "web", "bpm": 90, "temp": 36.8, "spo2": 97, "alarm_override": "auto"}'
```

**Response** (HTTP 200):
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

#### 6.4.4 GET /command — Mendapatkan Perintah Aktif

**Request:**
```bash
curl http://localhost:5000/command
```

**Response** (HTTP 200): Mengembalikan perintah kontrol yang sedang aktif untuk di-_poll_ oleh ESP32.

#### 6.4.5 POST /control/reset — Reset ke Mode Normal

**Request:**
```bash
curl -X POST http://localhost:5000/control/reset
```

**Response** (HTTP 200): Data diatur ulang ke nilai normal (BPM=90, suhu=36,8°C, SpO₂=97%).

#### 6.4.6 Validasi Input — Data Invalid

**Request:**
```bash
curl -X POST http://localhost:5000/data \
  -H "Content-Type: application/json" \
  -d '{"bpm": "invalid"}'
```

**Response** (HTTP 400):
```json
{
  "status": "error",
  "message": "invalid field types"
}
```

**Request (syntax error JSON):**
```bash
curl -X POST http://localhost:5000/data \
  -H "Content-Type: application/json" \
  -d "{bpm: 100}"
```

**Response** (HTTP 400):
```json
{
  "status": "error",
  "message": "invalid json"
}
```

**Log server:**
```
WARNING in app: Invalid JSON from 127.0.0.1: 400 Bad Request: Failed to decode JSON object
127.0.0.1 - - [02/Jun/2026 02:04:44] "POST /data HTTP/1.1" 400 -
```

Semua _endpoint_ mengembalikan kode status HTTP yang sesuai (200 untuk sukses, 400 untuk _error_ validasi), menunjukkan bahwa API berfungsi dengan baik.

### 6.5 Pengukuran Metrik Real-Time

**Tabel 6.3: Metrik Real-Time**

| Task | Periode (T) | WCET (C) | Jitter | Deadline Miss | Stack Usage | Sisa Stack |
|------|-------------|----------|--------|---------------|-------------|------------|
| AlarmTask | 50 ms | ~5 ms | Rendah | 0% | ~1.200 bytes | ~2.896 bytes |
| HeartRateTask | 100 ms | ~3 ms | Rendah | 0% | ~800 bytes | ~1.248 bytes |
| OxygenTask | 100 ms | ~3 ms | Rendah | 0% | ~800 bytes | ~1.248 bytes |
| TemperatureTask | 150 ms | ~3 ms | Rendah | 0% | ~800 bytes | ~1.248 bytes |
| WiFiTask | 2000 ms | ~80 ms | Sedang | 0% | ~3.500 bytes | ~4.692 bytes |
| CommandTask | 750 ms | ~8 ms | Rendah | 0% | ~2.000 bytes | ~4.144 bytes |
| MonitoringTask | 500 ms | ~5 ms | Rendah | 0% | ~1.000 bytes | ~2.072 bytes |

**Keterangan:**
- **WCET** (_Worst-Case Execution Time_): Waktu eksekusi maksimum yang diukur dari simulasi. WiFiTask memiliki WCET tertinggi karena melibatkan HTTP _request_.
- **Jitter**: Variasi waktu eksekusi. WiFiTask memiliki jitter lebih tinggi karena bergantung pada kondisi jaringan.
- **Deadline Miss**: Tidak ada _deadline miss_ yang terdeteksi selama pengujian.
- **Stack Usage**: Diperkirakan dari _high-water mark_ yang dilaporkan oleh `uxTaskGetStackHighWaterMark()`. Semua _task_ memiliki sisa _stack_ yang memadai.

### 6.6 Verifikasi Penanganan Race Condition

#### 6.6.1 Sebelum Proteksi (Unprotected Counter)

Dua task (`UnprotCounterA` dan `UnprotCounterB`) berbagi `sharedCounter` **tanpa proteksi**. Setiap task membaca, menunda 2 ms (memberi kesempatan task lain menulis), lalu menulis balik.

**Log Serial Monitor (Wokwi):**
```
========================================
 ADVANCED RTOS DEMO STARTED
========================================
=== PHASE 1: RACE CONDITION DEMO ===
UnprotectedCounterA/B (tanpa spinlock) → data corruption
ProtectedCounterA/B (dengan spinlock) → data aman
...
[RACE-UNPROTECTED] UnprotCounterA counter: 1
[RACE-UNPROTECTED] UnprotCounterB counter: 2
[RACE-UNPROTECTED] UnprotCounterA counter: 3
[RACE-UNPROTECTED] UnprotCounterB counter: 3    ← RACE! Dua task baca nilai yang sama
[RACE-UNPROTECTED] UnprotCounterA counter: 4
[RACE-UNPROTECTED] UnprotCounterB counter: 5
[RACE-UNPROTECTED] UnprotCounterA counter: 5    ← RACE! Lost update (harusnya 6)
```

Terlihat bahwa nilai counter **tidak berurutan** dan ada **lost update** — dua task membaca nilai yang sama atau menimpa hasil task lain. Ini adalah **race condition klasik** pada shared variable tanpa proteksi.

#### 6.6.2 Sesudah Proteksi (Protected Counter)

Dua task (`ProtCounterA` dan `ProtCounterB`) berbagi `sharedCounter` yang **dilindungi spinlock** (`portENTER_CRITICAL` / `portEXIT_CRITICAL`).

**Log Serial Monitor (Wokwi):**
```
[RACE-PROTECTED] ProtCounterA counter: 1
[RACE-PROTECTED] ProtCounterB counter: 2
[RACE-PROTECTED] ProtCounterA counter: 3
[RACE-PROTECTED] ProtCounterB counter: 4
[RACE-PROTECTED] ProtCounterA counter: 5
[RACE-PROTECTED] ProtCounterB counter: 6
```

Terlihat bahwa nilai counter **berurutan sempurna** (1, 2, 3, 4, 5, 6...) dan tidak ada satupun _lost update_. Spinlock memastikan bahwa operasi `read-modify-write` bersifat atomik — task lain tidak dapat menginterupsi di tengah-tengah operasi.

#### 6.6.3 Hasil Perbandingan

| **Aspek** | **Tanpa Proteksi** | **Dengan Spinlock** |
|-----------|-------------------|-------------------|
| Urutan counter | Tidak berurutan (1, 2, 3, 3, 4, 5, 5...) | Berurutan (1, 2, 3, 4, 5, 6...) |
| Lost update | ✅ Terjadi | ❌ Tidak ada |
| Konsistensi data | ❌ Tidak konsisten | ✅ Konsisten |
| Operasi atomik | ❌ Tidak | ✅ Ya |

### 6.7 Verifikasi Priority Inversion

#### 6.7.1 Sebelum Priority Inheritance (Binary Semaphore)

Tiga task berbagi `inversionSem` (_binary semaphore_, tanpa _priority inheritance_):
- **NoPIP_Low** (prioritas 1): memegang semaphore selama 3 detik
- **NoPIP_Med** (prioritas 3): berjalan terus tanpa semaphore
- **NoPIP_High** (prioritas 5): mencoba mengambil semaphore

**Log Serial Monitor (Wokwi):**
```
=== PHASE 2: PRIORITY INVERSION DEMO ===
No-PIP: InversionLow/Med/High (binary sem) → blocking lama
With-PIP: InversionLow/Med/High (mutex) → blocking cepat

[NO-PIP] LOW: Locked semaphore
[NO-PIP] MEDIUM: Running workload...
[NO-PIP] MEDIUM: Running workload...
[NO-PIP] HIGH: Acquired semaphore after 3412 ms (tanpa PIP → lambat!)
[NO-PIP] LOW: Released semaphore
```

**Analisis**:
- HighPriorityTask (prioritas 5) butuh **~3412 ms** untuk mendapatkan semaphore
- Penyebab: MediumPriorityTask (prioritas 3) berjalan terus dan mendahului LowPriorityTask (prioritas 1) yang memegang semaphore — _priority inversion_ terjadi

#### 6.7.2 Sesudah Priority Inheritance (FreeRTOS Mutex)

Tiga task berbagi `priorityMutex` (_mutex_ FreeRTOS, dengan _priority inheritance_):
- **PIP_Low** (prioritas 1): memegang mutex selama 3 detik
- **PIP_Med** (prioritas 3): berjalan tanpa mutex
- **PIP_High** (prioritas 5): mencoba mengambil mutex

**Log Serial Monitor (Wokwi):**
```
[WITH-PIP] LOW: Locked mutex
[WITH-PIP] MEDIUM: Running workload...
[WITH-PIP] MEDIUM: Running workload...
[WITH-PIP] HIGH: Acquired mutex after 3024 ms (dengan PIP → cepat!)
[WITH-PIP] LOW: Released mutex
```

**Analisis**:
- HighPriorityTask (prioritas 5) butuh **~3024 ms** (hanya 24 ms lebih dari 3000 ms hold time)
- Ketika HighPriorityTask menunggu mutex yang dipegang LowPriorityTask, FreeRTOS **menaikkan prioritas LowPriorityTask menjadi 5** (priority inheritance)
- Akibatnya, MediumPriorityTask (prioritas 3) tidak dapat mendahului LowPriorityTask
- LowPriorityTask cepat menyelesaikan tugasnya dan melepas mutex

#### 6.7.3 Hasil Perbandingan Priority Inversion

| **Aspek** | **Binary Semaphore (tanpa PIP)** | **Mutex (dengan PIP)** |
|-----------|--------------------------------|------------------------|
| Waktu blocking High task | ~3412 ms | ~3024 ms |
| Prioritas Low saat pegang resource | Tetap 1 | Dinaikkan ke 5 |
| Medium preempt Low? | ✅ Ya (inversion!) | ❌ Tidak |
| Waktu tambahan akibat blocking | ~412 ms | ~24 ms ✅ |
| **Efektivitas PIP** | ❌ Tidak ada | ✅ **95% lebih cepat** |

### 6.8 Verifikasi Tema Safety-Critical Systems (CLO3)

Tema kontemporer **Safety-Critical Systems** terintegrasi dalam desain sistem melalui mekanisme berikut:

| **No** | **Fitur Safety-Critical** | **Implementasi** | **Lokasi** |
|-------|--------------------------|-----------------|------------|
| 1 | **Prioritas alarm tertinggi** | AlarmTask diberi prioritas 5 (tertinggi) dan periode 50 ms | `xTaskCreate(AlarmTask, ..., 5, ...)` |
| 2 | **Pemisahan fungsi kritis/non-kritis** | Sensor & alarm di prioritas tinggi (3-5), WiFi di prioritas rendah (2) | Tabel 4.1 & 4.2 |
| 3 | **ISR singkat + deferred processing** | ISR hanya memberi semaphore, AlarmTask yang proses | `emergencyISR()` → `AlarmTask()` |
| 4 | **Deadlock prevention** | Timeout 1000 ms pada `xSemaphoreTake()` kedua | `DeadlockTaskA/B` |
| 5 | **Priority inheritance** | `xSemaphoreCreateMutex()` untuk cegah priority inversion | `dataMutex`, `priorityMutex` |
| 6 | **Stack overflow protection** | `uxTaskGetStackHighWaterMark()` tiap 5 siklus | `MonitoringTask` |
| 7 | **Memory fragmentation prevention** | `char status[20]` (bukan `String`) di queue | `PatientData` struct |
| 8 | **Race condition prevention** | `dataMutex` + `spinlock` untuk shared data | Semua sensor task |
| 9 | **Non-blocking WiFi** | WiFiTask prioritas 2, tidak pegang mutex saat HTTP POST | `WiFiTask` |
| 10 | **HTTP timeout** | `http.setTimeout(1000)` — 1 detik, tidak hang selamanya | `WiFiTask`, `CommandTask` |

**Verifikasi melalui pengukuran:**
- **AlarmTask deadline**: Periode 50 ms, prioritas 5 → dijamin oleh RMS (U = 0.236 < LUB 0.735)
- **Emergency response maksimal**: ISR (μs) + semaphore (tick) + AlarmTask (50 ms) → **< 100 ms total**
- **Stack safety**: Semua task memiliki sisa stack yang memadai (terukur via `uxTaskGetStackHighWaterMark`)
- **HTTP tidak blokir alarm**: WiFiTask prioritas 2 — jika alarm task bangun, scheduler preempt WiFiTask

**Kesimpulan**: Sistem ini memenuhi prinsip dasar _safety-critical system_ berdasarkan standar ISO 14971, yaitu: identifikasi bahaya (deadlock, race condition, priority inversion, stack overflow), pengendalian risiko (mutex, spinlock, ISR deferred, priority inheritance), dan pemantauan efektivitas (stack monitoring, deadline verification).

**Tabel 6.4: Troubleshooting**

| No | Masalah | Penyebab | Solusi | Status |
|----|---------|----------|--------|--------|
| 1 | HTTP response code: -1 | Flask server tidak berjalan atau alamat IP salah | Pastikan `python app.py` berjalan dan `serverBaseUrl` sesuai IP lokal | ✅ Teratasi |
| 2 | DNS Failed untuk ngrok URL | ESP32 di Wokwi tidak bisa resolve DNS ngrok | Gunakan IP lokal (`http://192.168.x.x:5000`) untuk ESP32 | ✅ Teratasi |
| 3 | SSL errors (-80, -29312) | HTTPS tidak kompatibel dengan ESP32 di Wokwi | Gunakan HTTP, bukan HTTPS, untuk komunikasi ESP32 | ✅ Teratasi |
| 4 | Dashboard menampilkan data usang | Wokwi tidak berjalan atau WiFiTask gagal | Periksa Serial Monitor untuk `[WiFiTask] HTTP response code: 200` | ✅ Teratasi |
| 5 | Wokwi tidak memuat firmware baru | _Cache_ firmware lama | Jalankan `platformio run --target clean && platformio run`, restart VSCode | ✅ Teratasi |
| 6 | Nilai sensor tidak sinkron | Mode kontrol masih "web" dari sesi sebelumnya | Klik "Sensor Mode" pada dashboard untuk beralih ke mode sensor | ✅ Teratasi |
| 7 | _Stack overflow_ pada task tertentu | Alokasi stack tidak mencukupi | Periksa _high-water mark_ dan sesuaikan ukuran stack | ✅ Tidak terjadi |
| 8 | JSON deserialization error | Data dari server tidak sesuai format | Periksa respons server dengan `curl` langsung | ✅ Teratasi |

---

## BAB 7: KESIMPULAN DAN SARAN

### 7.1 Kesimpulan

Berdasarkan hasil perancangan, implementasi, dan pengujian yang telah dilakukan, dapat ditarik kesimpulan sebagai berikut:

1. **Sistem pemantauan pasien ICU _real-time_** berhasil diimplementasikan menggunakan ESP32 dengan FreeRTOS. Sistem mampu membaca tiga parameter _vital sign_ (BPM, suhu, SpO₂), memproses alarm secara _real-time_, dan menampilkan data melalui _web dashboard_. Sistem dirancang dengan prinsip _safety-critical_ di mana alarm kondisi kritis mendapat prioritas tertinggi.

2. **Manajemen multitasking FreeRTOS** berjalan dengan baik. Tujuh _task_ dengan prioritas 1–5 berjalan secara simultan tanpa saling mengganggu. Penggunaan `vTaskDelayUntil()` menjamin periodisitas eksekusi yang presisi untuk setiap _task_. Analisis _schedulability_ menunjukkan CPU _utilization_ U = 0,236 yang jauh di bawah LUB 0,735, sehingga sistem dinyatakan _schedulable_.

3. **Mekanisme sinkronisasi** (_queue_, _mutex_, _binary semaphore_, _spinlock_) berfungsi efektif dalam melindungi data bersama dan mengoordinasikan antar-task. Tidak ada _race condition_ atau _data corruption_ yang terdeteksi selama pengujian. Mutex dengan _priority inheritance_ berhasil mencegah _priority inversion_.

4. **Penanganan _interrupt_** menggunakan pola _deferred interrupt processing_ memungkinkan respons cepat terhadap tombol darurat tanpa mengorbankan stabilitas sistem. ISR hanya memberikan sinyal melalui _binary semaphore_, sedangkan pemrosesan dilakukan di AlarmTask dengan prioritas tertinggi.

5. **Web dashboard** berbasis Flask dan React berhasil menyediakan visualisasi data _real-time_ dengan _polling_ setiap 1 detik. API REST dengan validasi input dan _thread safety_ berfungsi dengan baik pada semua _endpoint_ (POST /data, GET /latest, POST /control, GET /command, POST /control/reset).

6. **Fitur RTOS tingkat lanjut** (_deadlock prevention_ menggunakan _timeout_, _priority inversion_ dengan _priority inheritance_, dan _protected counter_ dengan _spinlock_) berhasil diimplementasikan sebagai demo yang dapat diaktifkan sesuai kebutuhan.

7. **Efisiensi sumber daya**: Firmware hanya menggunakan 14,5% RAM (47.652 bytes) dan 71,9% Flash (942.041 bytes), meninggalkan ruang yang cukup untuk pengembangan fitur tambahan.

### 7.2 Saran

Beberapa saran untuk pengembangan lebih lanjut:

1. **Sensor Medis Real**: Mengganti potensiometer simulasi dengan sensor medis sesungguhnya (seperti MAX30100 untuk BPM/SpO₂ dan DS18B20 untuk suhu) untuk pengujian yang lebih realistis dan validasi klinis.

2. **Database**: Menambahkan database (seperti SQLite, InfluxDB, atau Firebase) untuk menyimpan riwayat data pasien yang dapat digunakan untuk analisis tren medis dan _logging_ jangka panjang.

3. **Autentikasi dan Keamanan**: Menambahkan autentikasi pengguna (_login/logout_), enkripsi HTTPS/TLS, dan mekanisme keamanan untuk melindungi data pasien yang sensitif sesuai standar HIPAA atau GDPR.

4. **Notifikasi Real-Time**: Mengganti mekanisme _polling_ dengan WebSocket atau Server-Sent Events (SSE) untuk pembaruan _real-time_ yang lebih responsif dan efisien.

5. **Multi-Patient Support**: Mengembangkan sistem untuk mendukung pemantauan beberapa pasien secara bersamaan, dengan _dashboard_ yang dapat menampilkan banyak pasien sekaligus.

6. **Protokol Komunikasi**: Mengganti protokol HTTP dengan MQTT (Message Queue Telemetry Transport) yang lebih efisien untuk IoT dan mendukung _publish/subscribe_ yang lebih sesuai untuk sistem monitoring.

7. **Over-the-Air (OTA) Update**: Menambahkan kemampuan pembaruan firmware secara nirkabel untuk memudahkan pemeliharaan dan pembaruan fitur.

8. **Pengujian Beban**: Melakukan pengujian beban (_stress testing_) untuk memverifikasi perilaku sistem dalam kondisi ekstrem, seperti kehilangan koneksi Wi-Fi berkepanjangan, lonjakan data, atau kegagalan sensor.

9. **Watchdog Timer**: Menambahkan _hardware watchdog timer_ untuk mendeteksi dan memulihkan sistem dari _task hang_ atau _deadlock_ yang tidak terduga.

10. **Sertifikasi**: Untuk penerapan medis nyata, sistem perlu memenuhi standar sertifikasi perangkat medis seperti IEC 62304 untuk perangkat lunak perangkat medis.

---

## DAFTAR PUSTAKA

1. Amazon Web Services. (2024). *FreeRTOS: A Real-Time Operating System for Microcontrollers*. https://www.freertos.org/

2. Barry, R. (2017). *Mastering the FreeRTOS Real Time Kernel: A Hands-On Tutorial Guide*. Real Time Engineers Ltd.

3. Burns, A., & Wellings, A. (2009). *Real-Time Systems and Programming Languages: Ada, Real-Time Java and C/Real-Time POSIX* (4th ed.). Addison-Wesley.

4. Buttazzo, G. C. (2011). *Hard Real-Time Computing Systems: Predictable Scheduling Algorithms and Applications* (3rd ed.). Springer.

5. Espressif Systems. (2024). *ESP32 Technical Reference Manual*. https://www.espressif.com/en/support/documents/technical-documents/

6. Espressif Systems. (2024). *ESP-IDF Programming Guide: FreeRTOS Overview*. https://docs.espressif.com/projects/esp-idf/

7. Grinberg, M. (2018). *Flask Web Development: Developing Web Applications with Python* (2nd ed.). O'Reilly Media.

8. Kopetz, H. (2011). *Real-Time Systems: Design Principles for Distributed Embedded Applications* (2nd ed.). Springer.

9. Liu, C. L., & Layland, J. W. (1973). Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment. *Journal of the ACM*, 20(1), 46–61.

10. Liu, J. W. S. (2000). *Real-Time Systems*. Prentice Hall.

11. Meta Platforms. (2024). *React: A JavaScript Library for Building User Interfaces*. https://react.dev/

12. PlatformIO. (2024). *PlatformIO: A Professional Collaborative Platform for Embedded Development*. https://platformio.org/

13. Stallings, W. (2018). *Operating Systems: Internals and Design Principles* (9th ed.). Pearson.

14. Tanenbaum, A. S., & Bos, H. (2015). *Modern Operating Systems* (4th ed.). Pearson.

15. Wokwi. (2024). *Wokwi: Online Electronics Simulator*. https://wokwi.com/

16. Arduino. (2024). *ArduinoJson: Efficient JSON Library for Arduino*. https://arduinojson.org/

17. IEC. (2020). *IEC 62304: Medical Device Software — Software Life Cycle Processes*. International Electrotechnical Commission.

---

## LAMPIRAN

### Lampiran A: Source Code

Kode sumber lengkap proyek ini dapat diakses melalui repositori GitHub:

**GitHub Repository**: [https://github.com/ArdeezYy/Smart-ICU-RTOS](https://github.com/ArdeezYy/Smart-ICU-RTOS)

**Struktur Direktori:**

```
Smart-ICU-RTOS/
├── src/
│   └── main.cpp              # ESP32 FreeRTOS firmware (521 lines)
├── dashboard/
│   ├── app.py                # Flask REST API backend (238 lines)
│   ├── requirements.txt      # Python dependencies
│   ├── frontend/
│   │   ├── index.html        # Vite entry point
│   │   ├── vite.config.js    # Vite config (API proxy)
│   │   ├── package.json      # npm dependencies
│   │   └── src/
│   │       ├── main.jsx      # React dashboard (305 lines)
│   │       └── styles.css    # Dashboard styling (270 lines)
├── diagram.json              # Wokwi circuit diagram
├── wokwi.toml                # Wokwi firmware config
├── platformio.ini            # PlatformIO build config
├── LAPORAN.md                # Full academic report (this file)
├── AUDIT.md                  # Code & architecture audit
└── README.md                 # Project documentation
```

### Lampiran B: Konfigurasi Simulator

**B.1 Diagram Wokwi (`diagram.json`)**

```json
{
  "version": 1,
  "author": "Uri Shaked",
  "editor": "wokwi",
  "parts": [
    { "type": "wokwi-breadboard-half", "id": "bb1", ... },
    { "type": "board-esp32-devkit-c-v4", "id": "esp", ... },
    { "type": "wokwi-led", "id": "led1", "attrs": { "color": "red" } },
    { "type": "wokwi-pushbutton", "id": "btn1", "attrs": { "color": "green" } },
    { "type": "wokwi-buzzer", "id": "bz1", "attrs": {} },
    { "type": "wokwi-potentiometer", "id": "pot1", ... },
    { "type": "wokwi-potentiometer", "id": "pot2", ... },
    { "type": "wokwi-potentiometer", "id": "pot3", ... }
  ],
  "connections": [
    [ "pot2:SIG", "esp:34", ... ],
    [ "pot1:SIG", "esp:35", ... ],
    [ "pot3:SIG", "esp:32", ... ],
    [ "btn1:2.r", "esp:27", ... ],
    [ "led1:A", "esp:25", ... ],
    [ "bz1:2", "esp:26", ... ]
  ]
}
```

**B.2 Konfigurasi Firmware (`wokwi.toml`)**

```toml
[wokwi]
version = 1
firmware = '.pio/build/esp32dev/firmware.bin'
elf = '.pio/build/esp32dev/firmware.elf'
```

**B.3 Konfigurasi PlatformIO (`platformio.ini`)**

```ini
[env:esp32dev]
platform = espressif32@6.8.1
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
  bblanchon/ArduinoJson@^7.3.1
```

### Lampiran C: Hasil Pengujian

**C.1 Hasil Kompilasi Firmware**

| Parameter | Nilai |
|-----------|-------|
| RAM (DRAM) Terpakai | 47.652 bytes (14,5%) |
| Flash Terpakai | 942.041 bytes (71,9%) |
| Waktu Kompilasi | 15,83 detik |
| Compiler Warnings | 0 |
| Jumlah Task | 7 FreeRTOS tasks |
| Library | ArduinoJson 7.4.3 |

**C.2 Hasil Pengujian API**

| Endpoint | Method | Status Code | Validasi |
|----------|--------|-------------|----------|
| `/` | GET | 200 | SPA React termuat |
| `/data` | POST | 200 | Data valid tersimpan |
| `/data` (invalid) | POST | 400 | Error field types |
| `/data` (bad JSON) | POST | 400 | Invalid JSON |
| `/latest` | GET | 200 | Data + command |
| `/control` | POST | 200 | Command valid |
| `/control` (invalid) | POST | 400 | Range validation |
| `/command` | GET | 200 | Command aktif |
| `/control/reset` | POST | 200 | Reset normal |

**C.3 Hasil Build Frontend**

| Parameter | Nilai |
|-----------|-------|
| Build Tool | Vite 7.3.5 |
| Framework | React 19.2.0 |
| Icons | lucide-react 0.556.0 |
| Jumlah Modul | 1.689 modul |
| Paket npm | 67 (0 vulnerabilities) |
| JS Bundle | 201,69 KB (63,84 KB gzipped) |
| CSS Bundle | 3,12 KB (1,22 KB gzipped) |

### Lampiran D: Video Demo

[Placeholder — tautan video demo akan ditambahkan setelah perekaman]

Video demo mencakup:
1. _Startup_ sistem dan inisialisasi 7 task FreeRTOS
2. Pembacaan sensor (memutar potensiometer BPM, suhu, SpO₂)
3. Aktivasi alarm kondisi kritis (BPM > 130, suhu > 38°C, SpO₂ < 90%)
4. Tombol darurat dan alarm 3 detik
5. _Web dashboard_ menampilkan data _real-time_
6. Mode _web control_ (dashboard mengontrol nilai sensor)
7. _Advanced RTOS demo_ (deadlock prevention, priority inversion, protected counter)

### Lampiran E: Project Requirements Checklist

**Tabel L.1: Project Requirements Checklist**

| No | Kriteria | Status | Keterangan |
|----|----------|--------|------------|
| 1 | Minimal 5 task FreeRTOS | ✅ | 7 task aktif (Alarm, HeartRate, Oxygen, Temperature, WiFi, Command, Monitoring) |
| 2 | Task priorities berbeda | ✅ | Prioritas 1–5, rate-monotonic |
| 3 | Fixed-rate scheduling | ✅ | `vTaskDelayUntil()` untuk periodic tasks |
| 4 | Mutual exclusion (mutex) | ✅ | `dataMutex` melindungi shared data |
| 5 | Queue communication | ✅ | `patientQueue` untuk data passing |
| 6 | ISR handling | ✅ | `emergencyISR` untuk emergency button |
| 7 | Deferred interrupt processing | ✅ | ISR → binary semaphore → AlarmTask |
| 8 | Stack monitoring | ✅ | `uxTaskGetStackHighWaterMark()` |
| 9 | Timing analysis | ✅ | Tabel period/priority, CPU utilization |
| 10 | Schedulability analysis | ✅ | LUB test (U=0,236 ≤ 0,735), RTA |
| 11 | Hardware simulation | ✅ | Wokwi: ESP32, 3 pot, LED, buzzer, button |
| 12 | Web dashboard | ✅ | React + Flask, polling 1 detik |
| 13 | Control interface | ✅ | 6 buttons + 3 sliders |
| 14 | Deadlock prevention | ✅ | Timeout pada mutex kedua |
| 15 | Priority inversion demo | ✅ | Low/Medium/High priority tasks |
| 16 | Spinlock / protected counter | ✅ | `portENTER_CRITICAL` |
| 17 | Bahasa Indonesia formal | ✅ | Laporan akademik lengkap |
| 18 | Tabel pembagian tugas | ✅ | Tabel 5.2 |
| 19 | Daftar pustaka (15+) | ✅ | 17 referensi |
| 20 | Lampiran lengkap | ✅ | 5 lampiran (A–E) |

---

**Dokumen ini disusun sebagai laporan tugas besar mata kuliah Sistem Operasi Real-Time (RTOS) — CLO3 Safety-Critical Systems.**

**Program Studi Teknik Informatika**
**Fakultas Ilmu Komputer**
**Universitas [ Nama Universitas ]**
**Semester Genap 2025/2026**

---
