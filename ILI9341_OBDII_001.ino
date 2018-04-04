#include <Adafruit_Sensor.h>       //General sensor stuff
#include <Adafruit_HMC5883_U.h>    //Used for magnetometer (compass sensor)
#include <OBD.h>                   //Used for accessing ECU PIDs
#include <MPU6050.h>               //Used for accessing accelerometer, gyro, and temperature sensor
#include <SPI.h>                   //Used for display and micro SD card
#include <Wire.h>                  //I2C Communication. Used for the FT6206, MPU6050 and HMC5883
#include <Adafruit_FT6206.h>       //Touchscreen driver
#include <Adafruit_GFX.h>          //Core graphics library
#include <Adafruit_ILI9341.h>      //Display driver

//Create touchscreen object
Adafruit_FT6206 ctp = Adafruit_FT6206();

#define TFT_CS 10
#define TFT_DC 9

//Create display object and define pins
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//Assign a unique ID to this sensor at the same time
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

//Start obd object
COBDI2C obd;

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

int px, py, prev_px = 319;
#define screenWidth 320
int touched = 0;
int xcompass, ycompass;
float HEADING_DEGREES;

int backlight = 5, brightness, c = 1;

float lateral_max = 0, acceleration_max = 0, braking_max = 0;

//g-plot array declaration
float xArr[15];
float yArr[15];
int array_size = sizeof(xArr)/sizeof(xArr[0]);

void setup(){
  Serial.begin(115200);
  tft.begin();   //Start lcd screen
  ctp.begin(50); //Start touchscreen and set sensitivity (0-255). Default is 40
  mag.begin();   //Start magnetometer
  MPU6050_init();
  obd.begin();   //Start communication with OBD-II
  pinMode(backlight, OUTPUT);
  analogWrite(backlight, 255);
  tft.setRotation(1);
  tft.fillScreen(BLUE);
  tryConnecting();  //Write "Connecting.." on screen
  while(!obd.init()); //Try to initialize until success
  tft.fillScreen(BLUE);
  for(int n = 0; n < array_size; n++){      //Fill g-plot array full of zeroes
    xArr[n] = 0.0;
    yArr[n] = 0.0;
  }
}

