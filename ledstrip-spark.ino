
void setup() {
  DDRB  = B00100100; // set pin 2 and 5 as output, the rest as input

  // pin 0 and 3: enable pull-up register
  // pin 2 and 5: set output pin to LOW
  // rest:        no pull-up on input pin
  PORTB = B00001001;
  ledstripSetup();
}

void loop() {
  ledstripLoop();
  static uint8_t pins = 0;
  if ((pins & B00001001) != (PINB & B00001001)) {
    pins = PINB;
    if (!(PINB & B00000001)) {
      ledstripNextMode();
      delay(100);
    }
  }
}
