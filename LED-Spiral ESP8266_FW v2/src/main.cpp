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
int progress;
int RecLimit = 15;
bool recoveryDisabled = false;
unsigned long bootTimestamp;
const char SSID[] = "ledspiral";
const char passwd[] = "ledspiral";
const int port = 80;
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
CRGB clr1;
CRGB clr2;
uint8_t speed;
uint8_t loc1;
uint8_t loc2;
uint8_t ran1;
uint8_t ran2;
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

void RGBLoop();
void FadeInOut(int red, int green, int blue);
void Strobe(int red, int green, int blue, int StrobeCount, int FlashDelay, int EndPause);
void HalloweenEyes(int red, int green, int blue,
                   int EyeWidth, int EyeSpace,
                   boolean Fade, int Steps, int FadeDelay,
                   int EndPause);
void CylonBounce(int red, int green, int blue, int EyeSize, int SpeedDelay, int ReturnDelay);
void NewKITT(int red, int green, int blue, int EyeSize, int SpeedDelay, int ReturnDelay);
void CenterToOutside(int red, int green, int blue, int EyeSize, int SpeedDelay, int ReturnDelay);
void OutsideToCenter(int red, int green, int blue, int EyeSize, int SpeedDelay, int ReturnDelay);
void LeftToRight(int red, int green, int blue, int EyeSize, int SpeedDelay, int ReturnDelay);
void RightToLeft(int red, int green, int blue, int EyeSize, int SpeedDelay, int ReturnDelay);
void Twinkle(int red, int green, int blue, int Count, int SpeedDelay, boolean OnlyOne);
void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne);
void Sparkle(int red, int green, int blue, int SpeedDelay);
void SnowSparkle(int red, int green, int blue, int SparkleDelay, int SpeedDelay);
void RunningLights(int red, int green, int blue, int WaveDelay);
void colorWipe(int red, int green, int blue, int SpeedDelay);
void rainbowCycle(int SpeedDelay);
byte *Wheel(byte WheelPos);
void theaterChase(int red, int green, int blue, int SpeedDelay);
void theaterChaseRainbow(int SpeedDelay);
void Fire(int Cooling, int Sparking, int SpeedDelay);
void setPixelHeatColor(int Pixel, byte temperature);
void BouncingColoredBalls(int BallCount, byte colors[][3], boolean continuous);
void meteorRain(int red, int green, int blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay);
void fadeToBlack(int ledNo, byte fadeValue);
void showStrip();
void setPixel(int Pixel, int red, int green, int blue);
void setAll(int red, int green, int blue);
void fadeall();
void SetupPurpleAndGreenPalette();
void SetupTotallyRandomPalette();
void SetupBlackAndWhiteStripedPalette();

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

    else if (DMX[0] >= 240 and DMX[0] < 250) //Spiral fadeout inside DONE
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
    else if (DMX[0] >= 230 and DMX[0] < 240) //Spiral fadeout inside DONE
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
    else if (DMX[0] >= 220 and DMX[0] < 230) //Spiral fadeout inside DONE
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
    else if (DMX[0] >= 210 and DMX[0] < 220) //Spiral fadeout inside DONE
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
    else if (DMX[0] >= 200 and DMX[0] < 210) //Spiral fadeout inside DONE
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
    else if (DMX[0] >= 190 and DMX[0] < 200) //Spiral fadeout inside DONE
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
    else if (DMX[0] >= 180 and DMX[0] < 190) //Spiral fadeout inside DONE
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
    else if (DMX[0] >= 170 and DMX[0] < 180) //Spiral fadeout inside DONE
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
    else if (DMX[0] >= 160 and DMX[0] < 170) //Spiral fadeout inside DONE
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
    else if (DMX[0] >= 150 and DMX[0] < 160) //Spiral fadeout inside DONE
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
        if (DMX[1] <= 25)
        {
            palette = CloudColors_p;
        }
        else if (DMX[1] > 25 and DMX[1] <= 50)
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
        else if (DMX[1] > 150 and DMX[1] <= 170)
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
    else if (DMX[0] >= 140 and DMX[0] < 150) //Spiral fadeout inside DONE
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
    else if (DMX[0] >= 130 and DMX[0] < 140) //Spiral fadeout inside DONE
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
    else if (DMX[0] >= 120 and DMX[0] < 130) //Spiral fadeout inside DONE
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
    else if (DMX[0] >= 110 and DMX[0] < 120) //Spiral fadeout inside DONE
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

