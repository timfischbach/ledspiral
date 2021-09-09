# ledspiral

Official Repo of the led spiral project by Tim Fischbach

Big thanks to Andrija Vuksanovic helping me to bring this big project to life!

# Open the Wiki for the DMX Tables and the full documentation

# What's the LED Spiral project?
The LED Spiral project is a 2 * 2 meters big LED Spiral with ~900 WS2812B LEDs. The whole spiral is controlled via DMX.

# Known unfixable Bugs:
- When you power on the spiral while DMX already connected, there is a chance of the spiral not booting up and just having a whitescreen.
  Solution: unplug DMX, power up the spiral and plug DMX back in
  
# How does it work?
The LED Spiral consists of 3 segments which got they're own 5v power supply to power the led stripes. On the middle segment is the controller box located. Inside works a Arduino Mega2560 and a ESP8266. The Arduino is controlling the display (for adress setting, testing and flashing), and the DMX input, which is converted by an MAX481 IC to serial input. After reading the DMX values, the Arduino sends them to the ESP8266 which is choosing then the right LED modes, based by the DMX values. Then the ESP8266 is sending the final data to the LEDs. This happens with almost no delay.

# How to update?
Just connect the LED-Spiral with WiFi and it'll download the latest update from the update server.