/*
accel_t_gyro_union mpu;
MPU6050_readout(&mpu);

float tempC=MPU6050_GET_TEMPERATURE(mpu.value.temperature);
float tempF=tempC*(9.0/5.0)+32.0;
Serial.print(tempC);
Serial.print(" C");
Serial.print(" ");
Serial.print(tempF);
Serial.println(" F");

tft.fillRect(139, 109, 100, 80, BLUE);
if(obd.read(PID_RPM, RPM)){
  tft.setCursor(140, 110);
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.print(RPM);
}
delay(50);
*/
void loop(){
if(touched == 0){         //main screen
  //int ii = 4, jj = 250, ff = 6, tt = 20;
  int ii = 0, jj = 0, ff = 0, tt = 0;
  //Serial.println("Screen 0");
  tft.fillScreen(BLUE);
  settingsSymbol();     //settings button
  screen0StaticText();
  int RPM, SPEED, TPS, ENGINE_LOAD, C_TEMP, A_TEMP, FUEL_LEVEL;
  //byte SCREEN_0_PIDARR[] = {PID_RPM, PID_SPEED, PID_THROTTLE, PID_ENGINE_LOAD, PID_COOLANT_TEMP, PID_AMBIENT_TEMP, PID_FUEL_LEVEL};
  //int SCREEN_0_VARARR[] = {RPM, SPEED, TPS, ENGINE_LOAD, C_TEMP, A_TEMP, FUEL_LEVEL};
  while(touched == 0){
    findHeadingDegrees();
    if(ii == 0){//4)
//      ii = 0;
//      /*for(int i = 0; i < 4; i++){
//          if(obd.read(SCREEN_0_PIDARR[i], SCREEN_0_VARARR[i])){
//          }
//      }*/
      if(obd.read(PID_RPM, RPM)){
        displayTachometer(RPM);
      }

//      if(obd.read(PID_SPEED, SPEED)){
//        displaySpeed(SPEED);
//      }
      if(obd.read(PID_THROTTLE, TPS)){
        displayTPS(TPS);
      }

//      if(obd.read(PID_ENGINE_LOAD, ENGINE_LOAD)){
//        displayEngineLoad(ENGINE_LOAD);
//      }
    }//ii++;
#if 0
    if(tt == 0){//20)
      tt = 0;
      /*for(int i = 4; i < 6; i++){
        if(obd.read(SCREEN_0_PIDARR[i], SCREEN_0_VARARR[i])){
        }
      }*/
      if(obd.read(PID_COOLANT_TEMP, C_TEMP)){
      }
      if(obd.read(PID_AMBIENT_TEMP, A_TEMP)){
      }
      displayTemps(C_TEMP, A_TEMP);

    }//tt++;

    //if(gg == 10){
    //  PIDDISTANCE = random(0,
    //}

    if(jj == 0){//250){
      jj = 0;
      /*for(int i = 6; i < 7; i++){
        if(obd.read(SCREEN_0_PIDARR[i], SCREEN_0_VARARR[i])){
        }
      }*/
      if(obd.read(PID_FUEL_LEVEL, FUEL_LEVEL)){
      }
      displayFuelLevel(FUEL_LEVEL);
    }//jj++;

    if(FUEL_LEVEL <= 15){
      if(ff == 6){
        tft.setCursor(61, 201);
        tft.setTextSize(2);
        tft.setTextColor(RED);
        tft.print("LOW");

        tft.setCursor(227, 201);
        tft.setTextSize(2);
        tft.setTextColor(RED);
        tft.print("LOW");
      }
      else if(ff == 12){
        ff = 0;
        tft.fillRect(60, 200, 35, 15, BLUE);
        tft.fillRect(226, 200, 35, 15, BLUE);
      }ff++;
    }
    else if(FUEL_LEVEL > 15){
      tft.fillRect(60, 200, 35, 15, BLUE);
      tft.fillRect(226, 200, 35, 15, BLUE);
    }
#endif
    if(ctp.touched()){
      getNewPoint();
    }
    delay(25);
  }
}
/*else if(touched == 1){        //compass
  int ii = 2;
  //Serial.println("Screen 1");
  tft.fillScreen(BLUE);
  settingsSymbol();    //settings button
  tft.drawCircle(159,119,111,BLACK);
  tft.drawCircle(159,119,112,BLACK);
  while(touched==1){
    if(ii == 2){
      ii = 0;
      drawCrosshairs();
      drawInsideCompass();
      tft.drawLine(159, 119, xcompass, ycompass, BLUE);
      findHeadingDegrees();
      drawCompassNeedle(HEADING_DEGREES);
      writeHeadingDegrees(HEADING_DEGREES);
    }ii++;

    delay(50);
    if(ctp.touched()){
      getNewPoint();
    }
  }
}
else if(touched == 2){        //g-plot screen
  //Serial.println("Screen 2");
  tft.fillScreen(BLUE);
  settingsSymbol();    //settings button
  drawGPlotBorder();
  writeResetButton();
  while(touched == 2){

    drawCrosshairs();
    writeToGPlot();

    delay(35);
    if(ctp.touched()){
      getNewPoint();
    }
  }
}
else if(touched == 3){        //diagnostics
  //Serial.println("Screen 3");
  tft.fillScreen(BLUE);
  settingsSymbol();   //settings button
  //screen3StaticText();
  while(touched == 3){

    delay(250);
    if(ctp.touched()){
      getNewPoint();
    }
  }
}
else if(touched == 4){        //diagnostics cont
  //Serial.println("Screen 4");
  tft.fillScreen(BLUE);
  settingsSymbol();    //settings button
  while(touched == 4){


    if(ctp.touched()){
      getNewPoint();
    }
    delay(50);
  }
}
else if(touched == 5){         //code reader
  //Serial.println("Screen 5");
  tft.fillScreen(BLUE);
  settingsSymbol();     //settings button
  screen5StaticText();
  while(touched == 5){


    if(ctp.touched()){
      getNewPoint();
    }
    delay(50);
  }
}*/
else if(touched == 69){       //settings screen
  //Serial.println("Settings");
  tft.fillScreen(BLUE);
  backSymbol();
  otherStuffOnSettingsScreen();
  tft.drawFastVLine(prev_px, 1, 48, ORANGE);
  while(touched == 69){
    if(c == 1){
      brightness = 255;
      px = 319;
      tft.drawFastVLine(px, 1, 48, ORANGE);
    }c++;
    if(ctp.touched()){
      getNewPoint();
    }
    if(py < 51){
      setBrightness(px);
    }
    analogWrite(backlight, brightness);

    //delay(50);
  }
}
}

