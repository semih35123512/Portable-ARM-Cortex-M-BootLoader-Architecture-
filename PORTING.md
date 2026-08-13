# PORTING — başka MCU / STM32F4

## Kısa cevap

| Soru | Durum |
|------|--------|
| Yan proje şimdi hangi MCU'da ayağa kalkar? | **HC32F460** (Keil + `platforms/hc32f460` + public board) |
| STM32F4 / başka Cortex-M için iskelet var mı? | **Evet** (`platforms/user_template`, `platforms/stm32f4`, `platforms/stm32_template`) |
| Kutu aç-çalıştır STM32F4 mı? | **Hayır** — port doldurulmalı, cihaz/Keil-Cube hedefi değiştirilmeli |

Çekirdek (`boot_core` / `boot_storage` / `boot_meta` / stub mantığı) MCU bağımsız.  
SPI + iç flash + CRC + reset **port katmanında** sizin doldurmanız gerekir.

---

## Mevcut altyapı

```
BootRAM_core          → hazır (değiştirme)
BootStub              → portable (BOOT_SRAM_* + auto CMSIS header)
platforms/hc32f460    → public referans (board/ + meta hooks; OSOS yok)
platforms/stm32f4     → HAL örnek (BOOT_MCU_STM32F4_GENERIC) — Keil grubunda yok, kaynak var
platforms/stm32_template / user_template → TODO iskelet
private/osos_product  → ürün zone FS (gitignore; GitHub'a koyma)
```

Keil yan proje şu an: `BOOT_MCU_HC32F460` + HC32 Driver path.

---

## STM32F4 için yapılacaklar

1. **Yeni Keil/Cube hedef**  
   Device = sizin STM32F4; HC32 pack/define kaldırın (`HC32F460`, `USE_DDL_DRIVER`).

2. **MCU seçimi**  
   - `BOOT_MCU_STM32F4_GENERIC` + `STM32F401xE` / `STM32F407xx` / `STM32F446xx` (birini)  
   - veya herhangi MCU: `BOOT_MCU_USER`

3. **Build dosyaları**  
   - HC32 grubunu (`platform_hc32f460`, `platform_hc32f460_board`) **Exclude from build**  
   - Ekleyin: `platforms/stm32f4/*.c` **veya** `platforms/user_template/*.c`  
   - Core: `boot_core`, `boot_storage`, `boot_meta`, `boot_board`, stub  
   - HAL: `stm32f4xx_hal*.c` + sizin `system_stm32f4xx.c` / startup

4. **`boot_target_config.h`** + **`boot_config.h`**  
   Memory map (`platforms/stm32f4/boot_target_config.h`):
   ```c
   #define BOOT_STUB_FLASH_START   0x08000000UL
   #define BOOT_META_FLASH_ADDR    0x08002000UL
   #define BOOT_APP_FLASH_START    0x08004000UL
   #define BOOT_APP_FLASH_END      0x08080000UL  /* chip'e göre */
   #define BOOT_SRAM_START         0x20000000UL
   #define BOOT_SRAM_END           0x20020000UL
   #define BOOT_SPI_FW_MODE        BOOT_SPI_FW_MODE_FIXED  /* yan proje MODE2 */
   ```

5. **Port doldur** (`boot_port_*`)  
   - SPI NOR read (blocking)  
   - APP flash erase/program  
   - CRC32  
   - WDT feed + `NVIC_SystemReset`  
   - `boot_port_meta_erase_page` / `boot_port_meta_program` (meta sayfa)

6. **Stub taşınabilirliği (artık otomatik)**  
   `boot_stub.c` `BOOT_SRAM_START` / `BOOT_SRAM_END` makrolarını `boot_config.h`'den okur.  
   CMSIS device header `BOOT_MCU_*` define'ına göre otomatik seçilir.  
   Farklı bir header gerekirse `BOOT_CMSIS_DEVICE_HEADER` ile override edin:
   ```c
   #define BOOT_CMSIS_DEVICE_HEADER "stm32l4xx.h"
   ```

7. **Scatter / linker**  
   Platform scatter: `BootRAM/platforms/hc32f460/bootloader.sct` (HC32) veya  
   `platforms/stm32f4/bootloader.sct` / `platforms/generic/bootloader.sct`.  
   Stub @ flash base, meta ayrı sayfa, APP offset = `BOOT_APP_FLASH_START`.

8. **SPI imaj**  
   MODE2: CAND/GOOD sabit sektör pencerelerine binary'yi siz yazmış olmalısınız (staging bu pakette yok).

9. **APP tarafı**  
   Sağlıklı boot sonrası: `boot_fw_on_app_healthy()` (STM32 için zone/commit'i siz yazar veya MODE2 kopya).

---

## "Herhangi işlemci" için minimum

`BOOT_MCU_USER` + `platforms/user_template` TODO'ları + `boot_config.h`'te SRAM aralığı + scatter.  
`stm32f4` klasörü sadece F4 HAL örneği; başka aile için `user_template` kopyalayın.

---

## GitHub kullanıcısına net mesaj

- Bu repo **HC32F460 referans + taşınabilir IAP çekirdeği** gösterir.  
- STM32F4'te ayağa kaldırmak **mümkün**, ama "kendi Cube projesine port impl + stub + flash map" işi kullanıcıya ait.  
- Hazır "STM32F4 click-to-run" firmware yok.
