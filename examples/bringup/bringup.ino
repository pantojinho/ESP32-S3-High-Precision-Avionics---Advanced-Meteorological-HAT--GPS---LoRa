#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// Board I2C for touch + IMU
static constexpr int I2C_SDA = 47;
static constexpr int I2C_SCL = 48;

// SD card pins (on-board TF)
static constexpr int SD_CS   = 38;
static constexpr int SD_MOSI = 39;
static constexpr int SD_MISO = 40;
static constexpr int SD_SCK  = 41;

static void scanI2C() {
  uint8_t found = 0;
  for (uint8_t addr = 1; addr < 127; ++addr) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();
    if (err == 0) {
      Serial.print("I2C device @ 0x");
      if (addr < 16) Serial.print('0');
      Serial.println(addr, HEX);
      found++;
    }
  }
  if (found == 0) {
    Serial.println("No I2C devices found.");
  }
}

static void testSD() {
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS, SPI)) {
    Serial.println("SD init failed.");
    return;
  }
  Serial.println("SD init OK.");

  File f = SD.open("/bringup.txt", FILE_APPEND);
  if (!f) {
    Serial.println("SD open failed.");
    return;
  }
  f.println("bringup ok");
  f.close();
  Serial.println("SD write OK.");
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\nESP32-S3 AMOLED Bringup");

  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.println("Scanning I2C...");
  scanI2C();

  Serial.println("Testing SD card...");
  testSD();
}

void loop() {
  delay(1000);
}
