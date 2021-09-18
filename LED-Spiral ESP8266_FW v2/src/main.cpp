#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <FastLED.h>
#include "config.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <stdio.h>
#include <string.h>
#define NUM_LEDS_STRIP_ONE 440
#define NUM_LEDS_STRIP_TWO 470
#define NUM_LEDS NUM_LEDS_STRIP_ONE + NUM_LEDS_STRIP_TWO
#ifndef STASSID
#endif

String dllink = "http://dl.timfischbach.com/firmware/ledspiral/";
String strinit;
String initlink;
String binlink;
int serverstatus;
const char SSID[] = "ledspiral";
const char passwd[] = "ledspiral";
char v[6];
bool ltest = true;
byte *c;
int DMX[]{0, 0, 0, 0, 0, 0};
int delay_in_ms = 0;
int mode = 0;
int step = 0;
int direction = 0;
int SpiralPos = 0;
int SpiralPosNeg = 0;
uint8_t hue = 0;
long lastexecuted;
CRGBPalette16 currentPalette;
TBlendType currentBlending;
bool strobesafemode = false;

void delayscan(int d);
void SerialScan();
void handleRoot();
String CheckUpdate();
String UpdateLoop();
String split(String s, char parser, int index);
CRGB leds[NUM_LEDS_STRIP_ONE + NUM_LEDS_STRIP_TWO]; //initialize LEDs
CRGB ledsbackup[NUM_LEDS_STRIP_ONE + NUM_LEDS_STRIP_TWO];
DNSServer dnsServer;
WiFiClient updatewificlient;
HTTPClient http;
void Sparkle(int red, int green, int blue, int SpeedDelay);
void rainbowCycle(int SpeedDelay);
void theaterChase(int red, int green, int blue, int SpeedDelay);
void theaterChaseRainbow(int SpeedDelay);
void SetupPurpleAndGreenPalette();
void SetupTotallyRandomPalette();
void SetupBlackAndWhiteStripedPalette();
void setPixel(int Pixel, int red, int green, int blue);
void showStrip();
byte *Wheel(byte WheelPos);

void update_started()
{
}

void update_finished()
{

    for (int i = 0; i > 30; i++)
    {
        Serial.print('o');
        delay(100);
    }
}

void update_progress(int cur, int total)
{
}