//Print "Connecting.." on screen while OBD-II port trys to connect
void tryConnecting(){
  tft.setCursor(60, 110);
  tft.setTextColor(ORANGE);
  tft.setTextSize(3);
  tft.print("Connecting..");
}

float findHeadingDegrees(){
  //Get a new magnetometer sensor event
  sensors_event_t event;
  mag.getEvent(&event);

  //Calculate heading in radians
  float heading = atan2(event.magnetic.y, event.magnetic.x);

  //Magnetic declination in Louisville, KY is -4° 40' West. ==> .08145 radians http://www.magnetic-declination.com/
  //Encorporate magnetic declination into heading
  float declinationAngle = 0.08145;
  heading += declinationAngle;

  //Correct for when signs are reversed.
  if(heading < 0){
    heading += 2*PI;
  }

  //Check for wrap due to addition of declination.
  if(heading > 2*PI){
    heading -= 2*PI;
  }

  //Convert radians to degrees for readability.
  HEADING_DEGREES = heading * 180/PI;


  //float HEADING_DEGREES=random(0.00, 360.00);

  //Display heading as a cardinal direction
  if(HEADING_DEGREES >= 337.5 || HEADING_DEGREES < 22.5){         //N
    cardinalHeading("N");
  }
  else if(HEADING_DEGREES >= 22.5 && HEADING_DEGREES < 67.5){     //NE
    cardinalHeading("NE");
  }
  else if(HEADING_DEGREES >= 67.5 && HEADING_DEGREES < 112.5){    //E
    cardinalHeading("E");
  }
  else if(HEADING_DEGREES >= 112.5 && HEADING_DEGREES < 157.5){   //SE
    cardinalHeading("SE");
  }
  else if(HEADING_DEGREES >= 157.5 && HEADING_DEGREES < 202.5){   //S
    cardinalHeading("S");
  }
  else if(HEADING_DEGREES >= 202.5 && HEADING_DEGREES < 247.5){   //SW
    cardinalHeading("SW");
  }
  else if(HEADING_DEGREES >= 247.5 && HEADING_DEGREES < 292.5){   //W
    cardinalHeading("W");
  }
  else if(HEADING_DEGREES >= 292.5 && HEADING_DEGREES < 337.5){   //NW
    cardinalHeading("NW");
  }
  return HEADING_DEGREES;
}

//Print new cardinal heading at bottom left of screen
void cardinalHeading(String newCardinalHeading){
  if(newCardinalHeading != oldCardinalHeading){
    tft.fillRect(0, 216, 35, 25, BLUE);
    tft.setCursor(2,217);
    tft.setTextSize(3);
    tft.setTextColor(WHITE);
    tft.print(newCardinalHeading);
  }
  oldCardinalHeading = newCardinalHeading;
}

//Draw compass needle on screen 1
unsigned long drawCompassNeedle(int compassDegrees){
  //Serial.println(compassDegrees);
  float compassRadians = (((compassDegrees/360.0)*2.0*PI)+PI)*-1.0;  //no idea how i got this to work
  xcompass = (110*sin(compassRadians))+159;
  ycompass = (110*cos(compassRadians))+119;
  tft.drawLine(159, 119, xcompass, ycompass, RED);
  return xcompass, ycompass;
}

