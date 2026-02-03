# BSEC Notes (BME688 IAQ)

BSEC (Bosch) provides IAQ and VOC processing for BME688.
On Arduino, it is available as a binary library with example code.
On ESP-IDF, integration can be more complex because of binary blobs.

Plan:
- Start without IAQ (raw gas resistance only)
- Add BSEC when the rest of the system is stable
- Keep IAQ calculation in a separate task at low rate (1 Hz)

If IAQ is required early, consider isolating it in ESP-IDF and sharing
results over a small IPC or via UART to the Arduino app.
