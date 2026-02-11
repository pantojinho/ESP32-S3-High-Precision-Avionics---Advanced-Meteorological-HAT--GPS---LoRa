# PINMAP - Waveshare Base + HAT v1

Last update: 2026-02-11
Status: LOCKED for v1 baseline

## PT-BR

Esta tabela e a fonte oficial para firmware e hardware da HAT v1.
Se `src/config/pins.h` e este arquivo divergirem, o correto e atualizar os dois no mesmo commit.

## EN

This table is the official source for firmware and hardware on HAT v1.
If `src/config/pins.h` differs from this file, both must be updated in the same commit.

## Base board (Waveshare ESP32-S3 Touch AMOLED 1.64)

| Function | GPIO | Notes |
| --- | --- | --- |
| I2C SDA | GPIO47 | Shared sensor bus |
| I2C SCL | GPIO48 | Shared sensor bus |
| IMU INT | GPIO46 | On-board QMI8658 interrupt |
| GPS RX | GPIO44 | ESP RX <- GPS TX |
| GPS TX | GPIO43 | ESP TX -> GPS RX |
| SD CS | GPIO38 | Native TF slot |
| SD MOSI | GPIO39 | Native TF slot |
| SD MISO | GPIO40 | Native TF slot |
| SD SCK | GPIO41 | Native TF slot |

## HAT v1 (dedicated LoRa bus)

| Function | GPIO | Notes |
| --- | --- | --- |
| LoRa SCK | GPIO7 | Dedicated to SX1262/CC68 |
| LoRa MOSI | GPIO6 | Dedicated to SX1262/CC68 |
| LoRa MISO | GPIO5 | Dedicated to SX1262/CC68 |
| LoRa NSS | GPIO15 | Chip select |
| LoRa DIO1 | GPIO17 | IRQ/event |
| LoRa BUSY | GPIO18 | Radio busy line |
| LoRa RST | GPIO16 | Radio reset |

## GPS external connector (1x6, 2.54 mm)

| Pin | Signal | Notes |
| --- | --- | --- |
| 1 | 3V3 | Preferred supply for AliExpress GPS modules |
| 2 | GND | Common ground |
| 3 | TXD | GPS TX -> ESP RX (GPIO44) |
| 4 | RXD | GPS RX <- ESP TX (GPIO43) |
| 5 | NC | Reserved |
| 6 | NC | Reserved |

## Rules

1. Keep SD lines untouched (native baseboard function).
2. LoRa must stay on dedicated SPI lines to avoid SD/display conflicts.
3. Any pin change requires update in:
- `src/config/pins.h`
- `docs/hardware/PINMAP.md`
- `docs/project/PROJECT_MEMORY.md`
