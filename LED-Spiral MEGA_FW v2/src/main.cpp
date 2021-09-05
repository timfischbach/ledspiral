//Serialcommunication chars:
//D => DMX Channel Value to ESP
//s => ESP Status (returns o)
//v => ESP Version
//p => ESP reboot (returns o)
//l => LED test
//b => LEDS blackout

#include <avr/wdt.h>         // Watchdog Library
#include <EEPROM.h>          // EEPROM Library
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>             // SPI Library
#include <DMXSerial.h>       // DMXSerial Library by mathertel
#include <Wire.h>
#include <Fonts/FreeSansBold12pt7b.h> // including Fonts from Adafruit GFX Library
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
//variables:
////////////////////////////////
//DEVELOPER MODE
//Skips ESP Check on boot
//PLEASE KEEP IT ON FALSE!
bool devmode = false;
////////////////////////////////
int delay_in_ms = 0;
String ver;
String hwver = "v1.2";
int dTime = 5;
int menuTimeout = 30;
const int button1 = 22;
const int button2 = 24;
const int button3 = 26;
int adress = 1;
String adressStr = "001";
int arrowState = 1;
int Digit;
bool reset_to_start = false;
bool blackout = true;
bool WIFIConnected = false;
uint8_t dold[6] = {0};
uint8_t d[6] = {0};
long timeoutCounter;
//Display pins:
#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8
//Display custom colors:
#define ST77XX_GRAY 0x7BEF
//Prototypes:
void text(int x, int y, String text, uint16_t color);
void Initialising();
void Idle();
void DMXConfig();
void DMXDigit(int n);
void digitsDisplay(int d1, int d2, int d3, int n);
void LEDTest();
void error1();
bool buttonPressed(int b);
void arrowDown();
void DMXcheck();
void Menu(int TitleX, int TitleY, String Title, String Option1, String Option2, String Option3);
void MainMenu();
void DMXMenu();
void DMXDebug();
void number(int x, int y, uint8_t num, uint16_t color);
void SettingsMenu();
void AdvancedMenu();
void DMXPause();
void About();
void reboot();
void DisplayTime();
void resetTimeoutCounter();
bool checkTimeoutCounter(int secs);
void WIFIMenu();
void SuperSecret();
void drawBitmap(int16_t x, int16_t y,
                const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST); //Initialise display

void softwareReset()
{ //Arduino Reset
  wdt_enable(WDTO_15MS);
}

void setup()
{
  MCUSR = 0;                      //watchdog variable set
  wdt_disable();                  //watchdog timer disable
  pinMode(button1, INPUT_PULLUP); //Set button pins
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  tft.initR(INITR_BLACKTAB);    //Set display init color
  tft.setRotation(1);           //Set display rotation landscape mode
  tft.fillScreen(ST77XX_BLACK); //Fill screen black
  Initialising();               //Start Initialising
}

void loop()
{
}

