# cortexm-spi-bootloader

Cortex-M için **SPI NOR üzerinden FOTA / IAP** bootloader iskeleti.  
Referans port: **HDSC HC32F460** (Keil µVision). Çekirdek MCU bağımsız; STM32 ve diğer hedefler için port şablonları mevcut.

SPI’ye stage edilmiş firmware imajını CRC ile doğrular, uygulama flash’ını siler/programlar; kritik yol **RAM’den** çalışır. Reset stub, başarısız veya yarım kalan güncellemede **GOOD** imaja rollback yapar.

---

## Özellikler

- **MODE2 FIXED** SPI pencereleri: CANDIDATE + GOOD sektör aralıkları (ham binary)
- **MODE1 ZONE** (opsiyonel): bağlı sektör / zone FS okuma
- **boot_meta**: güncelleme durumu, cand/good size–CRC–startpoint (APP erase’ten bağımsız sayfa)
- **Boot stub @0x0**: meta kontrolü, GOOD restore, APP jump (`VTOR` / `0x4000`)
- **RAM IAP**: `BOOT_RAM_FUNC` ile erase/program sırasında flash’tan bağımsız yürütme
- Port katmanı: SPI, iç flash, CRC32, WDT, reset — `platforms/<mcu>/`

---

## Mimari

```text
                    ┌─────────────────┐
   FOTA download ──►│ SPI CAND window │
                    └────────┬────────┘
                             │
 APP: RAM_ApplySpiFirmware ────┤  meta: cand_size / payload_start / cand_crc
                             ▼
                    ┌─────────────────┐
                    │   boot_core     │  CRC verify → erase APP → program
                    │  (from RAM)     │
                    └────────┬────────┘
                             │ reset
                             ▼
                    ┌─────────────────┐
   Reset @0 ───────►│   boot_stub     │  meta / GOOD rollback / jump APP
                    └─────────────────┘
                             │
                             ▼  healthy boot
                    boot_fw_on_app_healthy() → BOOT_OK + CAND→GOOD promote
```

| Katman | Klasör / dosya | Rol |
|--------|----------------|-----|
| Config | `BootRAM/boot_config.h` | Flash haritası, SPI modu, sektör pencereleri |
| Core | `BootRAM/boot_core.c` | MCU bağımsız IAP döngüsü |
| Storage | `BootRAM/boot_storage.c` | SPI’den imaj okuma (ZONE / FIXED) |
| Meta | `BootRAM/boot_meta.*` | Kalıcı IAP bayrakları |
| Stub | `BootStub/` | Reset vektörü, rollback, APP jump |
| Port | `BootRAM/platforms/*` | SPI / EFM / CRC / WDT |
| Facade | `BootRAM/BootRAM.c` | `RAM_ApplySpiFirmware(...)` |
| Keil | `MDK-ARM/` | HC32F460 demo hedefi |

Public API: `BootRAM/boot_api.h`

---

## Flash haritası (HC32 referans)

| Bölge | Adres | Boyut |
|-------|--------|------|
| Boot stub + IAP load | `0x00000000` … `0x00001FFF` | 8 KB |
| **boot_meta** | `0x00002000` … `0x00003FFF` | 8 KB |
| **APP** | `0x00004000` … `0x00077FFF` | 464 KB |
| EEPROM emülatör | `0x00078000` … | değişmez |

APP vektör tablosu: `VECT_TAB_OFFSET = 0x4000` (APP projesinde aynı olmalı).

---

## SPI firmware pencereleri (MODE2)

Varsayılan (4 MB / 1024 × 4 KB sektör, 1-based):

| Slot | Sektörler | Kapasite |
|------|-----------|----------|
| **CANDIDATE** | 769 … 896 | 512 KB |
| **GOOD** | 897 … 1024 | 512 KB |

OTA indirme, ham `.bin` dosyasını **CANDIDATE** penceresine yazar. Aralığı `boot_config.h` içinde değiştirin; çakışma olmamalı.

---

## Kullanım (APP tarafı)

SPI’ye imaj yazıldıktan sonra:

```c
#include "boot_api.h"
#include "BootRAM.h"

/* image_size, payload_start, image_crc: FOTA metadata */
RAM_ApplySpiFirmware(image_size,
                   payload_start,
                   image_crc,
                   BOOT_FW_SLOT_CANDIDATE);
```

`RAM_ApplySpiFirmware`:

