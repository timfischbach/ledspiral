# ledspiral
Official Repo of the led spiral project by Tim Fischbach

Big thanks to Andrija Vuksanovic helping me to bring this big project to life!

# What's the LED Spiral project?
The LED Spiral project is a 2 * 2 meters big LED Spiral with ~1000 WS2812B LEDs. The whole spiral is controlled via DMX (control over wifi is following).

# How does it work?
The LED Spiral consists of 3 segments which got they're own 5v power supply to power the led stripes. On the middle segment is the controller box located. Inside works a Arduino Mega2560 and a ESP8266. The Arduino is controlling the display (for adress setting, testing and flashing), and the DMX input, which is converted by an MAX481 IC to serial input. After reading the DMX values, the Arduino sends them to the ESP8266 which is choosing then the right LED modes, based by the DMX values. Then the ESP8266 is sending the final data to the LEDs. This happens with almost no delay.

# How to update?
Just connect the LED-Spiral with WiFi and it'll download the latest update from the update server.

# When will the source code be available?
It'll be available by August 2021.