void update_error(int err)
{
    for (int i = 0; i > 30; i++)
    {
        Serial.print('e');
        delay(100);
    }
}
void setup()
{
    Serial.begin(1000000);
    FastLED.addLeds<NEOPIXEL, 0>(leds, 0, NUM_LEDS_STRIP_ONE);
    FastLED.addLeds<NEOPIXEL, 2>(leds, NUM_LEDS_STRIP_ONE, NUM_LEDS_STRIP_TWO);
    VERSION.toCharArray(v, 6);
}
void loop()
{

    SerialScan();
    //Effekte begin--------------------------------------------------
    if (DMX[0] >= 250) //full color DONE
    {
        if (mode != 250)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
            }
            mode = 250;
        }
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB(DMX[1], DMX[2], DMX[3]);
        }
        FastLED.show();
    }

    else if (DMX[0] >= 240 and DMX[0] < 250) 
    {
        if (mode != 240)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
            }
            mode = 240;
        }
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB(0, 0, 0);
        }
        Sparkle(DMX[1], DMX[2], DMX[3], DMX[4]);
    }
    else if (DMX[0] >= 230 and DMX[0] < 240) 
    {
        if (mode != 230)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
            }
            mode = 230;
        }
        rainbowCycle(DMX[4]);
    }
    else if (DMX[0] >= 220 and DMX[0] < 230) 
    {
        if (mode != 220)
        {
            step = 0;
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
            }
            mode = 220;
        }
        theaterChase(DMX[1], DMX[2], DMX[3], DMX[4]);
    }
    else if (DMX[0] >= 210 and DMX[0] < 220) 
    {
        if (mode != 210)
        {
            step = 0;
            direction = 0;
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
            }
            mode = 210;
        }
        theaterChaseRainbow(DMX[4]);
    }
    else if (DMX[0] >= 200 and DMX[0] < 210) 
    {
        if (mode != 200)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
            }
            mode = 200;
        }
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB(0, 0, 0);
        }
        for (int i = 0; i < 50; i++)
        {
            leds[random(NUM_LEDS)] = CRGB(DMX[1], DMX[2], DMX[3]);
        }
        FastLED.show();
        delayscan(DMX[4]);
    }
    else if (DMX[0] >= 190 and DMX[0] < 200) 
    {
        if (mode != 190)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
            }
            mode = 190;
        }
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB(0, 0, 0);
        }
        int rnd[] = {0, 125, 255};
        for (int i = 0; i < 50; i++)
        {
            leds[random(NUM_LEDS)] = CRGB(rnd[random(0, 2)], rnd[random(0, 2)], rnd[random(0, 2)]);
        }
        FastLED.show();
        delayscan(DMX[4]);
    }
    else if (DMX[0] >= 180 and DMX[0] < 190) 
    {
        strobesafemode = true;
        if (mode != 180)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
            }
            mode = 180;
        }
        if (millis() - lastexecuted > 100)
        {
            SpiralPos = 909;
            SpiralPosNeg = 909;
        }
        lastexecuted = millis();
        if (SpiralPos >= 0)
        {
            leds[SpiralPos] = CRGB(DMX[1], DMX[2], DMX[3]);
            leds[SpiralPos - 1] = CRGB(DMX[1], DMX[2], DMX[3]);
            leds[SpiralPos - 2] = CRGB(DMX[1], DMX[2], DMX[3]);
            FastLED.show();
            delay(DMX[4] * 2);
            SpiralPos = SpiralPos - 3;
        }
        if (SpiralPos < 0)
        {
            leds[SpiralPosNeg] = CRGB(0, 0, 0);
            leds[SpiralPosNeg - 1] = CRGB(0, 0, 0);
            leds[SpiralPosNeg - 2] = CRGB(0, 0, 0);
            FastLED.show();
            delay(DMX[4] * 2);
            SpiralPosNeg = SpiralPosNeg - 3;
            if (SpiralPosNeg < 0)
            {
                SpiralPos = 909;
                SpiralPosNeg = 909;
            }
        }
    }
    else if (DMX[0] >= 170 and DMX[0] < 180) 
    {
        if (mode != 170)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
            }
            mode = 170;
        }
        fill_rainbow(leds, NUM_LEDS, hue, 7);
        hue++;
        FastLED.show();
    }
    else if (DMX[0] >= 160 and DMX[0] < 170) 
    {
        strobesafemode = true;
        if (mode != 160)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
            }
            mode = 160;
        }
        int BPM;
        if (DMX[4] < 50)
        {
            BPM = 5;
        }
        else if (DMX[4] < 100 and DMX[4] >= 50)
        {
            BPM = 4;
        }
        else if (DMX[4] < 150 and DMX[4] >= 100)
        {
            BPM = 3;
        }
        else if (DMX[4] < 200 and DMX[4] >= 150)
        {
            BPM = 2;
        }
        else if (DMX[4] <= 200)
        {
            BPM = 1;
        }

        int trail = 4;
        fadeToBlackBy(leds, NUM_LEDS, trail);       //change 20 to smaller or larger number to adjust the length of the tail.
        uint16_t pos = beatsin16(BPM, 0, NUM_LEDS); //BPM will allow you to adjust the speed the dot is moving.
        leds[pos] = CRGB(DMX[1], DMX[2], DMX[3]);
        FastLED.show();
    }
    else if (DMX[0] >= 150 and DMX[0] < 160) 
    {
        if (mode != 150)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
            }
            mode = 150;
        }
        uint8_t BeatsPerMinute = 255 - DMX[4];
        CRGBPalette16 palette;
        if (DMX[1] <= 50)
        {
            palette = CloudColors_p;
        }
        else if (DMX[1] > 50 and DMX[1] <= 75)
        {
            palette = LavaColors_p;
        }
        else if (DMX[1] > 75 and DMX[1] <= 100)
        {
            palette = OceanColors_p;
        }
        else if (DMX[1] > 100 and DMX[1] <= 125)
        {
            palette = ForestColors_p;
        }
        else if (DMX[1] > 125 and DMX[1] <= 150)
        {
            palette = RainbowColors_p;
        }
        else if (DMX[1] > 150 and DMX[1] <= 175)
        {
            palette = RainbowStripeColors_p;
        }
        else if (DMX[1] > 175 and DMX[1] <= 200)
        {
            palette = PartyColors_p;
        }
        else if (DMX[1] > 200)
        {
            palette = HeatColors_p;
        }
        uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
        for (int i = 0; i < NUM_LEDS; i++)
        { //9948
            leds[i] = ColorFromPalette(palette, 0 + (i * 2), beat - 0 + (i * 10));
        }
        FastLED.show();
    }
    else if (DMX[0] >= 140 and DMX[0] < 150) 
    {
        if (mode != 140)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
            }
            mode = 140;
        }
        fadeToBlackBy(leds, NUM_LEDS, 10);
        int pos = random16(NUM_LEDS);
        leds[pos] += CRGB(DMX[1], DMX[2], DMX[3]);
        FastLED.show();
        delayscan(DMX[4]);
    }
    else if (DMX[0] >= 130 and DMX[0] < 140) 
    {
        if (mode != 130)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
            }
            mode = 130;
        }
        static uint16_t sPseudotime = 0;
        static uint16_t sLastMillis = 0;
        static uint16_t sHue16 = 0;

        uint8_t sat8 = beatsin88(87, 220, 250);
        uint8_t brightdepth = beatsin88(341, 96, 224);
        uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
        uint8_t msmultiplier = beatsin88(147, 23, 60);

        uint16_t hue16 = sHue16; //0 * 256;
        uint16_t hueinc16 = beatsin88(113, 1, 3000);

        uint16_t ms = millis();
        uint16_t deltams = ms - sLastMillis;
        sLastMillis = ms;
        sPseudotime += deltams * msmultiplier;
        sHue16 += deltams * beatsin88(400, 5, 9);
        uint16_t brightnesstheta16 = sPseudotime;

        for (uint16_t i = 0; i < NUM_LEDS; i++)
        {
            hue16 += hueinc16;
            uint8_t hue8 = hue16 / 256;

            brightnesstheta16 += brightnessthetainc16;
            uint16_t b16 = sin16(brightnesstheta16) + 32768;

            uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
            uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
            bri8 += (255 - brightdepth);

            CRGB newcolor = CHSV(hue8, sat8, bri8);

            uint16_t pixelnumber = i;
            pixelnumber = (NUM_LEDS - 1) - pixelnumber;

            nblend(leds[pixelnumber], newcolor, 64);
        }
        FastLED.show();
    }
    else if (DMX[0] >= 120 and DMX[0] < 130) 
    {
        if (mode != 120)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
            }
            mode = 120;
        }
        fadeToBlackBy(leds, NUM_LEDS, 20);
        uint8_t dothue = 0;
        for (int i = 0; i < 10; i++)
        {
            leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
            dothue += 32;
        }
        FastLED.show();
        delayscan(DMX[4]);
    }
    else if (DMX[0] >= 110 and DMX[0] < 120) 
    {
        if (mode != 110)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
            }
            mode = 110;
        }

        if (DMX[1] <= 25)
        {
            currentPalette = RainbowColors_p;
            currentBlending = LINEARBLEND;
        }
        if (DMX[1] > 25 and DMX[1] <= 50)
        {
            currentPalette = RainbowStripeColors_p;
            currentBlending = NOBLEND;
        }
        if (DMX[1] > 50 and DMX[1] <= 75)
        {
            currentPalette = RainbowStripeColors_p;
            currentBlending = LINEARBLEND;
        }
        if (DMX[1] > 75 and DMX[1] <= 100)
        {
            SetupPurpleAndGreenPalette();
            currentBlending = LINEARBLEND;
        }
        if (DMX[1] > 100 and DMX[1] <= 125)
        {
            SetupTotallyRandomPalette();
            currentBlending = LINEARBLEND;
        }
        if (DMX[1] > 125 and DMX[1] <= 150)
        {
            SetupBlackAndWhiteStripedPalette();
            currentBlending = NOBLEND;
        }
        if (DMX[1] > 150 and DMX[1] <= 175)
        {
            SetupBlackAndWhiteStripedPalette();
            currentBlending = LINEARBLEND;
        }
        if (DMX[1] > 175 and DMX[1] <= 200)
        {
            currentPalette = CloudColors_p;
            currentBlending = LINEARBLEND;
        }
        if (DMX[1] > 200)
        {
            currentPalette = PartyColors_p;
            currentBlending = LINEARBLEND;
        }
        static uint8_t startIndex = 0;
        startIndex = startIndex + 1; /* motion speed */
        uint8_t colorIndex = startIndex;
        for (int i = 0; i < NUM_LEDS; ++i)
        {
            leds[i] = ColorFromPalette(currentPalette, colorIndex, 255, currentBlending);
            colorIndex += 3;
        }
        FastLED.show();
        delayscan(DMX[4]);
    }

    else if (DMX[0] < 110)
    {
        mode = 0;
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB(0, 0, 0);
        }
        FastLED.show();
    }

    if (DMX[5] > 0)
    {
        delayscan(DMX[5]);
        if (strobesafemode = true)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                ledsbackup[i] = leds[i];
            }
        }
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB(0, 0, 0);
        }
        FastLED.show();
        delayscan(DMX[5]);
        if (strobesafemode = true)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = ledsbackup[i];
            }
            strobesafemode = false;
        }
    }
    //Effekte ende--------------------------------------------------
}

