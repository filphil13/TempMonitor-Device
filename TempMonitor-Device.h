#ifndef TEMPMONITOR_DEVICE_H
#define TEMPMONITOR_DEVICE_H

#include <ArduinoJson.h>   // Library for handling JSON data
#include <Preferences.h>   // Library for storing preferences in ESP32
#include <DHT.h>           // Library for DHT temperature and humidity sensor
#include "WiFi.h"          // Library for WiFi connectivity
#include <HTTPClient.h>    // Library for making HTTP requests
#include <WiFiClientSecure.h>
// SENSOR VARIABLES
#define DHTPIN 4          // Pin number for DHT sensor
#define DHTTYPE DHT22     // Type of DHT sensor
DHT dht(DHTPIN, DHTTYPE); // DHT sensor object
float TEMPERATURE = 0.0f; // Variable to store temperature
float HUMIDITY = 0.0f;    // Variable to store humidity

// TEMPERATURE SCANNING INTERVAL
unsigned long lastTime = millis();   // Variable to store last time temperature was scanned
unsigned long lastTime2 = millis();  // Variable to store last time data was sent
unsigned long timerDelay = 5000*60;     // Delay between temperature scans

// SYSTEM VARIABLES
#define ONBOARD_LED 2             // Pin number for onboard LED
#define BAUD_RATE 115200          // Serial communication baud rate
#define SCAN_INTERVAL 5000        // Interval for scanning WiFi networks
#define WIFI_TIMEOUT 10000        // Timeout for connecting to WiFi
#define WIFI_RECONNECT_TIMEOUT 5000 // Timeout for reconnecting to WiFi
Preferences preferences;         // Object for storing preferences

// WIFI VARIABLES
#define WIFI_MODE WIFI_STA      // WiFi mode (station mode)
String SSID;                    // WiFi SSID
String PASSWORD;                // WiFi password
String SENSOR_NAME;             // Name of the sensor
String URL;                     // URL for sending data
String USER_TOKEN;              // User token for authentication
String PORT = "443";            // Port for sending data
HTTPClient http;                // HTTP client object
WiFiClientSecure client;        // WiFi client object

