uint8_t pin13led = 13;

void setup() {
  pinMode(pin13led, OUTPUT);
}

void loop() {
  blinkled();
}

void blinkled() {
  digitalWrite(pin13led, HIGH);
  delay(50);
  digitalWrite(pin13led, LOW);
  delay(50);
}
