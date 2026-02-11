# HAT v1 Requirements

Last update: 2026-02-11

## Product objective (PT-BR)

Criar uma HAT PCB que encaixa por baixo da Waveshare ESP32-S3 Touch AMOLED 1.64,
replicavel por qualquer pessoa via fabricacao na China (PCB + montagem opcional).

## Product objective (EN)

Build a bottom-mounted HAT PCB for the Waveshare ESP32-S3 Touch AMOLED 1.64,
replicable by anyone via China manufacturing services.

## Locked mandatory modules (v1)

1. LoRa SX1262/CC68 at 915 MHz
2. QMI8658 IMU (attitude baseline)
3. BMP581 barometer (altitude/vario)
4. BME688 environmental sensor
5. External GPS connector (1x6, 2.54 mm)
6. I2C/UART expansion header

## Deferred to v2

1. LoRa mesh routing
2. Dedicated repeater mode
3. Non-GNSS satellite receive scenarios
4. Optional BMM350/LTR-390 depending on area/cost

## Mechanical constraints

1. Sandwich mounting with 4x M2 standoffs.
2. Keepout for USB-C, buttons, SD slot, and RF antenna zones.
3. Header alignment must match Waveshare side headers.

## Electrical constraints

1. Power from Waveshare BAT/3V3.
2. No conflict with native SD lines on the base board.
3. Dedicated LoRa SPI on HAT.

## Acceptance checklist

1. PINMAP and firmware pins are consistent.
2. ERC/DRC have no critical errors.
3. Prototype can run tracker, flight basic, and balloon mode.