1. Meta’ya `cand_size`, `cand_payload_start`, `cand_crc` (+ start node) yazar  
2. CRC doğrulama → APP erase → program (RAM)  
3. Başarıda `PROGRAMMED` + reset; stub APP’in `boot_ok` onayını bekler  

Sağlıklı APP açılışında: `boot_fw_on_app_healthy()` (BOOT_OK + CAND→GOOD).

Örnek: [`examples/example_after_spi_staged.c`](examples/example_after_spi_staged.c)

---

## Derleme (Keil µVision)

1. `MDK-ARM/cortexm-spi-bootloader.uvprojx` dosyasını açın  
2. Device pack: **HDSC.HC32F460**  
3. Compiler define (hedefte hazır):

   `HC32F460`, `USE_DDL_DRIVER`, `BOOT_MCU_HC32F460`, `BOOT_SPI_FW_MODE=2`

4. **CMSIS + HC32 LL Driver** include yolu (proje ayarı):

   ```text
   ../../Driver/cmsis/Include
   ../../Driver/cmsis/Device/HDSC/hc32f4xx/Include
   ../../Driver/hc32_ll_driver/inc
   ```

   Bu yollar `MDK-ARM`’a göre çözülür → repo’nun **üst dizininde** `Driver/` beklenir:

   ```text
   Parent/
     Driver/                 ← HC32 LL + CMSIS (ana DCU paketinden)
     cortexm-spi-bootloader/
       MDK-ARM/
       BootRAM/
       BootStub/
   ```

   `Driver` yoksa derleme `hc32_ll.h` / `core_cm4.h` bulamaz. Ana OSOS DCU projesindeki `Driver` klasörünü bu konuma koyun veya junction/symlink oluşturun.

5. Scatter: `BootRAM/platforms/hc32f460/bootloader.sct`

---

## Depo yapısı

```text
cortexm-spi-bootloader/
├── BootRAM/                 # portable IAP core + platforms
│   ├── platforms/
│   │   ├── hc32f460/        # public HC32 reference
│   │   │   ├── board/       # pins + SPI NOR macros (not product FS)
│   │   │   └── bootloader.sct
│   │   ├── stm32f4/
│   │   ├── generic/
│   │   └── user_template/
│   ├── boot_api.h
│   └── boot_config.h
├── BootStub/
├── examples/
├── MDK-ARM/
├── .gitignore               # Objects/, *.axf, private/, …
├── PORTING.md
└── README.md
```

**Publish etmeyin:** `private/` (gitignore) — OSOS zone FS / ürün `ososFlash` / `ososRead` glue.  
Public HC32 yolu meta + MODE2 sabit SPI pencereleri kullanır; ürün zone FS ayrı private mirror’da kalmalıdır.

---

## Başka MCU (STM32 / custom)

Çekirdek (`boot_core`, `boot_storage`, `boot_meta`) taşınabilir.  
Keil demo hedefi şu an **yalnızca HC32F460**.

Adımlar: **[PORTING.md](PORTING.md)**  
Şablon: `BootRAM/platforms/user_template/`

Özet:

1. `BOOT_MCU_*` seçin, HC32 gruplarını build’den çıkarın  
2. `boot_port_*` / hooks doldurun (SPI, flash, CRC, reset)  
3. Flash haritasını ve scatter’ı güncelleyin  
4. APP `VECT_TAB_OFFSET` = `BOOT_APP_FLASH_START`

---

## Meta durumları

| State | Anlam |
|-------|--------|
| `IDLE` | Normal çalışma |
| `UPDATING` | Erase/program sürüyor (reset = kesinti) |
| `PROGRAMMED` | Yazıldı; APP `boot_ok` bekleniyor |
| `FAIL` | Açık hata kilidi → GOOD restore |

Stub, `PROGRAMMED` ve `boot_ok==0` iken deneme sayısına göre GOOD’dan geri yükler.

---

## Dokümantasyon

| Dosya | İçerik |
|-------|--------|
| [PORTING.md](PORTING.md) | MCU taşıma adımları |
| [BootRAM/README.md](BootRAM/README.md) | IAP katmanları |
| [BootStub/README.md](BootStub/README.md) | Stub, MODE1/2, flash map |

---

## Notlar

- Bu depo ana DCU `Core/Boot*` ürün kodundan **bağımsız** bir yan / demo kopyadır; ana projeyi değiştirmez.  
- MODE2’de OTA staging: ham binary → CANDIDATE sektör penceresi.  
- `cand_*/...` gibi ifadeleri C yorumlarında kullanmayın (`*/` yorumu erken kapatır).
