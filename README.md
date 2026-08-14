# Portable ARM Cortex-M Bootloader with SPI NOR Staging and Rollback

A portable ARM Cortex-M bootloader / IAP architecture designed for reliable firmware updates using external SPI NOR Flash.

The project separates the MCU-independent bootloader core from target-specific hardware implementations through a platform abstraction layer.

The main design goal is to make the firmware update state machine, metadata handling, firmware slot management and rollback mechanism reusable across ARM Cortex-M targets, while MCU-specific Flash, SPI, CRC, watchdog, reset and memory-map operations remain inside the target platform layer.

> This project provides a portable Cortex-M bootloader architecture rather than a universal preconfigured binary for every Cortex-M MCU. Each target requires its own hardware port, memory map and linker configuration.


## Architecture

The bootloader is organized into three main layers:

```text
                    Application
                         │
                         │ BOOT_OK
                         ▼
              ┌─────────────────────┐
              │     Boot Stub       │
              │                     │
              │ Metadata validation │
              │ Rollback decision   │
              │ Vector validation   │
              │ Application jump    │
              └──────────┬──────────┘
                         │
                         ▼
              ┌─────────────────────┐
              │     Boot Core       │
              │                     │
              │ Firmware selection  │
              │ CRC verification    │
              │ Flash programming   │
              │ Metadata handling   │
              └──────────┬──────────┘
                         │
                         ▼
              ┌─────────────────────┐
              │ Platform Abstraction│
              │                     │
              │ SPI                 │
              │ Internal Flash      │
              │ CRC                 │
              │ Watchdog            │
              │ Reset               │
              │ Target configuration│
              └──────────┬──────────┘
                         │
             ┌───────────┼────────────┐
             ▼           ▼            ▼
          HC32F460     STM32F4     User Target
		  
		  
		  ## What is Portable?

The following components are designed to remain independent from a specific MCU:

- Bootloader core logic
- Firmware update state machine
- Firmware slot management
- Persistent boot metadata
- Candidate / Good firmware handling
- CRC-based firmware validation
- Boot attempt tracking
- Rollback decision logic
- SPI firmware storage abstraction
- Boot/application handover logic
- Generic Cortex-M porting interfaces

A new Cortex-M target should be able to reuse the core logic while implementing its own platform layer.

## What is Portable?

The following components are designed to remain independent from a specific MCU:

- Bootloader core logic
- Firmware update state machine
- Firmware slot management
- Persistent boot metadata
- Candidate / Good firmware handling
- CRC-based firmware validation
- Boot attempt tracking
- Rollback decision logic
- SPI firmware storage abstraction
- Boot/application handover logic
- Generic Cortex-M porting interfaces

A new Cortex-M target should be able to reuse the core logic while implementing its own platform layer.

## Firmware Update Flow

The firmware update process follows a Candidate → Programmed → BOOT_OK → Good flow.

```text
Firmware Download
       │
       ▼
External SPI NOR
       │
       ▼
   CANDIDATE
       │
       │ Size + CRC validation
       ▼
Boot Core
       │
       ├── Erase application
       ├── Program application
       └── Verify programmed image
       │
       ▼
   PROGRAMMED
       │
       ▼
      RESET
       │
       ▼
   Boot Stub
       │
       ├── Validate metadata
       ├── Detect interrupted update
       ├── Check boot attempts
       ├── Roll back if required
       └── Validate application vector
       │
       ▼
   Application
       │
       │ Successful startup
       ▼
     BOOT_OK
       │
       ▼
 CANDIDATE → GOOD
 
 
 # Reliability / Rollback

```markdown
## Reliability and Rollback

The bootloader is designed to prevent an interrupted or unhealthy firmware update from permanently replacing the last known-good firmware.

The update state is persisted in boot metadata.

A simplified failure scenario:

```text
CANDIDATE
    │
    ▼
PROGRAMMED
    │
    ▼
RESET
    │
    ▼
Application starts
    │
    ├── BOOT_OK ───────► CANDIDATE → GOOD
    │
    └── Boot failure
            │
            ▼
      Boot attempt count
            │
            ▼
      Restore GOOD
	  
	  # Current implementations

```markdown
## Current Reference Implementations

### HC32F460

The HC32F460 implementation is the current reference target.

It demonstrates the complete platform integration including:

- SPI NOR access
- Internal Flash programming
- CRC
- Watchdog
- Reset
- Target memory configuration
- Boot/application layout
- Firmware storage

### STM32F4

An STM32F4 implementation is provided as a reference/example port.

The memory map and target configuration must be adapted to the specific STM32F4 device.

### Generic / User Template

The generic and user-template platforms provide a starting point for integrating the bootloader into another Cortex-M MCU.

The target-specific platform layer should implement the required hardware operations without modifying the portable bootloader core.


## Current Limitations

- The current fully integrated reference target is HC32F460.
- STM32F4 is provided as a reference/example port.
- New MCUs require a target-specific memory map and linker configuration.
- CRC32 provides data integrity checking but not cryptographic authenticity.
- Secure Boot is not currently implemented.
- Firmware signature verification is not currently implemented.
- Anti-rollback based on cryptographic version protection is not currently implemented.

## Security Considerations

The current implementation focuses primarily on firmware integrity and update reliability.

CRC32 can detect accidental corruption but does not provide cryptographic authenticity.

A production secure firmware update system should additionally consider:

- SHA-256 image hashing
- Digital signatures
- ECDSA / RSA verification
- Secure key storage
- Anti-rollback protection
- Hardware Root of Trust
- Debug interface protection

These are planned extensions rather than current features.

## Roadmap

- [ ] Host-side bootloader state-machine tests
- [ ] Power-loss / failure-injection testing
- [ ] Automated firmware image packaging
- [ ] SHA-256 image verification
- [ ] Digital signature verification
- [ ] Anti-rollback protection
- [ ] Additional Cortex-M reference ports
- [ ] CI build validation