void RGBLoop()
{
    for (int j = 0; j < 3; j++)
    {
        // Fade IN
        for (int k = 0; k < 256; k++)
        {
            switch (j)
            {
            case 0:
                setAll(k, 0, 0);
                break;
            case 1:
                setAll(0, k, 0);
                break;
            case 2:
                setAll(0, 0, k);
                break;
            }
            showStrip();
            delayscan(3);
        }
        // Fade OUT
        for (int k = 255; k >= 0; k--)
        {
            switch (j)
            {
            case 0:
                setAll(k, 0, 0);
                break;
            case 1:
                setAll(0, k, 0);
                break;
            case 2:
                setAll(0, 0, k);
                break;
            }
            showStrip();
            delayscan(3);
        }
    }
}

void FadeInOut(int red, int green, int blue)
{
    float r, g, b;

    for (int k = 0; k < 256; k = k + 1)
    {
        r = (k / 256.0) * red;
        g = (k / 256.0) * green;
        b = (k / 256.0) * blue;
        setAll(r, g, b);
        showStrip();
    }

    for (int k = 255; k >= 0; k = k - 2)
    {
        r = (k / 256.0) * red;
        g = (k / 256.0) * green;
        b = (k / 256.0) * blue;
        setAll(r, g, b);
        showStrip();
    }
}

void Strobe(int red, int green, int blue, int StrobeCount, int FlashDelay, int EndPause)
{
    for (int j = 0; j < StrobeCount; j++)
    {
        setAll(red, green, blue);
        showStrip();
        delay(FlashDelay);
        setAll(0, 0, 0);
        showStrip();
        delayscan(FlashDelay);
    }

    delayscan(EndPause);
}

void HalloweenEyes(int red, int green, int blue,
                   int EyeWidth, int EyeSpace,
                   boolean Fade, int Steps, int FadeDelay,
                   int EndPause)
{
    randomSeed(analogRead(0));

    int i;
    int StartPoint = random(0, NUM_LEDS - (2 * EyeWidth) - EyeSpace);
    int Start2ndEye = StartPoint + EyeWidth + EyeSpace;

    for (i = 0; i < EyeWidth; i++)
    {
        setPixel(StartPoint + i, red, green, blue);
        setPixel(Start2ndEye + i, red, green, blue);
    }

    showStrip();

    if (Fade == true)
    {
        float r, g, b;

        for (int j = Steps; j >= 0; j--)
        {
            r = j * (red / Steps);
            g = j * (green / Steps);
            b = j * (blue / Steps);

            for (i = 0; i < EyeWidth; i++)
            {
                setPixel(StartPoint + i, r, g, b);
                setPixel(Start2ndEye + i, r, g, b);
            }

            showStrip();
            delayscan(FadeDelay);
        }
    }

    setAll(0, 0, 0); // Set all black

    delayscan(EndPause);
}

void CylonBounce(int red, int green, int blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{

    for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++)
    {
        setAll(0, 0, 0);
        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        showStrip();
        delayscan(SpeedDelay);
    }

    delayscan(ReturnDelay);

    for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--)
    {
        setAll(0, 0, 0);
        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        showStrip();
        delayscan(SpeedDelay);
    }

    delayscan(ReturnDelay);
}

void NewKITT(int red, int green, int blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
    RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
    CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}

// used by NewKITT
void CenterToOutside(int red, int green, int blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
    for (int i = ((NUM_LEDS - EyeSize) / 2); i >= 0; i--)
    {
        setAll(0, 0, 0);

        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

        setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(NUM_LEDS - i - j, red, green, blue);
        }
        setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

        showStrip();
        delayscan(SpeedDelay);
    }
    delayscan(ReturnDelay);
}

// used by NewKITT
void OutsideToCenter(int red, int green, int blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
    for (int i = 0; i <= ((NUM_LEDS - EyeSize) / 2); i++)
    {
        setAll(0, 0, 0);

        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);

        setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(NUM_LEDS - i - j, red, green, blue);
        }
        setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);

        showStrip();
        delayscan(SpeedDelay);
    }
    delayscan(ReturnDelay);
}