//Write degrees inside circle on screen 1
void writeHeadingDegrees(int HEADING_DEGREES){
  //tft.drawFastVLine(159, 0, 240, LIGHTER_GRAY);
  if(HEADING_DEGREES < 10){
    tft.setCursor(152, 160);
  }
  else if(HEADING_DEGREES >= 10 && HEADING_DEGREES < 100){
    tft.setCursor(143, 160);
  }
  else if(HEADING_DEGREES >= 100){
    tft.setCursor(134, 160);
  }
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.fillRect(133, 159, 53, 23, BLUE);
  tft.print(HEADING_DEGREES);
}

//draw double border for gplot
void drawGPlotBorder(){
  tft.drawCircle(159, 119, 110, BLACK);
  tft.drawCircle(159, 119, 111, BLACK);
}

//draw crosshairs inside compass and gplot
void drawCrosshairs(){
  tft.drawFastVLine(159, 10, 219, GRAY);
  tft.drawFastHLine(50, 119, 219, GRAY);
}

//write N, S, E, W inside compass
void drawInsideCompass(){
  tft.setCursor(152, 13);
  tft.setTextSize(3);
  tft.setTextColor(GRAY);
  tft.print("N");

  tft.setCursor(152, 207);
  tft.setTextSize(3);
  tft.setTextColor(GRAY);
  tft.print("S");

  tft.setCursor(252, 109);
  tft.setTextSize(3);
  tft.setTextColor(GRAY);
  tft.print("E");

  tft.setCursor(51, 109);
  tft.setTextSize(3);
  tft.setTextColor(GRAY);
  tft.print("W");
}

//manipulate gplot array and write dots on gplot
void writeToGPlot(){
  //shift each array element over one to the right
  for(int i = array_size-1; i > 0; i--){
    xArr[i] = xArr[i-1];
    yArr[i] = yArr[i-1];
  }

  //create struct for MPU6050 (acclerometer, gyro, and temperature sensor)
  accel_t_gyro_union mpu;
  MPU6050_readout(&mpu);

  float yaccel = (mpu.value.x_accel)/16384.0;
  float xaccel = (mpu.value.y_accel)/16384.0;
  //float zaccel=(mpu.value.z_accel)/16384.0;

  //int temp=mpu.value.temperature;
  //int xgyro=mpu.value.x_gyro;

  //insert new acceleration value into array position 0
  xArr[0] = xaccel;
  yArr[0] = yaccel;

  //erase red dot in last array element
  tft.fillRoundRect((110*xArr[array_size-1])+160-5, (110*-yArr[array_size-1])+120-5, 10, 10, 2, BLUE);

  //draw green dot in array position 0
  tft.fillRoundRect((110*xArr[0])+160-5, (110*-yArr[0])+120-5, 10, 10, 2, DARK_GREEN);

  //draw red dots
  for(int j = 1; j < array_size-1; j++){
    tft.fillRoundRect((110*xArr[j])+160-5, (110*-yArr[j])+120-5, 10, 10, 2, RED);
  }

  //redraw gplot border if magnitude of acceleration is greater than a number
  if(sqrt((xaccel*xaccel)+(yaccel*yaccel)) > 0.95){
    drawGPlotBorder();
  }

  //Check to see if value is maximum
  if(abs(xaccel) > lateral_max){
      lateral_max = abs(xaccel);
  }
  if(yaccel > 0){
    if(yaccel > acceleration_max){
      acceleration_max = yaccel;
    }
  }
  else if(yaccel < 0){
    if(yaccel < braking_max){
      braking_max = yaccel;
    }
  }
  //erase acceleration maximums
  tft.fillRect(0, 0, 60, 16, BLUE);   //accel
  tft.fillRect(0, 224, 60, 16, BLUE); //brake
  tft.fillRect(270, 0, 60, 16, BLUE); //lat

  //write acceleration maximums
  tft.setTextSize(2);
  tft.setTextColor(BLACK);

  tft.setCursor(1, 1);
  tft.print(acceleration_max);
  tft.setCursor(1, 225);
  tft.print(abs(braking_max));
  tft.setCursor(271, 1);
  tft.print(lateral_max);

  if(ctp.touched()){
    getNewPoint();
  }
}

