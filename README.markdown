# LEDstrip using a Digispark

## My configuration

I am using Chinese clones of the Digispark that have the reset pin enabled (thus
making the P5 pin unusable). I have first uploaded a new version (2.3) of the
Micronucleus bootloader and then disabled the RESET pin. This way, the
bootloader is much smaller and more bug-free. It needs a new Micronucleus to
upload the programs, though (and optionally modification of the boards.txt
file).

By default, Micronucleus waits 6 seconds before it will conclude there is no
computer connected. This is (as far as I can see) far longer than actually
necessary. A single second is long enough. Therefore, I have built a special
version of Micronucleus (version 2.3) that reduces this delay. You can change
this by setting `AUTO_EXIT_MS` to 1000 (in
`firmware/configuration/t85_default/bootloaderconfig.h` of the Micronucleus
source).

Additionally, the version of Micronucleus that is shipped with the Digispark is
very out of date. Newer versions are much smaller and in my experience also much
more stable (it has [less connection
problems](https://digistump.com/wiki/digispark/tutorials/connecting#troubleshooting)).

You need an AVR programmer for installing a new bootloader. Luckily, if you have
another Arduino you can use the [Arduino as
ISP](https://www.arduino.cc/en/Tutorial/ArduinoISP) sketch to emulate a
programmer (I think it's even possible doing this with another Digispark, but I
haven't tested this).

To install the new bootloader (replace `$ARDUINO_DIR` with the installation
directory of the Arduino IDE software):

    $ avrdude -C$ARDUINO_DIR/hardware/tools/avr/etc/avrdude.conf -v -pattiny85 -cstk500v1 -P/dev/ttyACM0 -b19200 -Uflash:w:t85_shortdelay.hex.hex:i

Now **test the bootloader** by installing a sketch using the `micronucleus`
command. After you've disabled the reset pin, there's no going back (unless you
want to buy or build a high-voltage programmer to reset these fuses).

Command to disable the reset pin (**WARNING: this effectively locks the bootloader**):

    $ avrdude -P /dev/ttyACM0 -p attiny85 -c stk500v1 -v -b 19200 -U lfuse:w:0xe1:m -U hfuse:w:0x5d:m -U efuse:w:0xfe:m

Now you should be able to run the sketch without reset.

Connect:

| Pin | Function
|-----|---------
| P0  | button: change speed of animation (6 steps)
| P1  | LED strip signal (configured for the WS2812 and 30 LEDs)
| P2  | LOW (for a button over P0-P2)
| P3  | LOW (for a button over P3-P5)
| P4  | not connected
| P5  | button: change animation mode