void delayscan(int d)
{
    long time = millis();
    while (millis() < time + d)
    {
        SerialScan();
    }
}

void SerialScan()
{
    if (Serial.available() > 0)
    { //get DMX values
        char w = Serial.read();
        if (w == 'D')
        {
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

            DMX[5] = Serial.read(); // read the incoming byte:
            delay(delay_in_ms);
            Serial.print(" I received:");
            Serial.println(DMX[5]);
            Serial.println("DMX-Vals done.");
            DMX[4] = 255 - DMX[4];
            DMX[5] = 255 - DMX[5] + 1;
            if (DMX[5] == 256)
            {
                DMX[5] = 0;
            }
        }
        else if (w == 's')
        { //status
            Serial.print('o');
        }
        else if (w == 'v')
        { //get version

            Serial.print(v[1]);
            delay(10);
            Serial.print(v[3]);
        }
        else if (w == 'p')
        { //restart request
            Serial.print('o');
            Serial.println("\r");
            ESP.restart();
        }
        else if (w == 'l')
        { //led test
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(255, 255, 255);
            }
            FastLED.show();
            while (Serial.read() != 'b')
            {
                delay(1);
            }
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB(0, 0, 0);
                ltest = true;
            }
            FastLED.show();
        }
        else if (w == 'w')
        {

            WiFi.begin("ledspiral", "ledspiral");
            int timepassed = 0;
            while (WiFi.status() != WL_CONNECTED or timepassed == 90)
            {
                delay(1000);
                timepassed++;
            }
            if (WiFi.status() == WL_CONNECTED)
            {
                Serial.print('o');
            }
            else
            {
                Serial.print('e');
            }
        }
        else if (w == 'u')
        {
            Serial.flush();
            if (CheckUpdate() == "UPDATE_AVAILABLE")
            {
                for (int i = 0; i < 20; i++)
                {
                    delay(100);
                    Serial.print('u');
                }
            }
            else
            {
                for (int i = 0; i < 20; i++)
                {
                    delay(100);
                    Serial.print('e');
                }
            }
        }
        else if (w == 'i')
        {
            while (UpdateLoop() != "HTTP_ERROR")
            {
                UpdateLoop();
            }
            Serial.print('z');
        }
    }
}

