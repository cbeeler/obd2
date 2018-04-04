#include <Arduino.h>
#include <Adafruit_Sensor.h> //General sensor stuff
//#include <Adafruit_HMC5883_U.h> //Used for magnetometer (compass sensor)
//#include <OBD.h> //Used for accessing ECU PIDs
//#include <MPU6050.h> //Used for accessing accelerometer, gyro, and temperature sensor
#include <SPI.h> //Used for display and micro SD card
#include <Wire.h> //I2C Communication. Used for the FT6206, MPU6050 and HMC5883
#include <Adafruit_FT6206.h> //Touchscreen driver
#include <Adafruit_GFX.h> //Core graphics library
#include <Adafruit_ILI9341.h> //Display driver

#include "getNewPoint.h"

//Adafruit_FT6206 ctp = Adafruit_FT6206();

const int TFT_CS = 10;
const int TFT_DC = 9;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

//COBDI2C obd;

//obd i2c address 0x62
//magnetometer i2c address 0x3C

//Color definitions in 565 Hex
#define BLACK 0x0000
#define LIGHTER_GRAY 0xE71C
#define LIGHT_GRAY 0xC618
#define GRAY 0x39E7
#define BLUE 0x001F
#define TEAL 0x075F
#define TACH_BLUE 0x0018
#define CYAN 0x07FF
#define RED 0xF800
#define ORANGE 0xFA40
#define MAGENTA 0xF81F
#define GREEN 0x0640
#define BRIGHT_GREEN 0x07E0
#define DARK_GREEN 0x0540
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
//Can also use tft.color565(uint8_t r, uint8_t g, uint8_t b);

String newCardinalHeading, oldCardinalHeading;

//int px, py, prev_px = 319;
#define screenWidth 320
int touched = 0;
// int xcompass, ycompass;
// float HEADING_DEGREES;

int backlight = 5, brightness, c = 1;

//float lateral_max = 0, acceleration_max = 0, braking_max = 0;

//g-plot array declaration
// float xArr[15];
// float yArr[15];
// int array_size = sizeof(xArr)/sizeof(xArr[0]);

void setup() {
  Serial.begin(9600);
  tft.begin();   //Start lcd screen
  ctp.begin(50); //Start touchscreen and set sensitivity (0-255). Default is 40
  //mag.begin();   //Start magnetometer
  //MPU6050_init();
  //obd.begin();   //Start communication with OBD-II
  pinMode(backlight, OUTPUT);
  analogWrite(backlight, 255);
  tft.setRotation(1);
  tft.fillScreen(BLUE);
  //tryConnecting();  //Write "Connecting.." on screen
  //while(!obd.init()); //Try to initialize until success
  //tft.fillScreen(BLUE);
  // for(int n = 0; n < array_size; n++){      //Fill g-plot array full of zeroes
  //   xArr[n] = 0.0;
  //   yArr[n] = 0.0;
  // }
}

void loop() {
  if(ctp.touched()) {
    getNewPoint();
  }
}