//Initialisiing_____________________________________________________
void Initialising()
{
  tft.fillScreen(ST77XX_WHITE);     //set screen white
  tft.setFont(&FreeSansBold12pt7b); //preset font
  text(8, 35, "LED SPIRAL", ST77XX_BLACK);
  tft.setFont(); //set font
  text(16, 118, "Code by Tim Fischbach", ST77XX_BLACK);
  tft.fillRect(0, 80, 30, 22, ST77XX_GREEN); //loading screen rectangle
  tft.setFont(&FreeSans9pt7b);
  text(20, 95, "Loading Data...", ST77XX_BLACK);
  EEPROM.get(0, adress); //get last saved DMX adress from EEPROM
  EEPROM.get(3, dTime);
  Serial1.begin(1000000); //begin serial to ESP
  delay(1000);
  Serial1.print('s'); //send status request to ESP
  delay(2000);
  text(20, 95, "Loading Data...", ST77XX_WHITE);
  tft.fillRect(0, 80, 60, 22, ST77XX_GREEN);
  text(20, 95, "Booting ESP...", ST77XX_BLACK);
  delay(2000);
  if (devmode == false)
  {
    while (Serial1.read() != 'o')
    {           //if ESP not responding
      error1(); //ESP error
    }

    text(20, 95, "Booting ESP...", ST77XX_WHITE);
    tft.fillRect(0, 80, 100, 22, ST77XX_GREEN);
    text(20, 95, "Getting Version", ST77XX_BLACK);
    Serial1.print('v'); //send version request to ESP
    char verc1, verc2;
    while (Serial1.available() == false)
    {
    }                       //wait for response
    verc1 = Serial1.read(); //save char response
    while (Serial1.available() == false)
    {
    }                                                //wait for response
    verc2 = Serial1.read();                          //save char response
    ver = "v" + String(verc1) + "." + String(verc2); //assembly version chars to version string
    tft.setFont(&FreeSansBold9pt7b);
  }
  text(113, 50, ver, ST77XX_BLACK);
  delay(500);
  tft.setFont(&FreeSans9pt7b);
  text(20, 95, "Getting Version", ST77XX_WHITE);
  tft.fillRect(0, 80, 120, 22, ST77XX_GREEN);
  text(20, 95, "Starting DMX...", ST77XX_BLACK);
  DMXSerial.init(DMXReceiver); //set DMX libaray mode
  DMXSerial.write(adress, 0);  //reset adress ports
  DMXSerial.write(adress + 1, 0);
  DMXSerial.write(adress + 2, 0);
  DMXSerial.write(adress + 3, 0);
  DMXSerial.write(adress + 4, 0);
  DMXSerial.write(adress + 5, 0);
  delay(1000);
  tft.fillRect(0, 80, 160, 22, ST77XX_GREEN);
  text(56, 95, "Done!", ST77XX_BLACK);
  delay(1000);
  Idle();
}
//Idle Screen________________________________________________________________
void Idle()
{
  tft.fillScreen(ST77XX_BLACK); //screen black
  tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
  tft.setFont(&FreeSans9pt7b);
  text(9, 14, "Current DMX Adr.", ST77XX_WHITE);
  tft.setFont(&FreeSans24pt7b);
  if (adress <= 99 and adress > 9)
  { //if adress smaller 99 and bigger 9 -> first digit a zero
    adressStr = "0" + String(adress);
  }
  else if (adress < 10)
  { //if adress  smaller 10 -> first two digits a zero
    adressStr = "00" + String(adress);
  }
  else
  { // else insert no extra zero
    adressStr = String(adress);
  }
  text(44, 80, adressStr, ST77XX_RED);
  tft.setFont();
  text(7, 118, "MENU", ST77XX_GREEN);
  long curTime = millis();
  while (buttonPressed(button1) == false)
  {             //if nothing happening on MEGA
    DMXcheck(); // idle Mode___________________________________________________________________
    if (millis() > curTime + dTime * 60000)
    {
      tft.fillScreen(ST77XX_BLACK); //screen black
      while (buttonPressed(button1) == false and buttonPressed(button2) == false and buttonPressed(button3) == false)
      {
        DMXcheck();
      }
      Idle();
    }
  }
  MainMenu();
}
//Menu Screen______________________________________________
void Menu(int TitleX, int TitleY, String Title, String Option1, String Option2, String Option3)
{
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
  tft.setFont(&FreeSans9pt7b);
  text(TitleX, TitleY, Title, ST77XX_WHITE);
  tft.setFont();
  text(7, 118, "BACK", ST77XX_GREEN);
  text(67, 118, "DOWN", ST77XX_GREEN);
  text(124, 118, "ENTER", ST77XX_GREEN);
  tft.drawLine(0, 47, 160, 47, ST77XX_GRAY);
  tft.drawLine(0, 77, 160, 77, ST77XX_GRAY);
  tft.drawLine(0, 107, 160, 107, ST77XX_GRAY);
  tft.setFont(&FreeSans9pt7b);
  text(40, 38, Option1, ST77XX_WHITE);
  text(40, 68, Option2, ST77XX_WHITE);
  text(40, 98, Option3, ST77XX_WHITE);
  arrowState = 1;                                     //set Cursor State
  tft.fillTriangle(8, 38, 8, 26, 14, 32, ST77XX_RED); //display cursor
  resetTimeoutCounter();
}
//DMX set___________________________________________________
void DMXConfig()
{ //set dmx adress method
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
  tft.setFont(&FreeSans9pt7b);
  text(25, 14, "Set DMX Adr.", ST77XX_WHITE);
  tft.setFont();
  text(7, 118, "UP", ST77XX_GREEN);
  text(67, 118, "DOWN", ST77XX_GREEN);
  text(124, 118, "ENTER", ST77XX_GREEN);
  tft.fillRect(45, 47, 24, 36, ST77XX_GREEN);
  tft.setFont(&FreeSans24pt7b);
  text(44, 80, adressStr, ST77XX_RED);
  DMXDigit(1); //go third mode case (DMX fist digit setup)
}
//DMX digit set___________________________________________________
void DMXDigit(int n)
{
  while (buttonPressed(button3) == false)
  {                                    //if next/ok button pressed
    int Digit1 = (adress / 100U) % 10; // split old ardress to single digits
    int Digit2 = (adress / 10U) % 10;
    int Digit3 = (adress / 1U) % 10;
    int Digits[]{Digit1, Digit2, Digit3}; //save old adress digits to array
    Digit = Digits[n - 1];                //get old digit value from array

    if (buttonPressed(button1) == true)
    {          //if digit up button pressed
      Digit++; // increase digit by one
      if (n == 1 and Digit > 5)
      { //if selected digit is first digit and digit over 5 -> Digit to 0 (dmx cant be larger as 512)
        Digit = 0;
        if (Digits[2] == 0 and Digits[1] == 0 and Digit == 0)
        { //if first + second digit 0 and third digit changed to zero -> DMX set to 001 (DMX adress can't be 000)
          reset_to_start = true;
        }
        else
        { //if not -> 3rd digit can be 0
          Digit = 0;
        }
      }
      else if (n == 2 and Digits[0] == 5 and Digit > 0)
      { //if first digit 5 -> second digit needs to be 0

        Digit = 0;
      }
      else if (n == 3 and Digits[0] == 5 and Digit > 8)
      { //if first digit 5 -> second digit can't be bigger as 8 (5 dmx adresses reserved! can't be bigger as 512)

        Digit = 0;
      }
      else if (n == 3 and Digits[0] == 0 and Digits[1] == 0 and Digit > 9)
      { // if first digit 0 and second digit 0 and 3rd digit bigger as 9 -> 3rd digit is 1 (DMX adress can't be 0)

        Digit = 1;
      }
      else if (n == 3 and Digits[0] == 0 and Digit == 0)
      { // if first digit 0 and current 3rd digit 0 -> 3rd digit 1 (DMX can't be zero)
        Digit = 1;
      }
      else if (Digit > 9)
      { //if digit bigger 9 -> 0 (digit from 0-9)
        Digit = 0;
      }
      if (reset_to_start == false)
      { // Digit = oldDigit ->New Digit set
        Digits[n - 1] = Digit;
      }
      else
      { //else adress 001
        Digits[0] = 0;
        Digits[1] = 0;
        Digits[2] = 1;
        reset_to_start = false;
      }
      digitsDisplay(Digits[0], Digits[1], Digits[2], n); //display and save digits
    }

    if (buttonPressed(button2) == true)
    {          //if digit down button pressed
      Digit--; //old digit - 1
      if (n == 1 and Digit < 0)
      { //if 3rd digit smaller 0 -> digit 5 (DMX max 512)
        Digit = 5;
      }
      else if (n == 2 and Digits[0] == 5 and Digit < 0)
      { //if 2nd digit selected and under 0 and first digit 5 -> 2nd digit 0 (cant be bigger as 508)

        Digit = 0;
      }
      else if (n == 3 and Digits[0] == 5 and Digits[1] == 0 and Digit < 0)
      { //if 3nd digit selected and fist digit 5 and 2nd digit 0 and 3rd digit smaller 0 -> 3rd digit 8 (can't be bigger as 508)

        Digit = 7;
      }
      else if (n == 3 and Digits[0] == 0 and Digits[1] == 0 and Digit < 1)
      { //if 3rd digit selected and 1st digit 0 and 2nd digit 0 and selected digit smaller one -> 9 (digit range 0-9 and adress can't be zero)

        Digit = 9;
      }
      else if (n == 1 and Digits[2] == 0 and Digit == 0)
      { //if 1st digit selected and zero and 3rd digit 0 -> adress 001

        reset_to_start = true;
      }
      else if (Digit < 0)
      { //else if current digit smaller 0 -> 9 (Digit range 0-9)

        Digit = 9;
      }
      if (reset_to_start == false)
      { //if everything ok ->old digit = new digit
        Digits[n - 1] = Digit;
      }
      else
      { //if Number invalid -> 001
        Digits[0] = 0;
        Digits[1] = 0;
        Digits[2] = 1;
        reset_to_start = false;
      }
      digitsDisplay(Digits[0], Digits[1], Digits[2], n); //display and save digits
    }
  }

  tft.fillRect(40, 32, 80, 64, ST77XX_BLACK);
  if (n == 1)
  { //if 1st digit was set -> select 2nd digit
    tft.fillRect(71, 47, 24, 36, ST77XX_GREEN);
    text(44, 80, adressStr, ST77XX_RED);
    DMXDigit(2);
  }
  else if (n == 2)
  { //if 2nd digit was set -> select 3rd digit
    tft.fillRect(97, 47, 24, 36, ST77XX_GREEN);
    text(44, 80, adressStr, ST77XX_RED);
    DMXDigit(3);
  }
  else
  { //else leave adress set mode and go to menu
    tft.fillRect(40, 32, 84, 64, ST77XX_GRAY);
    tft.setFont(&FreeSans24pt7b);
    text(46, 80, "OK", ST77XX_RED);
    delay(1000);
    DMXMenu();
  }
}