String split(String s, char parser, int index)
{
    String rs = "";
    int parserIndex = index;
    int parserCnt = 0;
    int rFromIndex = 0, rToIndex = -1;
    while (parserIndex >= parserCnt)
    {
        rFromIndex = rToIndex + 1;
        rToIndex = s.indexOf(parser, rFromIndex);
        if (parserIndex == parserCnt)
        {
            if (rToIndex == 0 || rToIndex == -1)
                return "";
            return s.substring(rFromIndex, rToIndex);
        }
        else
            parserCnt++;
    }
    return rs;
}

String CheckUpdate()
{

    initlink = dllink + "init";
    http.begin(initlink);
    serverstatus = http.GET();
    if (serverstatus == 200)
    {
        strinit = http.getString();
        http.end();
        String name = split(strinit, '!', 0);
        String filename = split(strinit, '!', 1);
        String recversion = split(strinit, '!', 2);
        String newversion = split(strinit, '!', 3);
        if (VERSION != newversion)
        {
            binlink = dllink + filename + newversion + ".bin";
            return "UPDATE_AVAILABLE";
        }
    }
    else
    {
        http.end();
        return "UPDATE_SERVER_DOWN";
    }
}

String UpdateLoop()
{

    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);
    t_httpUpdate_return ret = ESPhttpUpdate.update(updatewificlient, binlink);
    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        return "HTTP_ERROR";
        break;

    case HTTP_UPDATE_NO_UPDATES:
        break;

    case HTTP_UPDATE_OK:
        break;
    }
}
void Sparkle(int red, int green, int blue, int SpeedDelay)
{
    int Pixel = random(NUM_LEDS);
    setPixel(Pixel, red, green, blue);
    int Pixel2 = random(NUM_LEDS);
    setPixel(Pixel2, red, green, blue);
    int Pixel3 = random(NUM_LEDS);
    setPixel(Pixel3, red, green, blue);
    int Pixel4 = random(NUM_LEDS);
    setPixel(Pixel4, red, green, blue);
    int Pixel5 = random(NUM_LEDS);
    setPixel(Pixel5, red, green, blue);
    showStrip();
    delayscan(SpeedDelay);
    setPixel(Pixel, 0, 0, 0);
    setPixel(Pixel2, 0, 0, 0);
    setPixel(Pixel3, 0, 0, 0);
    setPixel(Pixel4, 0, 0, 0);
    setPixel(Pixel5, 0, 0, 0);
}
void rainbowCycle(int SpeedDelay)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        c = Wheel(((i * 256 / NUM_LEDS) + step) & 255);
        setPixel(i, *c, *(c + 1), *(c + 2));
    }
    showStrip();
    delayscan(SpeedDelay);
    step++;
    if (step > 256 * 5)
    {
        step = 0;
    }
}
void theaterChase(int red, int green, int blue, int SpeedDelay)
{
    for (int i = 0; i < NUM_LEDS; i = i + 3)
    {
        setPixel(i + step, red, green, blue); //turn every third pixel on
    }
    showStrip();

    delayscan(SpeedDelay);

    for (int i = 0; i < NUM_LEDS; i = i + 3)
    {
        setPixel(i + step, 0, 0, 0); //turn every third pixel off
    }
    step++;
    if (step >= 3)
    {
        step = 0;
    }
}
void theaterChaseRainbow(int SpeedDelay)
{

    for (int i = 0; i < NUM_LEDS; i = i + 3)
    {
        c = Wheel((i + step) % 255);
        setPixel(i + direction, *c, *(c + 1), *(c + 2)); //turn every third pixel on
    }
    showStrip();

    delayscan(SpeedDelay);

    for (int i = 0; i < NUM_LEDS; i = i + 3)
    {
        setPixel(i + direction, 0, 0, 0); //turn every third pixel off
    }
    direction++;
    if (direction >= 3)
    {
        direction = 0;
    }
    step++;
    if (step > 255)
    {
        step = 0;
    }
}
void SetupTotallyRandomPalette()
{
    for (int i = 0; i < 16; ++i)
    {
        currentPalette[i] = CHSV(random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid(currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV(HUE_PURPLE, 255, 255);
    CRGB green = CHSV(HUE_GREEN, 255, 255);
    CRGB black = CRGB::Black;

    currentPalette = CRGBPalette16(
        green, green, black, black,
        purple, purple, black, black,
        green, green, black, black,
        purple, purple, black, black);
}
void setPixel(int Pixel, int red, int green, int blue)
{
#ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    strip.setPixelColor(Pixel, strip.Color(red, green, blue));
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
    // FastLED
    leds[Pixel] = CRGB(red, green, blue);
#endif
}
void showStrip()
{
#ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
    // FastLED
    FastLED.show();
#endif
}
byte *Wheel(byte WheelPos)
{
    static byte c[3];

    if (WheelPos < 85)
    {
        c[0] = WheelPos * 3;
        c[1] = 255 - WheelPos * 3;
        c[2] = 0;
    }
    else if (WheelPos < 170)
    {
        WheelPos -= 85;
        c[0] = 255 - WheelPos * 3;
        c[1] = 0;
        c[2] = WheelPos * 3;
    }
    else
    {
        WheelPos -= 170;
        c[0] = 0;
        c[1] = WheelPos * 3;
        c[2] = 255 - WheelPos * 3;
    }

    return c;
}