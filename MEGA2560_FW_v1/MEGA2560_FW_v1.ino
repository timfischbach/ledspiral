#include <avr/wdt.h>         // Watchdog Library
#include <EEPROM.h>          // EEPROM Library
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>             // SPI Library
#include <DMXSerial.h>       // DMXSerial Library by  mathertel
#include <Fonts/FreeSansBold12pt7b.h> // including Fonts from Adafruit GFX Library
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
//variables:
int delay_in_ms = 1;
String ver;
const int button1 = 22;
const int button2 = 24;
const int button3 = 26;
int adress = 1;
int mode = 1;
String adressStr = "001";
int arrowState = 1;
bool Mrun = false;
bool Frun = false;
int Digit;
uint8_t in[5];
char out[3];
bool change;
bool reset_to_start = false;
uint8_t d1old, d2old, d3old, d4old, d5old;
//Display pins:
#define TFT_CS          10
#define TFT_RST          9
#define TFT_DC           8
//Display custom colors:
#define ST77XX_GRAY 0x7BEF
//Prototypes:
void text(int x, int y, String text, uint16_t color);
void Initialising();
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST); //Initialise display

void softwareReset( ) {
  wdt_enable( WDTO_15MS);
}

void setup() {
  MCUSR = 0;
  wdt_disable();
  pinMode(button1, INPUT_PULLUP); //Set button pins
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  tft.initR(INITR_BLACKTAB);  //Set display init color
  tft.setRotation(1);         //Set display ro landscape mode
  tft.fillScreen(ST77XX_BLACK); //Fill screen black
  Initialising(); //Start Initialising
}

void loop() {
  switch (mode) {
    case 1:
      if (Frun == false) {
        MainFrame();
      }
      while (buttonPressed(button1) == false) {
        //if(compareDMX() == true) {

        uint8_t d1 = DMXSerial.read(adress);
        uint8_t d2 = DMXSerial.read(adress + 1);
        uint8_t d3 = DMXSerial.read(adress + 2);
        uint8_t d4 = DMXSerial.read(adress + 3);
        uint8_t d5 = DMXSerial.read(adress + 4);
        if (d1 != d1old or d2 != d2old or d3 != d3old or d4 != d4old or d5 != d5old) {
          d1old = d1;
          d2old = d2;
          d3old = d3;
          d4old = d4;
          d5old = d5;
          Serial1.print('D');
          delay(delay_in_ms);
          Serial1.write(d1);
          delay(delay_in_ms);
          Serial1.write(d2);
          delay(delay_in_ms);
          Serial1.write(d3);
          delay(delay_in_ms);
          Serial1.write(d4);
          delay(delay_in_ms);
          Serial1.write(d5);
          delay(delay_in_ms);
        }
      }
      mode = 2;
      Mrun = false;
      break;
    case 2:
      if (Mrun == false) {
        Menu();
      }
      if (buttonPressed(button1) == true) {
        mode = 1;
        Frun = false;
      }
      else if (buttonPressed(button2) == true) {
        arrow('d');
      }
      else if (buttonPressed(button3) == true) {
        arrow('e');
      }
      else {
      }
      break;
    case 3:
      DMXDigit(1);
      break;
    case 4:
      DMXDigit(2);
      break;
    case 5:
      DMXDigit(3);
      break;
  }
}




void Initialising() {
  tft.fillScreen(ST77XX_WHITE);
  tft.setFont(&FreeSansBold12pt7b);
  text(8, 35, "LED SPIRAL", ST77XX_BLACK);
  tft.setFont();
  text(16, 118, "Code by Tim Fischbach", ST77XX_BLACK);
  tft.fillRect(0, 80, 60, 22, ST77XX_GREEN);
  tft.setFont(&FreeSans9pt7b);
  text(20, 95, "Booting ESP...", ST77XX_BLACK);
  Serial1.begin(115200);
  delay(1000);
  Serial1.print('s');
  delay(3000);
  tft.fillRect(0, 80, 80, 22, ST77XX_GREEN);
  text(20, 95, "Booting ESP...", ST77XX_BLACK);
  delay(3000);
  while (Serial1.read() != 'o') {
    error1();
  }
  text(20, 95, "Booting ESP...", ST77XX_WHITE);
  tft.fillRect(0, 80, 100, 22, ST77XX_GREEN);
  text(20, 95, "Getting Version", ST77XX_BLACK);
  Serial1.print('v');
  char verc1, verc2;
  while (Serial1.available() == false) {}
  verc1 = Serial1.read();
  while (Serial1.available() == false) {}
  verc2 = Serial1.read();
  ver = "v" + String(verc1) + "." + String(verc2);
  tft.setFont(&FreeSansBold9pt7b);
  text(113, 50, ver, ST77XX_BLACK);
  delay(500);
  tft.setFont(&FreeSans9pt7b);
  text(20, 95, "Getting Version", ST77XX_WHITE);
  tft.fillRect(0, 80, 130, 22, ST77XX_GREEN);
  text(20, 95, "Starting DMX...", ST77XX_BLACK);
  DMXSerial.init(DMXReceiver);
  DMXSerial.write(adress, 0);
  DMXSerial.write(adress + 1, 0);
  DMXSerial.write(adress + 2, 0);
  DMXSerial.write(adress + 3, 0);
  DMXSerial.write(adress + 4, 0);
  EEPROM.get(0, adress);
  delay(1000);
  while (!Serial1) {
    error2();
  }
  tft.fillRect(0, 80, 160, 22, ST77XX_GREEN);
  text(56, 95, "Done!", ST77XX_BLACK);
  delay(1000);
}

