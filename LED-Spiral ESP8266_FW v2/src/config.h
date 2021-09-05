#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

extern String VERSION;

// EEPROM ADRESS DEFINE
extern int RecoveryAdress;
extern int SSIDAdress; //String compatibility? OTA Stable?
extern int WiFiPassAdress;

#endif