void digitsDisplay(int d1, int d2, int d3, int n)
{ //show digits and selected digit
  tft.setFont(&FreeSans24pt7b);
  tft.fillRect(40, 32, 80, 64, ST77XX_BLACK);
  String strd1 = String(d1);
  String strd2 = String(d2);
  String strd3 = String(d3);
  String adressStr1 = String(strd1 + strd2 + strd3);
  adressStr = adressStr1;
  adress = adressStr.toInt();
  EEPROM.put(0, adress); //save current digit to EEPROM
  if (n == 1)
  {
    tft.fillRect(45, 47, 24, 36, ST77XX_GREEN);
  }
  else if (n == 2)
  {
    tft.fillRect(71, 47, 24, 36, ST77XX_GREEN);
  }
  else
  {
    tft.fillRect(97, 47, 24, 36, ST77XX_GREEN);
  }
  tft.setFont(&FreeSans24pt7b);
  text(44, 80, adressStr, ST77XX_RED);
}

//LED Test________________________________________________________
void LEDTest()
{ //test leds
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
  tft.setFont(&FreeSans9pt7b);
  text(43, 14, "LED Test", ST77XX_WHITE);
  text(4, 72, "LED Test running...", ST77XX_WHITE);
  tft.setFont();
  text(7, 118, "BACK", ST77XX_GREEN);
  while (buttonPressed(button1) == false)
  {
    Serial1.print('l');
    delay(500);
  }
  Serial1.print('b');
  Idle();
}

