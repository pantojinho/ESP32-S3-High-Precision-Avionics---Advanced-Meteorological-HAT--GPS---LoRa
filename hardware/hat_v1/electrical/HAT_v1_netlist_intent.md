# HAT v1 Electrical Intent

## Major blocks

1. LoRa transceiver block (SX1262/CC68)
2. Flight sensors: QMI8658 + BMP581
3. Meteo sensor: BME688
4. GPS external connector
5. Expansion header (I2C/UART)

## Bus strategy

1. I2C:
- shared on GPIO47/48 for sensors

2. UART:
- GPS on GPIO44/43

3. SPI:
- dedicated LoRa SPI on GPIO7/6/5

## Power strategy

1. Main rail from baseboard 3V3/BAT.
2. Local decoupling per IC.
3. RF and digital grounds managed to reduce noise coupling.
