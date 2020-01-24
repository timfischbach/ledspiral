# ledspiral
Official Repo of the led spiral project by Tim Fischbach

Big thanks to Andrija Vuksanovic for helping me out bringing this project to life!

# What's the LED Spiral project?
The LED Spiral project is a 2 * 2 meters big LED Spiral with ~1000 WS2812B LEDs. The whole spiral is controlled via DMX (control over wifi is following).

# How is it working?
The LED Spiral consists of 3 segments which got they're own 5v power supply to power the led stripes. On the middle segment is the controller box located. Inside works a Arduino Mega2560 and a ESP8266. The Arduino is controlling the display (for adress setting, testing and flashing), and the DMX input, which is converted by an MAX481 IC to serial input. After reading the DMX values, the Arduino sends them to the ESP8266 which is choosing then the right LED modes, based by the DMX values. Then the ESP8266 is sending the final data to the LEDs. This happens with almost no delay, thanks to the hyperthreading library for ESP8266.

# How to update?
When a new version for the LED Spiral is available, download the update file. Select in the controller menu the entry "OTA Mode". After some seconds, the ESP8266 connects automatically to a WiFi called "router" with the password: "ledspiral". If the ESP8266 is connected to your network, open this link: http://esp8266-webupdate.local/update in your browser. You'll be asked for a username and password, which is in both cases: "admin". Then click on choose file and select the update file. After this click update and wait some minutes. After this, click reboot on the controller. The update progress is now done.
