#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <OBD.h>
//mpu6050 https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050
//https://playground.arduino.cc/Main/MPU-6050
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_FT6206.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

Adafruit_FT6206 ctp = Adafruit_FT6206();

const int TFT_CS = 10;
const int TFT_DC = 9;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

COBDI2C obd;



void setup() {
    mag.begin();
}

void loop() {
    // put your main code here, to run repeatedly:
}