//Error Screen 1_____________________________________________________
void error1()
{

  tft.fillScreen(ST77XX_BLUE);
  tft.setFont();
  text(8, 8, "Error 01:", ST77XX_WHITE);
  text(8, 17, "ESP not responding.", ST77XX_WHITE);
  text(7, 118, "Please reboot the system.", ST77XX_WHITE);
  while (true)
  {
  }
}

//simplified Text method_____________________________________________
void text(int x, int y, String text, uint16_t color)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.println(text);
}
//button press_________________________________
bool buttonPressed(int b)
{
  if (digitalRead(b) == LOW)
  {
    while (digitalRead(b) == LOW)
    {
    }
    delay(20); //debouncing button
    return true;
  }
  else
  {
    return false;
  }
}

//arrow code______________________________________________
void arrowDown()
{
  resetTimeoutCounter();
  arrowState = arrowState + 1;
  if (arrowState == 4)
  {
    arrowState = 1;
  }
  if (arrowState == 1)
  {
    tft.fillTriangle(8, 38, 8, 26, 14, 32, ST77XX_RED);
    tft.fillTriangle(8, 98, 8, 86, 14, 92, ST77XX_BLACK);
  }
  else if (arrowState == 2)
  {
    tft.fillTriangle(8, 68, 8, 56, 14, 62, ST77XX_RED);
    tft.fillTriangle(8, 38, 8, 26, 14, 32, ST77XX_BLACK);
  }
  else
  {
    tft.fillTriangle(8, 98, 8, 86, 14, 92, ST77XX_RED);
    tft.fillTriangle(8, 68, 8, 56, 14, 62, ST77XX_BLACK);
  }
}