void writeResetButton(){
  tft.fillRect(216, 217, 65, 20, ORANGE);
  tft.setCursor(220, 220);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("RESET");
}

//Function that writes the settings symbol at the bottom right corner of each screen
void settingsSymbol(){
  tft.fillRoundRect(290, 216, 25, 5, 2, TEAL);
  tft.fillRoundRect(290, 224, 25, 5, 2, TEAL);
  tft.fillRoundRect(290, 232, 25, 5, 2, TEAL);
}

//Function that writes the back symbol at the bottom left corner of the screen
void backSymbol(){
  tft.fillTriangle(25, 215, 25, 237, 2, 227, ORANGE);
}

//Get a new point on the touchscreen and determine what screen to go to next
void getNewPoint(){
  //Serial.print(touched);
  //Serial.print(" ");
  //Retrieve a point
  TS_Point p = ctp.getPoint();

  //flip y around to match the screen and switch x and y directions
  px = screenWidth-p.y;
  py = p.x;

//  Serial.print("("); Serial.print(px);
//  Serial.print(", "); Serial.print(py);
//  Serial.println(")");

  if(px >= 160 && py < 200 && touched != 69){   //right side
  //Serial.println("right side");
    touched++;
    if(touched > 5){
      touched = 0;
    }
  }
  else if(px < 160 && py < 200 && touched != 69){  //left side
  //Serial.println("left side");
    touched--;
    if(touched<0){
      touched = 5;
    }
  }
  else if(py > 199){  //settings, back, and reset buttons
    if(px > 280 && touched != 69){
      //Serial.println("settings button");
      touched = 69;
    }
    else if(px < 41 && touched == 69){
      //Serial.println("back button");
      touched = 0;
    }
    else if(px > 220 && px < 280 && touched == 2){
      //Serial.println("reset button");
      lateral_max = 0;
      acceleration_max = 0;
      braking_max = 0;
    }
  }
  //Serial.println(touched);
  //return touched, px, py;
}

//Display tachometer blocks on screen 0
void displayTachometer(int RPM){
  int BLOCK_COLOR, X, TACH;
  TACH = RPM/25;

  for(X = 310; X > TACH; X -= 10){
    tft.fillRect(X, 0, 10, 40, BLUE);
  }
  for(X = 0; X < TACH+1; X += 10){
    if(RPM < 6500){
      BLOCK_COLOR=GREEN;
    }
    else if(RPM >= 6500){
      BLOCK_COLOR = RED;
    }
    tft.fillRect(X, 0, 10, 40, BLOCK_COLOR);
  }

  if(RPM >= 100 && RPM < 1000){
    tft.setCursor(134, 9);
  }
  else if(RPM >= 1000 && RPM < 10000){
    tft.setCursor(125, 9);
  }
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.print(RPM);
}

