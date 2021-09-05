
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <FastLED.h>

#define NUM_LEDS_STRIP_ONE 440
#define NUM_LEDS_STRIP_TWO 470
#define NUM_LEDS NUM_LEDS_STRIP_ONE + NUM_LEDS_STRIP_TWO
#ifndef STASSID
#define STASSID "router"
#define STAPSK  "ledspiral"
#endif
const char* ssid = STASSID;
const char* password = STAPSK;


bool ota = false;
const int port = 80;
const char v = '1';
const char v2 = '0';
bool ltest = false;
int DMX[] {0, 0, 0, 0, 0};
int delay_in_ms = 1;
int LEDSEL[255];
int SpiralPos = 0;
int SpiralPosNeg = 0;
uint8_t hue = 0;
long lastexecuted;

CRGB leds[NUM_LEDS_STRIP_ONE + NUM_LEDS_STRIP_TWO];




void setup() {
  //ESP.wdtDisable();
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    ArduinoOTA.setHostname("myesp8266");
    ArduinoOTA.setPassword("admin");
    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else { // U_FS
        type = "filesystem";
      }

      // NOTE: if updating FS this would be the place to unmount FS using FS.end()
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });
    ArduinoOTA.begin();
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    ota = true;
  }
  else {
    FastLED.addLeds<NEOPIXEL, 0>(leds, 0, NUM_LEDS_STRIP_ONE);
    FastLED.addLeds<NEOPIXEL, 2>(leds, NUM_LEDS_STRIP_ONE, NUM_LEDS_STRIP_TWO);


  }
}
void loop() {
  if (ota == true) {
    ArduinoOTA.handle();
  }
  //maincode
  else {
    if (Serial.available() > 0) {
      char w = Serial.read();
      if (w == 'D') {
        Serial.println("DMX-Vals incomming:");
        delay(delay_in_ms);
        DMX[0] = Serial.read(); // read the incoming byte:
        delay(delay_in_ms);
        Serial.print(" I received:");
        Serial.println(DMX[0]);

        DMX[1] = Serial.read(); // read the incoming byte:
        delay(delay_in_ms);
        Serial.print(" I received:");
        Serial.println(DMX[1]);

        DMX[2] = Serial.read(); // read the incoming byte:
        delay(delay_in_ms);
        Serial.print(" I received:");
        Serial.println(DMX[2]);

        DMX[3] = Serial.read(); // read the incoming byte:
        delay(delay_in_ms);
        Serial.print(" I received:");
        Serial.println(DMX[3]);

        DMX[4] = Serial.read(); // read the incoming byte:
        delay(delay_in_ms);
        Serial.print(" I received:");
        Serial.println(DMX[4]);
        Serial.println("DMX-Vals done.");
      }
      else if (w == 's') {
        Serial.print('o');
      }
      else if (w == 'd') {
        String inSerial = Serial.readString();
        Serial.flush();
      }
      else if (w == 'v') {

        Serial.print(v);
        delay(10);
        Serial.print(v2);
      }
      else if (w == 'p') {
        Serial.print('o');
        ESP.restart();
      }
      else if (w == 'l') {
        if (ltest == true) {
          ltest = false;
          for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB(255, 255, 255);
          }
          FastLED.show();
        }
      }
      else if (w == 'b') {
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB(0, 0, 0);
          ltest = true;
        }
        FastLED.show();
      }
    }
    //Effekte begin--------------------------------------------------
    if (DMX[0] > 160 and DMX[0] < 170 ) {
      if( random8() < 200) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
    }
    if (DMX[0] > 170 and DMX[0] < 180) {
      Serial.println("DMX175");
      int BPM = 3 ;
      int trail = 4;
      fadeToBlackBy( leds, NUM_LEDS, trail); //change 20 to smaller or larger number to adjust the length of the tail.
      uint16_t pos = beatsin16(BPM, 0, NUM_LEDS); //BPM will allow you to adjust the speed the dot is moving.
      leds[pos] = CRGB(DMX[1], DMX[2], DMX[3]);
      FastLED.show();
    }
    if (DMX[0] > 180 and DMX[0] < 190) {
      Serial.println("DMX185");
      fill_rainbow( leds, NUM_LEDS, hue, 1);
      hue++;
      FastLED.show();
    }
    if (DMX[0] > 190 and DMX[0] < 200) {
      Serial.println("DMX195");
      fill_rainbow( leds, NUM_LEDS, hue, 255 / NUM_LEDS);
      hue++;
      FastLED.show();
    }

    if (DMX[0] > 200 and DMX[0] < 210) {
      Serial.println("DMX205");
      fill_rainbow( leds, NUM_LEDS, 255, 7);
    }
    if (DMX[0] > 210 and DMX[0] < 220) {
      if (millis() - lastexecuted > 100) {
        SpiralPos = 909;
        SpiralPosNeg = 909; 
      }
      lastexecuted = millis();
      Serial.println("DMX215");
      if (SpiralPos >= 0) {
        leds[SpiralPos] = CRGB(DMX[1], DMX[2], DMX[3]);
        leds[SpiralPos - 1] = CRGB(DMX[1], DMX[2], DMX[3]);
        leds[SpiralPos - 2] = CRGB(DMX[1], DMX[2], DMX[3]);
        FastLED.show();
        delay(DMX[4] / 2);
        SpiralPos = SpiralPos - 3;
      }
      if (SpiralPos < 0) {
        leds[SpiralPosNeg] = CRGB(0, 0, 0);
        leds[SpiralPosNeg - 1] = CRGB(0, 0, 0);
        leds[SpiralPosNeg - 2] = CRGB(0, 0, 0);
        FastLED.show();
        delay(DMX[4] / 2);
        SpiralPosNeg = SpiralPosNeg - 3;
        if (SpiralPosNeg < 0) {
          SpiralPos = 909;
          SpiralPosNeg = 909;
        }
      }
    }
    if (DMX[0] > 220 and DMX[0] < 230) {
            if (millis() - lastexecuted > 100) {
        SpiralPos = 0;
        SpiralPosNeg = 0; 
      }
      lastexecuted = millis();
      Serial.println("DMX225");
      if (SpiralPos <= 909) {
        leds[SpiralPos] = CRGB(DMX[1], DMX[2], DMX[3]);
        leds[SpiralPos + 1] = CRGB(DMX[1], DMX[2], DMX[3]);
        leds[SpiralPos + 2] = CRGB(DMX[1], DMX[2], DMX[3]);
        FastLED.show();
        delay(DMX[4] / 2);
        SpiralPos = SpiralPos + 3;
      }
      if (SpiralPos > 909) {
        leds[SpiralPosNeg] = CRGB(0, 0, 0);
        leds[SpiralPosNeg + 1] = CRGB(0, 0, 0);
        leds[SpiralPosNeg + 2] = CRGB(0, 0, 0);
        FastLED.show();
        delay(DMX[4] / 2);
        SpiralPosNeg = SpiralPosNeg + 3;
        if (SpiralPosNeg >= 909) {
          SpiralPos = 0;
          SpiralPosNeg = 0;
        }
      }
    }
    if (DMX[0] > 230 and DMX[0] < 240) {
      Serial.println("DMX235");
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(DMX[1], DMX[2], DMX[3]);
      }
      FastLED.show();
      if (DMX[4] > 0) {
        delay(random(0, 1000));
      }
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(0, 0, 0);
      }
      FastLED.show();
      if (DMX[4] > 0) {
        delay(random(0, 1000));
      }
    }
    if (DMX[0] > 240 and DMX[0] < 250) {
      Serial.println("DMX245");
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(DMX[1], DMX[2], DMX[3]);
      }
      FastLED.show();
      if (DMX[4] > 0) {
        delay(DMX[4] * 2);
      }
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(0, 0, 0);
      }
      FastLED.show();
      if (DMX[4] > 0) {
        delay(DMX[4] * 2);
      }
    }
    if (DMX[0] > 250) {
      Serial.println("DMX255");
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(DMX[1], DMX[2], DMX[3]);
      }
      FastLED.show();
    }
    if (DMX[0] <160) {
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(0, 0, 0);
      }
    }
    //Effekte ende--------------------------------------------------
  }
}