void DMXcheck()
{
  bool DMXChanged = false;
  for (int i = 0; i < 6; i++)
  {
    d[i] = DMXSerial.read(adress + i); //read dmx values
  }
  for (int i = 0; i < 6; i++)
  {
    if (d[i] != dold[i])
    {
      dold[i] = d[i];
      DMXChanged = true;
    }
  }
  if (DMXChanged == true)
  {
    if (blackout == true)
    {
      delay(5);
      DMXcheck();
      blackout = false;
    }
    if (d[1] and d[2] and d[3] and d[4] and d[5] == 0)
    {
      blackout = true;
    }
    else
    {
      blackout = false;
    }
    Serial1.print('D'); //tell ESP to send DMX values now
    for (int i = 0; i < 6; i++)
    {
      delay(delay_in_ms);  //delay to prevent data loss
      Serial1.write(d[i]); // send data
    }
  }
}

void MainMenu()
{
  Menu(60, 14, "Menu", "DMX", "LED Test", "Settings"); // execute Menu
  while (buttonPressed(button3) == false)
  {
    if (buttonPressed(button1) == true)
    { //if button 1 pressed
      Idle();
    }
    else if (buttonPressed(button2) == true)
    {              //if button 2 pressed
      arrowDown(); //move Arrow down
    }
    else
    {
      DMXcheck();
      if (checkTimeoutCounter(menuTimeout) == true)
      {
        Idle();
      }
    }
  }
  if (arrowState == 1)
  {
    DMXMenu();
  }
  else if (arrowState == 2)
  {
    LEDTest();
  }
  else
  {
    SettingsMenu();
  }
}

void DMXMenu()
{
  Menu(61, 14, "DMX", "Set DMX Addr.", "DMX Debug", "Pause DMX"); // execute Menu
  while (buttonPressed(button3) == false)
  {
    if (buttonPressed(button1) == true)
    { //if button 1 pressed
      MainMenu();
    }
    else if (buttonPressed(button2) == true)
    {              //if button 2 pressed
      arrowDown(); //move Arrow down
    }
    else
    {
      DMXcheck();
      if (checkTimeoutCounter(menuTimeout) == true)
      {
        Idle();
      }
    }
  }
  if (arrowState == 1)
  {
    DMXConfig();
  }
  else if (arrowState == 2)
  {
    DMXDebug();
  }
  else
  {
    DMXPause();
  }
}

void DMXDebug()
{
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
  tft.setFont(&FreeSans9pt7b);
  text(28, 14, "DMX Debug", ST77XX_WHITE);
  String CurAdr = "Cur. DMX Adr.:" + adressStr;
  text(4, 35, CurAdr, ST77XX_WHITE);
  text(4, 55, "Channel Values:", ST77XX_WHITE);
  tft.setFont();
  text(7, 118, "BACK", ST77XX_GREEN);
  while (buttonPressed(button1) == false)
  {
    tft.setFont(&FreeSans9pt7b);
    tft.fillRect(0, 60, 160, 54, ST77XX_BLACK);
    text(4, 80, "1:", ST77XX_WHITE);
    text(59, 80, "2:", ST77XX_WHITE);
    text(109, 80, "3:", ST77XX_WHITE);
    text(4, 100, "4:", ST77XX_WHITE);
    text(59, 100, "5:", ST77XX_WHITE);
    text(109, 100, "6:", ST77XX_WHITE);
    number(19, 80, d[0], ST77XX_RED);
    number(74, 80, d[1], ST77XX_YELLOW);
    number(124, 80, d[2], ST77XX_GREEN);
    number(19, 100, d[3], ST77XX_CYAN);
    number(74, 100, d[4], ST77XX_BLUE);
    number(124, 100, d[5], ST77XX_MAGENTA);
    for (int i = 0; i < 200; i++)
    {
      delay(1);
      DMXcheck();
    }
  }
  DMXMenu();
}

