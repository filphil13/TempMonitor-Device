#ifndef TEMPMONITOR-DEVICE
#define TEMPMONITOR-DEVICE
  /* Include guard */

#include <ArduinoJson.h>
#include <Preferences.h>
#include <DHT.h>
#include "WiFi.h"
#include <HTTPClient.h>

//SENSOR VARIABLES
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float TEMPERATURE = 0.0f;
float HUMIDITY = 0.0f;

//TEMPERATURE SCANNING INTERVAL
unsigned long lastTime = millis();
unsigned long timerDelay = 5000;

//SYSTEM VARIABLES
#define ONBOARD_LED  2
#define BAUD_RATE 115200
#define SCAN_INTERVAL 5000
Preferences preferences;

//WIFI VARIABLES
#define WIFI_MODE WIFI_STA
String SSID;
String PASSWORD;
String IP = "http://temp-monitor-a38f32c02c5e.herokuapp.com/updateSensor";
WiFiClient client;
HTTPClient http;
String SENSOR_NAME = "Test Sensor 1";

//WIFI FUNCTIONS
void setupWiFi();
void changeWifiCreds(String, String);
void scanWifi();

//TEMP DATA FUNCTIONS
void updateData();
void sendData();

// SYSTEM FUNCTIONS
void saveCreds();
void loadCreds();
void printData();
void blinkLED(int);
void printNetworkStatus();
void SPIMenu();

void(* resetFunc) (void) = 0;
#endif