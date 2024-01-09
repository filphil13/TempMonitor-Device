#ifndef TEMPMONITOR_DEVICE_H
#define TEMPMONITOR_DEVICE_H

#include <ArduinoJson.h>   // Library for handling JSON data
#include <Preferences.h>   // Library for storing preferences in ESP32
#include <DHT.h>           // Library for DHT temperature and humidity sensor
#include "WiFi.h"          // Library for WiFi connectivity
#include <HTTPClient.h>    // Library for making HTTP requests

// SENSOR VARIABLES
#define DHTPIN 4          // Pin number for DHT sensor
#define DHTTYPE DHT22     // Type of DHT sensor
DHT dht(DHTPIN, DHTTYPE); // DHT sensor object
float TEMPERATURE = 0.0f; // Variable to store temperature
float HUMIDITY = 0.0f;    // Variable to store humidity

// TEMPERATURE SCANNING INTERVAL
unsigned long lastTime = millis();   // Variable to store last time temperature was scanned
unsigned long lastTime2 = millis();  // Variable to store last time data was sent
unsigned long timerDelay = 5000;     // Delay between temperature scans

// SYSTEM VARIABLES
#define ONBOARD_LED 2             // Pin number for onboard LED
#define BAUD_RATE 115200          // Serial communication baud rate
#define SCAN_INTERVAL 5000        // Interval for scanning WiFi networks
#define WIFI_TIMEOUT 10000        // Timeout for connecting to WiFi
#define WIFI_RECONNECT_TIMEOUT 5000 // Timeout for reconnecting to WiFi
Preferences preferences;         // Object for storing preferences

// WIFI VARIABLES
#define WIFI_MODE WIFI_STA     // WiFi mode (station mode)
String SSID;                  // WiFi SSID
String PASSWORD;              // WiFi password
String SENSOR_NAME;           // Name of the sensor
String URL;                   // URL for sending data
WiFiClient client;            // WiFi client object
HTTPClient http;              // HTTP client object

// WIFI FUNCTIONS
bool setupWiFi();              // Function to setup WiFi connection
void changeWifiCreds(String, String); // Function to change WiFi credentials
void scanWifi();               // Function to scan available WiFi networks

// TEMP DATA FUNCTIONS
void updateData();             // Function to update temperature and humidity data
void sendData();               // Function to send data to the server
void changeURL(String);        // Function to change the server URL
void changeName(String);       // Function to change the sensor name

// SYSTEM FUNCTIONS
void saveCreds();              // Function to save WiFi credentials
void loadCreds();              // Function to load WiFi credentials
void printCreds();             // Function to print WiFi credentials
void printData();              // Function to print temperature and humidity data
void blinkLED(int);            // Function to blink the onboard LED
void printNetworkStatus();     // Function to print network status
void SPIMenu();                // Function to display SPI menu

void(* resetFunc) (void) = 0;  // Function pointer to reset the device

#endif