void number(int x, int y, uint8_t num, uint16_t color)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.println(num);
}

void SettingsMenu()
{
  Menu(45, 14, "Settings", "WIFI & Update", "Advanced", "About"); // execute Menu
  while (buttonPressed(button3) == false)
  {
    if (buttonPressed(button1) == true)
    { //if button 1 pressed
      MainMenu();
    }
    else if (buttonPressed(button2) == true)
    {              //if button 2 pressed
      arrowDown(); //move Arrow down
    }
    else
    {
      DMXcheck();
      if (checkTimeoutCounter(menuTimeout) == true)
      {
        Idle();
      }
    }
  }
  if (arrowState == 1)
  {
    WIFIMenu();
  }
  else if (arrowState == 2)
  {
    AdvancedMenu();
  }
  else
  {
    About();
  }
}

void AdvancedMenu()
{
  Menu(45, 14, "Advanced", "Display Time", "Reboot", ":)"); // execute Menu
  while (buttonPressed(button3) == false)
  {
    if (buttonPressed(button1) == true)
    { //if button 1 pressed
      SettingsMenu();
    }
    else if (buttonPressed(button2) == true)
    {              //if button 2 pressed
      arrowDown(); //move Arrow down
    }
    else
    {
      DMXcheck();
      if (checkTimeoutCounter(menuTimeout) == true)
      {
        Idle();
      }
    }
  }
  if (arrowState == 1)
  {
    DisplayTime();
  }
  else if (arrowState == 2)
  {
    reboot();
  }
  else
  {
    SuperSecret();
  }
}

void About()
{
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
  tft.setFont(&FreeSans9pt7b);
  text(38, 14, "About", ST77XX_WHITE);
  text(4, 35, "The LED Spiral", ST77XX_RED);
  text(4, 55, "Project by", ST77XX_RED);
  text(4, 75, "Tim Fischbach", ST77XX_RED);
  tft.setFont();
  text(4, 79, "Version: " + ver, ST77XX_WHITE);
  text(4, 88, "HW Version: " + hwver + " legacy", ST77XX_WHITE);
  text(4, 97, "github.com", ST77XX_CYAN);
  text(4, 106, "/timfischbach/ledspiral", ST77XX_CYAN);
  text(7, 118, "BACK", ST77XX_GREEN);
  while (buttonPressed(button1) == false)
  {
    DMXcheck();
  }
  SettingsMenu();
}

void DMXPause()
{
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
  tft.setFont(&FreeSans9pt7b);
  text(24, 14, "Pause DMX", ST77XX_WHITE);
  text(4, 35, "DMX is paused", ST77XX_WHITE);
  text(4, 75, "Press Back to ", ST77XX_WHITE);
  text(4, 95, "unpause it", ST77XX_WHITE);
  tft.setFont();
  text(7, 118, "BACK", ST77XX_GREEN);
  while (buttonPressed(button1) == false)
  {
  }
  DMXMenu();
}

void reboot()
{
  tft.fillScreen(ST77XX_BLACK);
  tft.setFont();
  text(8, 8, "THE LED SPIRAL PROJECT", ST77XX_GREEN);
  text(8, 17, "BY Tim Fischbach", ST77XX_GREEN);
  text(8, 26, "Version: " + ver, ST77XX_GREEN);
  text(8, 35, "HW Version: " + hwver + " legacy", ST77XX_GREEN);
  delay(500);
  text(8, 53, "Reboot initiated! Cya! :)", ST77XX_GREEN);
  delay(1000);
  text(8, 62, "Rebooting ESP...", ST77XX_GREEN);
  Serial1.print('r');
  delay(3000);
  text(8, 71, "Rebooting AtMega2560...", ST77XX_GREEN);
  delay(2000);
  softwareReset();
}

