//Get a new point on the touchscreen and determine what screen to go to next
#define screenWidth 320
int px, py;

Adafruit_FT6206 ctp = Adafruit_FT6206();

void getNewPoint() {
  //Serial.print(touched);
  //Serial.print(" ");
  //Retrieve a point
  TS_Point p = ctp.getPoint();

  //flip y around to match the screen and switch x and y directions
  px = screenWidth-p.y;
  py = p.x;

 Serial.print("("); Serial.print(px);
 Serial.print(", "); Serial.print(py);
 Serial.println(")");

/*
  if(px >= 160 && py < 200 && touched != 69) {   //right side
  //Serial.println("right side");
    touched++;
    if(touched > 5){
      touched = 0;
    }
  }
  else if(px < 160 && py < 200 && touched != 69) {  //left side
  //Serial.println("left side");
    touched--;
    if(touched<0) {
      touched = 5;
    }
  }
  else if(py > 199) {  //settings, back, and reset buttons
    if(px > 280 && touched != 69) {
      //Serial.println("settings button");
      touched = 69;
    }
    else if(px < 41 && touched == 69) {
      //Serial.println("back button");
      touched = 0;
    }
    else if(px > 220 && px < 280 && touched == 2) {
      //Serial.println("reset button");
      lateral_max = 0;
      acceleration_max = 0;
      braking_max = 0;
    }
  }
  */
}