void MainFrame() {
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
  tft.setFont(&FreeSans9pt7b);
  text(9, 14, "Current DMX Adr.", ST77XX_WHITE);
  tft.setFont(&FreeSans24pt7b);
  if (adress <= 99 and adress > 9) {
    adressStr = "0" + String(adress);
  }
  else if (adress <= 9) {
    adressStr = "00" + String(adress);
  }
  else {
    adressStr = String(adress);
  }
  text(44, 80, adressStr, ST77XX_RED);
  tft.setFont();
  text(7, 118, "MENU", ST77XX_GREEN);
  Frun = true;
}

void Menu() {
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
  tft.setFont(&FreeSans9pt7b);
  text(60, 14, "Menu", ST77XX_WHITE);
  tft.setFont();
  text(7, 118, "BACK", ST77XX_GREEN);
  text(67, 118, "DOWN", ST77XX_GREEN);
  text(124, 118, "ENTER", ST77XX_GREEN);
  tft.drawLine(0, 47, 160, 47, ST77XX_GRAY);
  tft.drawLine(0, 77, 160, 77, ST77XX_GRAY);
  tft.drawLine(0, 107, 160, 107, ST77XX_GRAY);
  tft.setFont(&FreeSans9pt7b);
  text(40, 38, "Set DMX Adr.", ST77XX_WHITE);
  text(40, 68, "OTA Mode", ST77XX_WHITE);
  text(40, 98, "LED Test", ST77XX_WHITE);
  arrowState = 1;
  tft.fillTriangle(8, 38, 8, 26, 14, 32, ST77XX_RED);
  Mrun = true;
}
void DMXConfig() {
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
  mode = 3;

}

void DMXDigit(int n) {
  int Digit1 = (adress / 100U) % 10;
  int Digit2 = (adress / 10U) % 10;
  int Digit3 = (adress / 1U) % 10;
  int Digits[] {Digit1, Digit2, Digit3};
  Digit = Digits[n - 1];

  if (buttonPressed(button1) == true) {
    Digit++;
    if (n == 1 and Digit > 5) {
      Digit = 0;
      if (Digits[2] == 0 and Digits[1] == 0) {
        if (Digit == 0) {
          reset_to_start = true;
        }
      } else {
        Digit = 0;
      }
    }
    else if (n == 2 and Digits[0] == 5) {
      if (Digit > 0) {
        Digit = 0;
      }
    }
    else if(n == 3 and Digits[0] == 5) {
      if (Digit > 8) {
        Digit = 0;
      }
    }
    else if (n == 3 and Digits[0] == 0 and Digits[1] == 0) {
      if (Digit > 9) {
        Digit = 1;
      }
    }
    else if (n == 3 and Digits[0] == 0 and Digit == 0) {
      Digit = 1;
    }
    else {
      if (Digit > 9) {
        Digit = 0;
      }
    }
    if (reset_to_start == false) {
      Digits[n - 1] = Digit;
    }
    else {
      Digits[0] = 0;
      Digits[1] = 0;
      Digits[2] = 1;
      reset_to_start = false;
    }
    digitsDisplay(Digits[0], Digits[1], Digits[2], n);
  }

  if (buttonPressed(button2) == true) {
    Digit--;
    if (n == 1 and Digit< 0) {
      Digit = 5;
    }
    else if (n == 2 and Digits[0] == 5) {
      if (Digit < 0) {
        Digit = 1;
      }
    }
    else if (n == 3 and Digits[0] == 5 and Digits[1] == 1) {
      if (Digit < 0) {
        Digit = 2;
      }
    }
    else if (n == 3 and Digits[0] == 0 and Digits[1] == 0) {
      if (Digit < 1) {
        Digit = 9;
      }
    }
    else if (n == 1 and Digits[2] == 0) {
      if (Digit == 0) {
        reset_to_start = true;
      }
    }
    else {
      if (Digit < 0) {
        Digit = 9;
      }
    }
    if (reset_to_start == false) {
      Digits[n - 1] = Digit;
    }
    else {
      Digits[0] = 0;
      Digits[1] = 0;
      Digits[2] = 1;
      reset_to_start = false;
    }
    digitsDisplay(Digits[0], Digits[1], Digits[2], n);
  }
  if (buttonPressed(button3) == true) {
    tft.fillRect(40, 32, 80, 64, ST77XX_BLACK);
    if (n == 1) {
      tft.fillRect(71, 47, 24, 36, ST77XX_GREEN);
      text(44, 80, adressStr, ST77XX_RED);
      mode = 4;
    }
    else if (n == 2) {
      tft.fillRect(97, 47, 24, 36, ST77XX_GREEN);
      text(44, 80, adressStr, ST77XX_RED);
      mode = 5;
    }
    else {
      tft.fillRect(40, 32, 84, 64, ST77XX_GRAY);
      tft.setFont(&FreeSans24pt7b);
      text(46, 80, "OK", ST77XX_RED);
      delay(1000);
      mode = 1;
      Frun = false;

    }
  }
}