void DisplayTime()
{
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
  tft.setFont(&FreeSans9pt7b);
  text(25, 14, "Display Time", ST77XX_WHITE);
  text(4, 35, "Time in Minutes", ST77XX_WHITE);
  text(4, 55, "till the Display", ST77XX_WHITE);
  text(4, 75, "turns black:", ST77XX_WHITE);
  number(4, 98, dTime, ST77XX_RED);
  text(20, 98, "Minutes", ST77XX_RED);
  tft.setFont();
  text(7, 118, "UP", ST77XX_GREEN);
  text(67, 118, "DOWN", ST77XX_GREEN);
  text(124, 118, "ENTER", ST77XX_GREEN);
  tft.setFont(&FreeSans9pt7b);
  while (buttonPressed(button3) == false)
  {
    if (buttonPressed(button1) == true)
    {
      number(4, 98, dTime, ST77XX_BLACK);
      dTime++;
      if (dTime > 5)
      {
        dTime = 1;
      }
      number(4, 98, dTime, ST77XX_RED);
    }
    else if (buttonPressed(button2) == true)
    {
      number(4, 98, dTime, ST77XX_BLACK);
      dTime--;
      if (dTime < 1)
      {
        dTime = 5;
      }
      number(4, 98, dTime, ST77XX_RED);
    }
    else
    {
      DMXcheck();
    }
  }
  tft.fillRect(40, 32, 84, 64, ST77XX_GRAY);
  tft.setFont(&FreeSans24pt7b);
  EEPROM.put(3, dTime);
  text(46, 80, "OK", ST77XX_RED);
  delay(1000);
  AdvancedMenu();
}

void resetTimeoutCounter()
{
  timeoutCounter = millis();
}

bool checkTimeoutCounter(int secs)
{
  secs = secs * 1000;
  if (timeoutCounter + secs < millis())
  {
    return true;
  }
  else
  {
    return false;
  }
}

