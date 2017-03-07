
int main() {
  // initialize timers etc.
  init();

  // setup

  // set pin 2 and 3 as output, the rest as input
  DDRB  = B00001100;

  // pin 0 and 5: enable pull-up register
  // pin 2 and 5: set output pin to LOW
  // rest:        no pull-up on input pin
  PORTB = B00100001;
  ledstripSetup();

  // loop
  for (;;) {
    ledstripLoop();
    static uint8_t pins = 0;
    if ((pins & B00100001) != (PINB & B00100001)) {
      pins = PINB;
      if (!(PINB & B00000001)) {
        ledstripNextMode();
      }
      if (!(PINB & B00100000)) {
        ledstripNextSpeed();
      }
      for (uint8_t i = 0xff; i; i--);
    }
  }
}