//Static text on screen 0
void screen0StaticText(){
  //tft.drawFastVLine(159, 0, 240, LIGHTER_GRAY);  //line in middle to line stuff up
  //tft.drawFastHLine(0, 118, 320, LIGHTER_GRAY);
  tft.drawRect(60, 217, 202, 22, BLACK);   //fuel level outline
  tft.setCursor(102, 201);       //write "fuel level"
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.print("FUEL LEVEL");

  tft.setCursor(130, 65);    //write "speed"
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.print("SPEED");

  tft.drawRect(1, 68, 38, 102, BLACK);
  tft.setCursor(3, 172);    //write "TPS"
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.print("TPS");

  tft.drawRect(281, 68, 38, 102, BLACK);
  tft.setCursor(283, 172);  //write "ENG"
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.print("ENG");

  tft.setCursor(60, 110);   //write "coolant"
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.print("COOLANT");

  tft.setCursor(218, 110);  //write "ambient"
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.print("AMBIENT");

  tft.setCursor(144, 140);  //write "MPG"
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.print("MPG");
}
/*
//static text on screen 3
void screen3StaticText(){
  const int column1 = 1;
  const int column2 = 149;
  tft.setTextSize(2);
  tft.setTextColor(BLACK);

  tft.setCursor(column1, 1);
  tft.print("RPM:"); //engine rpm

  tft.setCursor(column1, 17);
  tft.print("ENG:"); //engine load

  tft.setCursor(column1, 33);
  tft.print("CLT:"); //coolant temp

  tft.setCursor(column1, 49);
  tft.print("FPR:"); //fuel pressure

  tft.setCursor(column1, 65);
  tft.print("IMP:"); //intake MAP

  tft.setCursor(column1, 81);
  tft.print("SPD:"); //vehicle speed

  tft.setCursor(column1, 97);
  tft.print("TAD:"); //timing advance

  tft.setCursor(column1, 113);
  tft.print("ITP:"); //intake temp

  tft.setCursor(column1, 129);
  tft.print("MAF:"); //MAF flow

  tft.setCursor(column1, 145);
  tft.print("TPS:"); //throttle

  tft.setCursor(column1, 161);
  tft.print("RUN:"); //run time

  tft.setCursor(column1, 177);
  tft.print("DWM:"); //distance with MIL

  tft.setCursor(column1, 193);
  tft.print("CEG:"); //commanded EGR

  tft.setCursor(column1, 209);
  tft.print("EGE:"); //EGR error

  tft.setCursor(column1, 225);
  tft.print("FLV:"); //fuel level

  //next column
  tft.setCursor(column2, 1);
  tft.print("WUP:"); //warm ups

  tft.setCursor(column2, 17);
  tft.print("DST:"); //distance

  tft.setCursor(column2, 33);
  tft.print("BAR:"); //barometric

  tft.setCursor(column2, 49);
  tft.print("CMV:"); //control module voltage

  tft.setCursor(column2, 65);
  tft.print("AEL:"); //absolute engine load

  tft.setCursor(column2, 81);
  tft.print("ATP:"); //ambient temp

  tft.setCursor(column2, 97);
  tft.print("TWM:"); //time with MIL

  tft.setCursor(column2, 113);
  tft.print("TCC:"); //time since codes cleared

  tft.setCursor(column2, 129);
  tft.print("FRP:"); //fuel rail pressure

  tft.setCursor(column2, 145);
  tft.print("EOT:"); //engine oil temp

  tft.setCursor(column2, 161);
  tft.print("FIT:"); //fuel injection timing

  tft.setCursor(column2, 177);
  tft.print("EFR:"); //engine fuel rate

  tft.setCursor(column2, 193);
  tft.print("ETD:"); //engine torque demanded

  tft.setCursor(column2, 209);
  tft.print("ETP:"); //engine torque percentage

  tft.setCursor(column2, 225);
  tft.print("ERT:"); //engine ref torque
}
*/
//Static text on screen 5
void screen5StaticText(){
  tft.setCursor(10, 5);
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.print("Clear Codes n Stuff yo");
}

//Display speed on screen 0
void displaySpeed(int SPEED_KPH){
  int SPEED = round(SPEED_KPH * 0.621371);
  if(SPEED < 10){
    tft.setCursor(150, 85);
  }
  else if(SPEED >= 10 && SPEED < 100){
    tft.setCursor(138, 85);
  }
  else if(SPEED >= 100){
    tft.setCursor(126, 85);
  }
  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  tft.fillRect(125, 84, 69, 30, BLUE);
  tft.print(SPEED);
}

//Display TPS on screen 0
void displayTPS(int TPS){
  int Y, TPS_BAR;
  TPS_BAR = 169 - TPS;

  for(Y = 69; Y < TPS_BAR; Y++){
    tft.drawFastHLine(2, Y, 36, BLUE);
  }
  for(Y = 168; Y > TPS_BAR - 1; Y--){
    tft.drawFastHLine(2, Y, 36, GREEN);
  }

  if(TPS < 10){
    tft.setCursor(15, 111);
  }
  else if(TPS >= 10 && TPS < 100){
    tft.setCursor(9, 111);
  }
  else if(TPS == 100){
    tft.setCursor(2, 111);
  }
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(TPS);
}