void digitsDisplay(int d1, int d2, int d3, int n) {
  tft.setFont(&FreeSans24pt7b);
  tft.fillRect(40, 32, 80, 64, ST77XX_BLACK);
  String strd1 = String(d1);
  String strd2 = String(d2);
  String strd3 = String(d3);
  String adressStr1 = String(strd1 + strd2 + strd3);
  adressStr = adressStr1;
  adress = adressStr.toInt();
  EEPROM.put(0, adress);
  if (n == 1) {
    tft.fillRect(45, 47, 24, 36, ST77XX_GREEN);
  }
  else if (n == 2) {
    tft.fillRect(71, 47, 24, 36, ST77XX_GREEN);
  }
  else {
    tft.fillRect(97, 47, 24, 36, ST77XX_GREEN);
  }
  tft.setFont(&FreeSans24pt7b);
  text(44, 80, adressStr, ST77XX_RED);
}

void OTAmode() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setFont();
  text(8, 8, "OTA MODE ACTIVATED!", ST77XX_GREEN);
  delay(1000);
  text(8, 24, "by Tim Fischbach", ST77XX_GREEN);
  text(8, 33, "Version: " + ver, ST77XX_GREEN);
  text(8, 42, "Stopping DMX...", ST77XX_GREEN);
  delay(500);
  text(8, 51, "Send ESP OTA Request...", ST77XX_GREEN);
  Serial1.print('p');
  while (Serial1.read() == 'o') {}
  text(8, 60, "OK! Rebooting ESP...", ST77XX_GREEN);
  Serial1.print('r');
  delay(9000);
  text(8, 69, "ESP OTA Online", ST77XX_GREEN);
  text(8, 78, "Password: admin", ST77XX_GREEN);

  tft.setFont();
  text(7, 118, "REBOOT", ST77XX_GREEN);
  while (buttonPressed(button1) == false) {}
  text(7, 118, "REBOOT", ST77XX_BLACK);
  text(8, 96, "Rebooting...", ST77XX_GREEN);
  delay(2000);
  softwareReset();



}

void LEDTest() {
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRect(0, 0, 160, 17, ST77XX_GRAY);
  tft.setFont(&FreeSans9pt7b);
  text(43, 14, "LED Test", ST77XX_WHITE);
  text(4, 72, "LED Test running...", ST77XX_WHITE);
  tft.setFont();
  text(7, 118, "BACK", ST77XX_GREEN);
  while (buttonPressed(button1) == false) {
  Serial1.print('l');
  delay(500);
  }
  Serial1.print('b');
  mode = 1;
  Frun = false;
}


void error1() {
  tft.fillScreen(ST77XX_BLUE);
  tft.setFont();
  text(8, 8, "Error 01:", ST77XX_WHITE);
  text(8, 17, "ESP not responding.", ST77XX_WHITE);
  text(7, 118, "Please reboot the system.", ST77XX_WHITE);
  while (true) {}
}

void error2() {
  tft.fillScreen(ST77XX_BLUE);
  tft.setFont();
  text(8, 8, "Error 02:", ST77XX_WHITE);
  text(8, 17, "DMX Chip not responding.", ST77XX_WHITE);
  text(7, 118, "Please reboot the system.", ST77XX_WHITE);
  while (true) {}
}


void text(int x, int y, String text, uint16_t color) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.println(text);
}

bool buttonPressed(int b) {
  if (digitalRead(b) == LOW) {
one:
    if (digitalRead(b) == HIGH) {
      delay(20);
      return true;
    }
    else {
      goto one;
    }
  }
  else {
    return false;
  }
}


void arrow(char c) {
  switch (c) {
    case 'd':
      arrowState = arrowState + 1;
      if (arrowState == 4) {
        arrowState = 1;
      }
      if (arrowState == 1) {
        tft.fillTriangle(8, 38, 8, 26, 14, 32, ST77XX_RED);
        tft.fillTriangle(8, 98, 8, 86, 14, 92, ST77XX_BLACK);
      }
      else if (arrowState == 2) {
        tft.fillTriangle(8, 68, 8, 56, 14, 62, ST77XX_RED);
        tft.fillTriangle(8, 38, 8, 26, 14, 32, ST77XX_BLACK);
      }
      else {
        tft.fillTriangle(8, 98, 8, 86, 14, 92, ST77XX_RED);
        tft.fillTriangle(8, 68, 8, 56, 14, 62, ST77XX_BLACK);
      }
      break;
    case 'e':
      if (arrowState == 1) {
        DMXConfig();
      }
      if (arrowState == 2) {
        OTAmode();
      }
      if (arrowState == 3) {
        LEDTest();
      }
      break;
  }
}