void WIFIMenu()
{
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
  tft.setFont(&FreeSans9pt7b);
  Serial1.print('w');
  text(18, 14, "WIFI & Update", ST77XX_WHITE);
  text(20, 35, "!!!WARNING!!!", ST77XX_RED);
  tft.setFont();
  text(4, 55, "DMX WILL BE PAUSED!", ST77XX_YELLOW);
  text(4, 75, "CONTINUE?", ST77XX_YELLOW);
  tft.setFont();
  text(10, 118, "NO", ST77XX_GREEN);
  text(130, 118, "YES", ST77XX_GREEN);
  while (buttonPressed(button3) == false)
  {
    if (buttonPressed(button1) == true)
    {
      SettingsMenu();
    }
    DMXcheck();
  }
  if (WIFIConnected == true)
  {
  wificon:
    tft.fillScreen(ST77XX_BLACK);
    tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
    tft.setFont(&FreeSans9pt7b);
    Serial1.write('u');
    text(18, 14, "WIFI & Update", ST77XX_WHITE);
    text(4, 35, "Searching for", ST77XX_WHITE);
    text(4, 55, "Updates...", ST77XX_WHITE);
    text(4, 75, "This will take", ST77XX_WHITE);
    text(4, 95, "~20 Seconds", ST77XX_WHITE);
    tft.setFont();
    text(7, 118, "BACK", ST77XX_GREEN);
    while (buttonPressed(button1) == false)
    {
      if (Serial1.read() == 'e')
      {
        tft.fillScreen(ST77XX_BLACK);
        tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
        tft.setFont(&FreeSans9pt7b);
        text(18, 14, "WIFI & Update", ST77XX_WHITE);
        text(4, 35, "No Updates", ST77XX_WHITE);
        text(4, 55, "available!", ST77XX_WHITE);
        delay(3000);
        SettingsMenu();
      }
      else if (Serial1.read() == 'u')
      {
        tft.fillScreen(ST77XX_BLACK);
        tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
        tft.setFont(&FreeSans9pt7b);
        text(18, 14, "WIFI & Update", ST77XX_WHITE);
        text(10, 35, "Update available!", ST77XX_GREEN);
        text(30, 55, "Install?", ST77XX_WHITE);
        tft.setFont();
        text(10, 118, "NO", ST77XX_GREEN);
        text(130, 118, "YES", ST77XX_GREEN);
        while (buttonPressed(button3) == false)
        {
          if (buttonPressed(button1) == true)
          {
            SettingsMenu();
          }
        }
        Serial1.flush();
        Serial1.write('i');
        tft.fillScreen(ST77XX_BLACK);
        tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
        tft.setFont(&FreeSans9pt7b);
        text(44, 14, "Updating...", ST77XX_WHITE);
        text(20, 35, "Downloading...", ST77XX_YELLOW);
        delay(20000);
        while (Serial1.available())
        {
          Serial1.read();
        }
        Serial1.print('v'); //send version request to ESP
        char verc1, verc2;
        while (Serial1.available() == false)
        {
        }                       //wait for response
        verc1 = Serial1.read(); //save char response
        while (Serial1.available() == false)
        {
        }                                                             //wait for response
        verc2 = Serial1.read();                                       //save char response
        String verupdate = "v" + String(verc1) + "." + String(verc2); //assembly version chars to version string

        if (verupdate == ver)
        {
          text(20, 35, "Downloading...", ST77XX_BLACK);
          text(4, 35, "ERROR:", ST77XX_RED);
          text(4, 55, "HTTP ERROR", ST77XX_RED);
          tft.setFont();
          text(4, 100, "Please try again later!", ST77XX_WHITE);
          text(132, 118, "OK", ST77XX_GREEN);
          while (buttonPressed(button3) == false)
          {
          }
          SettingsMenu();
        }
        else if (verupdate != ver)
        {
          tft.fillScreen(ST77XX_BLACK);
          tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
          tft.setFont(&FreeSans9pt7b);
          text(44, 14, "Updating...", ST77XX_WHITE);
          text(10, 35, "SUCCESS!", ST77XX_GREEN);
          text(10, 55, "UPD. TO VER:", ST77XX_CYAN);
          text(10, 75, verupdate, ST77XX_GREEN);
          text(10, 95, "Reboot...", ST77XX_RED);
          delay(5000);
          reboot();
        }
      }
    }
    SettingsMenu();
  }
  else
  {
    tft.fillScreen(ST77XX_BLACK);
    tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
    tft.setFont(&FreeSans9pt7b);
    Serial1.print('w');
    text(18, 14, "WIFI & Update", ST77XX_WHITE);
    text(10, 35, "Instructions:", ST77XX_WHITE);
    tft.setFont();
    text(4, 55, "Create a Hotspot called:", ST77XX_WHITE);
    text(4, 64, "ledspiral", ST77XX_WHITE);
    text(4, 73, "Password: ledspiral", ST77XX_WHITE);
    text(10, 118, "Abort in:", ST77XX_WHITE);
    for (int i = 90; i > 0; i--)
    {
      if (Serial1.read() == 'o')
      {
        WIFIConnected = true;
        tft.fillScreen(ST77XX_BLACK);
        tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
        tft.setFont(&FreeSans9pt7b);
        Serial1.print('w');
        text(18, 14, "WIFI & Update", ST77XX_WHITE);
        text(25, 70, "Connected!", ST77XX_GREEN);
        delay(2000);
        goto wificon;
      }
      else if (Serial1.read() == 'e')
      {
        SettingsMenu();
      }
      tft.fillRect(60, 108, 20, 20, ST77XX_BLACK);
      number(60, 118, i, ST77XX_RED);
      delay(1000);
    }
  }
}

void SuperSecret()
{
  tft.fillScreen(ST77XX_BLACK);
  tft.setFont(&FreeSans9pt7b);
  text(4, 20, "Congrats!", ST77XX_RED);
  text(4, 35, "You", ST77XX_YELLOW);
  text(4, 50, "have", ST77XX_GREEN);
  text(4, 65, "found", ST77XX_CYAN);
  text(4, 80, "a", ST77XX_BLUE);
  text(4, 95, "easteregg :)", ST77XX_MAGENTA);
  tft.setFont();
  text(7, 118, "BACK", ST77XX_GREEN);
  while (buttonPressed(button1) == false)
  {
    DMXcheck();
  }
  AdvancedMenu();
}
