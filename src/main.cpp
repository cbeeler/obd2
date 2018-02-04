#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <libraries/OBD/OBD.h>
//mpu6050
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_FT6206.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
//COBDI2C obd;
Adafruit_FT6206 ctp = Adafruit_FT6206();


void setup() {
    mag.begin();
}

void loop() {
    // put your main code here, to run repeatedly:
}