// used by NewKITT
void LeftToRight(int red, int green, int blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
    for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++)
    {
        setAll(0, 0, 0);
        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        showStrip();
        delayscan(SpeedDelay);
    }
    delayscan(ReturnDelay);
}

// used by NewKITT
void RightToLeft(int red, int green, int blue, int EyeSize, int SpeedDelay, int ReturnDelay)
{
    for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--)
    {
        setAll(0, 0, 0);
        setPixel(i, red / 10, green / 10, blue / 10);
        for (int j = 1; j <= EyeSize; j++)
        {
            setPixel(i + j, red, green, blue);
        }
        setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
        showStrip();
        delayscan(SpeedDelay);
    }
    delayscan(ReturnDelay);
}

void Twinkle(int red, int green, int blue, int Count, int SpeedDelay, boolean OnlyOne)
{
    setAll(0, 0, 0);

    for (int i = 0; i < Count; i++)
    {
        setPixel(random(NUM_LEDS), red, green, blue);
        showStrip();
        delayscan(SpeedDelay);
        if (OnlyOne)
        {
            setAll(0, 0, 0);
        }
    }

    delayscan(SpeedDelay);
}

void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne)
{
    setAll(0, 0, 0);

    for (int i = 0; i < Count; i++)
    {
        setPixel(random(NUM_LEDS), random(0, 255), random(0, 255), random(0, 255));
        showStrip();
        delayscan(SpeedDelay);
        if (OnlyOne)
        {
            setAll(0, 0, 0);
        }
    }

    delayscan(SpeedDelay);
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

void SnowSparkle(int red, int green, int blue, int SparkleDelay, int SpeedDelay)
{
    setAll(red, green, blue);

    int Pixel = random(NUM_LEDS);
    setPixel(Pixel, 0xff, 0xff, 0xff);
    showStrip();
    delayscan(SparkleDelay);
    setPixel(Pixel, red, green, blue);
    showStrip();
    delayscan(SpeedDelay);
}

void RunningLights(int red, int green, int blue, int WaveDelay)
{
    int Position = 0;

    for (int i = 0; i < NUM_LEDS * 2; i++)
    {
        Position++; // = 0; //Position + Rate;
        for (int i = 0; i < NUM_LEDS; i++)
        {
            // sine wave, 3 offset waves make a rainbow!
            //float level = sin(i+Position) * 127 + 128;
            //setPixel(i,level,0,0);
            //float level = sin(i+Position) * 127 + 128;
            setPixel(i, ((sin(i + Position) * 127 + 128) / 255) * red,
                     ((sin(i + Position) * 127 + 128) / 255) * green,
                     ((sin(i + Position) * 127 + 128) / 255) * blue);
        }

        showStrip();
        delayscan(WaveDelay);
    }
}

void colorWipe(int red, int green, int blue, int SpeedDelay)
{
    for (uint16_t i = 0; i < NUM_LEDS; i++)
    {
        setPixel(i, red, green, blue);
        showStrip();
        delayscan(SpeedDelay);
    }
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

// used by rainbowCycle and theaterChaseRainbow
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

void Fire(int Cooling, int Sparking, int SpeedDelay)
{
    static byte heat[NUM_LEDS];
    int cooldown;

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < NUM_LEDS; i++)
    {
        cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

        if (cooldown > heat[i])
        {
            heat[i] = 0;
        }
        else
        {
            heat[i] = heat[i] - cooldown;
        }
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = NUM_LEDS - 1; k >= 2; k--)
    {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' near the bottom
    if (random(255) < Sparking)
    {
        int y = random(7);
        heat[y] = heat[y] + random(160, 255);
        //heat[y] = random(160,255);
    }

    // Step 4.  Convert heat to LED colors
    for (int j = 0; j < NUM_LEDS; j++)
    {
        setPixelHeatColor(j, heat[j]);
    }

    showStrip();
    delayscan(SpeedDelay);
}

void setPixelHeatColor(int Pixel, byte temperature)
{
    // Scale 'heat' down from 0-255 to 0-191
    byte t192 = round((temperature / 255.0) * 191);

    // calculate ramp up from
    byte heatramp = t192 & 0x3F; // 0..63
    heatramp <<= 2;              // scale up to 0..252

    // figure out which third of the spectrum we're in:
    if (t192 > 0x80)
    { // hottest
        setPixel(Pixel, 255, 255, heatramp);
    }
    else if (t192 > 0x40)
    { // middle
        setPixel(Pixel, 255, heatramp, 0);
    }
    else
    { // coolest
        setPixel(Pixel, heatramp, 0, 0);
    }
}

void BouncingColoredBalls(int BallCount, byte colors[][3], boolean continuous)
{
    float Gravity = -9.81;
    int StartHeight = 1;

    float Height[BallCount];
    float ImpactVelocityStart = sqrt(-2 * Gravity * StartHeight);
    float ImpactVelocity[BallCount];
    float TimeSinceLastBounce[BallCount];
    int Position[BallCount];
    long ClockTimeSinceLastBounce[BallCount];
    float Dampening[BallCount];
    boolean ballBouncing[BallCount];
    boolean ballsStillBouncing = true;

    for (int i = 0; i < BallCount; i++)
    {
        ClockTimeSinceLastBounce[i] = millis();
        Height[i] = StartHeight;
        Position[i] = 0;
        ImpactVelocity[i] = ImpactVelocityStart;
        TimeSinceLastBounce[i] = 0;
        Dampening[i] = 0.90 - float(i) / pow(BallCount, 2);
        ballBouncing[i] = true;
    }

    while (ballsStillBouncing)
    {
        for (int i = 0; i < BallCount; i++)
        {
            TimeSinceLastBounce[i] = millis() - ClockTimeSinceLastBounce[i];
            Height[i] = 0.5 * Gravity * pow(TimeSinceLastBounce[i] / 1000, 2.0) + ImpactVelocity[i] * TimeSinceLastBounce[i] / 1000;

            if (Height[i] < 0)
            {
                Height[i] = 0;
                ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
                ClockTimeSinceLastBounce[i] = millis();

                if (ImpactVelocity[i] < 0.01)
                {
                    if (continuous)
                    {
                        ImpactVelocity[i] = ImpactVelocityStart;
                    }
                    else
                    {
                        ballBouncing[i] = false;
                    }
                }
            }
            Position[i] = round(Height[i] * (NUM_LEDS - 1) / StartHeight);
        }

        ballsStillBouncing = false; // assume no balls bouncing
        for (int i = 0; i < BallCount; i++)
        {
            setPixel(Position[i], colors[i][0], colors[i][1], colors[i][2]);
            if (ballBouncing[i])
            {
                ballsStillBouncing = true;
            }
        }

        showStrip();
        setAll(0, 0, 0);
    }
}

void meteorRain(int red, int green, int blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay)
{
    setAll(0, 0, 0);

    for (int i = 0; i < NUM_LEDS + NUM_LEDS; i++)
    {

        // fade brightness all LEDs one step
        for (int j = 0; j < NUM_LEDS; j++)
        {
            if ((!meteorRandomDecay) || (random(10) > 5))
            {
                fadeToBlack(j, meteorTrailDecay);
            }
        }

        // draw meteor
        for (int j = 0; j < meteorSize; j++)
        {
            if ((i - j < NUM_LEDS) && (i - j >= 0))
            {
                setPixel(i - j, red, green, blue);
            }
        }

        showStrip();
        delayscan(SpeedDelay);
    }
}

// used by meteorrain
void fadeToBlack(int ledNo, byte fadeValue)
{
#ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;

    oldColor = strip.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r = (r <= 10) ? 0 : (int)r - (r * fadeValue / 256);
    g = (g <= 10) ? 0 : (int)g - (g * fadeValue / 256);
    b = (b <= 10) ? 0 : (int)b - (b * fadeValue / 256);

    strip.setPixelColor(ledNo, r, g, b);
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
    // FastLED
    leds[ledNo].fadeToBlackBy(fadeValue);
#endif
}

// *** REPLACE TO HERE ***

// ***************************************
// ** FastLed/NeoPixel Common Functions **
// ***************************************

// Apply LED color changes
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

// Set a LED color (not yet visible)
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

// Set all LEDs to a given color and apply it (visible)
void setAll(int red, int green, int blue)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        setPixel(i, red, green, blue);
    }
    showStrip();
}

void fadeall()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i].nscale8(250);
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