//Display Engine Load on screen 0
void displayEngineLoad(int ENGINE_LOAD){
  int Y, ENG_BAR;
  ENG_BAR = 169 - ENGINE_LOAD;

  for(Y = 69; Y < ENG_BAR; Y++){
    tft.drawFastHLine(282, Y, 36, BLUE);
  }
  for(Y = 168; Y > ENG_BAR - 1; Y--){
    tft.drawFastHLine(282, Y, 36, GREEN);
  }

  if(ENGINE_LOAD < 10){
    tft.setCursor(296, 111);
  }
  else if(ENGINE_LOAD >= 10 && ENGINE_LOAD < 100){
    tft.setCursor(289, 111);
  }
  else if(ENGINE_LOAD == 100){
    tft.setCursor(282, 111);
  }
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(ENGINE_LOAD);
}

void displayTemps(int C_TEMP, int A_TEMP){
  tft.fillRect(54, 119, 52, 23, BLUE);
  if(C_TEMP < 10){   //coolant temperature
    tft.setCursor(71, 120);
  }
  else if(C_TEMP >= 10 && C_TEMP < 100){
    tft.setCursor(63, 120);
  }
  else if(C_TEMP >= 100){
    tft.setCursor(55, 120);
  }
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.print(C_TEMP);
  ////////////////////////////////////////
  tft.fillRect(211, 119, 52, 23, BLUE);
  if(A_TEMP < 10){      //ambient temperature
    tft.setCursor(231, 120);
  }
  else if(A_TEMP >= 10 && A_TEMP < 100){
    tft.setCursor(222, 120);
  }
  else if(A_TEMP >= 100){
    tft.setCursor(212, 120);
  }
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.print(A_TEMP);
}
/*
//Display fuel mileage on screen 0
void displayFuelMileage(double km1, double km2, int fuelLevel1 int fuelLevel2){
  double deltakm=km2-km1;
  double miles=deltakm*0.621371;
  double deltafuelLevel=fuelLevel2-fuelLevel1;
  int tankCapacity=10;  //in gallons
  double gallons=(deltafuelLevel/100)*tankCapacity;
  int mpg=round(miles/gallons);

  tft.setCursor( ,  );
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.print(mpg);
}*/

//Display fuel level on screen 0
void displayFuelLevel(int FUEL_LEVEL){
  int BLOCK_COLOR, X, LEVEL;
  LEVEL = (FUEL_LEVEL * 2) + 61;
  for(X = 259; X > LEVEL - 1; X -= 2){
    tft.fillRect(X, 218, 2, 20, BLUE);
  }
  for(X = 61; X < LEVEL; X += 2){
    if(FUEL_LEVEL >= 25){
      BLOCK_COLOR = GREEN;
    }
    else if(FUEL_LEVEL >= 10 && FUEL_LEVEL < 25){
      BLOCK_COLOR = YELLOW;
    }
    else if(FUEL_LEVEL < 10){
      BLOCK_COLOR = RED;
    }
    tft.fillRect(X, 218, 2, 20, BLOCK_COLOR);
  }
  if(FUEL_LEVEL < 10){
    tft.setCursor(154, 221);
  }
  else if(FUEL_LEVEL >= 10 && FUEL_LEVEL < 100){
    tft.setCursor(148, 221);
  }
  else if(FUEL_LEVEL == 100){
    tft.setCursor(142, 221);
  }
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print(FUEL_LEVEL);
}

void otherStuffOnSettingsScreen(){
  tft.drawRect(0, 0, 320, 50, GRAY);
  tft.setCursor(105, 18);
  tft.setTextSize(2);
  tft.setTextColor(GRAY);
  tft.print("Brightness");
  }

int setBrightness(int px){
  double level = px/2;
  brightness = (level/160)*255;
  if(px > 80 && px < 240){
    tft.setCursor(105, 18);
    tft.setTextSize(2);
    tft.setTextColor(GRAY);
    tft.print("Brightness");
  }
  if(prev_px != px){
    tft.drawFastVLine(prev_px, 1, 48, BLUE);
    tft.drawFastVLine(px, 1, 48, ORANGE);
    prev_px = px;
  }
  return brightness, prev_px;
}