const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"LS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tDQpNSUlGS3pDQ0JOQ2dBd0lCQWdJ\n"\
"UUN1L25MemFCUlRBeG9RWFVPRGprMmpBS0JnZ3Foa2pPUFFRREFqQktNUXN3DQpD\n"\
"UVlEVlFRR0V3SlZVekVaTUJjR0ExVUVDaE1RUTJ4dmRXUm1iR0Z5WlN3Z1NXNWpM\n"\
"akVnTUI0R0ExVUVBeE1YDQpRMnh2ZFdSbWJHRnlaU0JKYm1NZ1JVTkRJRU5CTFRN\n"\
"d0hoY05Nak13T1RFM01EQXdNREF3V2hjTk1qUXdPVEUyDQpNak0xT1RVNVdqQnlN\n"\
"UXN3Q1FZRFZRUUdFd0pWVXpFVE1CRUdBMVVFQ0JNS1EyRnNhV1p2Y201cFlURVdN\n"\
"QlFHDQpBMVVFQnhNTlUyRnVJRVp5WVc1amFYTmpiekVaTUJjR0ExVUVDaE1RUTJ4\n"\
"dmRXUm1iR0Z5WlN3Z1NXNWpMakViDQpNQmtHQTFVRUF4TVNiMjVrYVdkcGRHRnNi\n"\
"Mk5sWVc0dVlYQndNRmt3RXdZSEtvWkl6ajBDQVFZSUtvWkl6ajBEDQpBUWNEUWdB\n"\
"RTZyemhpQXl0ZUptbXhseXRFQlRSK1ZqN2E3dE1zZDhqY29TUWk2aHdmdW1nRFJq\n"\
"bXlFbkxkZUR0DQpHVkt4OGg0c2xKdDF5TTF5ZWRIS1g5UG1JcjFWaEtPQ0EyNHdn\n"\
"Z05xTUI4R0ExVWRJd1FZTUJhQUZLWE9OK3JyDQpzSFVPbEdlSXRFWDYyU1FRaDVZ\n"\
"Zk1CMEdBMVVkRGdRV0JCUjRzU2tCbmVnbGtMeFdVNUYxZHQvMjZzRzVjVEF6DQpC\n"\
"Z05WSFJFRUxEQXFnaEp2Ym1ScFoybDBZV3h2WTJWaGJpNWhjSENDRkNvdWIyNWth\n"\
"V2RwZEdGc2IyTmxZVzR1DQpZWEJ3TUQ0R0ExVWRJQVEzTURVd013WUdaNEVNQVFJ\n"\
"Q01Da3dKd1lJS3dZQkJRVUhBZ0VXRzJoMGRIQTZMeTkzDQpkM2N1WkdsbmFXTmxj\n"\
"blF1WTI5dEwwTlFVekFPQmdOVkhROEJBZjhFQkFNQ0E0Z3dIUVlEVlIwbEJCWXdG\n"\
"QVlJDQpLd1lCQlFVSEF3RUdDQ3NHQVFVRkJ3TUNNSHNHQTFVZEh3UjBNSEl3TjZB\n"\
"MW9ET0dNV2gwZEhBNkx5OWpjbXd6DQpMbVJwWjJsalpYSjBMbU52YlM5RGJHOTFa\n"\
"R1pzWVhKbFNXNWpSVU5EUTBFdE15NWpjbXd3TjZBMW9ET0dNV2gwDQpkSEE2THk5\n"\
"amNtdzBMbVJwWjJsalpYSjBMbU52YlM5RGJHOTFaR1pzWVhKbFNXNWpSVU5EUTBF\n"\
"dE15NWpjbXd3DQpkZ1lJS3dZQkJRVUhBUUVFYWpCb01DUUdDQ3NHQVFVRkJ6QUJo\n"\
"aGhvZEhSd09pOHZiMk56Y0M1a2FXZHBZMlZ5DQpkQzVqYjIwd1FBWUlLd1lCQlFV\n"\
"SE1BS0dOR2gwZEhBNkx5OWpZV05sY25SekxtUnBaMmxqWlhKMExtTnZiUzlEDQpi\n"\
"RzkxWkdac1lYSmxTVzVqUlVORFEwRXRNeTVqY25Rd0RBWURWUjBUQVFIL0JBSXdB\n"\
"RENDQVg4R0Npc0dBUVFCDQoxbmtDQkFJRWdnRnZCSUlCYXdGcEFIY0E3czNRWk5Y\n"\
"YkdzN0ZYTGVkdE0wVG9qS0hSbnk4N043RFVVaFpSbkVmDQp0WnNBQUFHS29Od2Er\n"\
"UUFBQkFNQVNEQkdBaUVBdjNPRG8zMDhhQXRTOWRzVVdIVDJwV28yYlFleVh3ZmZP\n"\
"T1AvDQp0RTVSbEVNQ0lRRE1XdmY5TkZKaUFoVzV2MFpNUkowM012ZlRGT0U5d0pZ\n"\
"b2ZQQjVIS05GcXdCM0FFaXc0MnZhDQpwa2MwRCtWcUF2cWRNT3NjVWdITFZ0MHNn\n"\
"ZG03djZzNTJJUnpBQUFCaXFEY0d3VUFBQVFEQUVnd1JnSWhBSk54DQpFTjVlQ20r\n"\
"eTBaanN2UVhlTnJ6NlhtZDg0NSt2OGFaZHdTNXNaa2x4QWlFQW9nUTlWZGlmR3lt\n"\
"QmhCWHBjWWxWDQpzcHk3S0xEK1VpVGZQcy9naGdLSmlUWUFkUURhdHI5clA3VzJJ\n"\
"cCtid3J0Y2EraHdrWEZzdTFHRWhUUzlwRDB3DQpTTmY3cXdBQUFZcWczQnJhQUFB\n"\
"RUF3QkdNRVFDSUVLc0JtM0J1NUpPZTd4bzgzV2dUOTVGci9UR1QwRmRCSEFsDQpI\n"\
"a0NSaGd2RkFpQjVIZXV1ZmxqTW9nTVB6SWhiUVFPWnRiZ005WC9HNHpNRmFaU0hn\n"\
"OTVsZERBS0JnZ3Foa2pPDQpQUVFEQWdOSkFEQkdBaUVBOFo0UW42WG1ScjF6Um1p\n"\
"a0ZDQWpFVFpmVEVwcEdkS1FDMDAyT1Y3UE5VQUNJUUN1DQpBRlpPRlBnQnI5QlVw\n"\
"MEMxUVppTGJmT1FsdWZPb0FFRzNHbVpBREc3dmc9PQ0KLS0tLS1FTkQgQ0VSVElG\n"\
"SUNBVEUtLS0tLQ0K\n"\
"-----END CERTIFICATE-----\n";

// WIFI FUNCTIONS
bool setupWiFi();                       // Function to setup WiFi connection
void changeWifiCreds(String, String);   // Function to change WiFi credentials
void scanWifi();                        // Function to scan available WiFi networks

// TEMP DATA FUNCTIONS
void updateData();             // Function to update temperature and humidity data
void sendData();               // Function to send data to the server
void changeURL(String);        // Function to change the server URL
void changeToken(String);      // Function to change the user token
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