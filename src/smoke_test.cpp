#include <Arduino.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

// Reuse pin definitions from config
// Note: We redefine them here locally to keep this file self-contained and "smoke-test" safe
// without dragging in the whole project dependency tree.

// I2C
#define I2C_SDA 47
#define I2C_SCL 48

// SD Card
#define SD_CS   38
#define SD_MOSI 39
#define SD_MISO 40
#define SD_SCK  41

// GPS UART
#define GPS_RX 44
#define GPS_TX 43

HardwareSerial Serial1(1);

static void scanI2C() {
  Serial.println(">>> Scanning I2C Bus...");
  uint8_t count = 0;
  for (uint8_t i = 1; i < 127; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found device at 0x");
      if (i < 16) Serial.print("0");
      Serial.println(i, HEX);
      count++;
    }
  }
  if (count == 0) Serial.println("No I2C devices found");
  else Serial.printf("Found %d I2C devices\n", count);
}

static void testSD() {
  Serial.println(">>> Testing SD Card...");
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS, SPI)) {
    Serial.println("SD Init Failed! Check card insertion or formatting.");
  } else {
    uint64_t totalBytes = SD.totalBytes();
    uint64_t usedBytes = SD.usedBytes();
    Serial.printf("SD Init OK! Size: %.2f GB\n", totalBytes / (1024.0 * 1024.0 * 1024.0));
    Serial.printf("Used: %.2f MB\n", usedBytes / (1024.0 * 1024.0));
    
    File f = SD.open("/smoke_test.txt", FILE_WRITE);
    if (f) {
      f.println("Smoke test write OK");
      f.close();
      Serial.println("SD Write OK");
    } else {
      Serial.println("SD Write Failed");
    }
  }
}

static void testGPS() {
  Serial.println(">>> Checking GPS (UART)...");
  // Just dump buffer for 1 second
  unsigned long start = millis();
  bool received = false;
  Serial1.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  
  while (millis() - start < 2000) {
    while (Serial1.available()) {
      char c = Serial1.read();
      Serial.write(c);
      received = true;
    }
  }
  if (!received) {
    Serial.println("No data received from GPS yet (check headers/outdoor view)");
  }
  Serial.println("\n>>> GPS Test End");
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Give time for USB CDC
  
  Serial.println("\n\n========================================");
  Serial.println("   ESP32-S3 SMOKE TEST START");
  Serial.println("========================================");
  
  Serial.printf("Chip Model: %s\n", ESP.getChipModel());
  Serial.printf("Cores: %d\n", ESP.getChipCores());
  Serial.printf("CPU Freq: %d MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Flash Size: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
  if (psramInit()) {
    Serial.printf("PSRAM: %d MB (OK)\n", ESP.getPsramSize() / (1024 * 1024));
  } else {
    Serial.println("PSRAM: Failed or Not Available");
  }

  // I2C Init
  Wire.begin(I2C_SDA, I2C_SCL);
  scanI2C();
  
  // SD Test
  testSD();

  // GPS Test
  testGPS();
  
  Serial.println("Setup Complete. Entering Loop...");
}

void loop() {
  Serial.printf("[Alive] Uptime: %lu ms\n", millis());
  delay(2000);
}
