#include <ArduinoJson.h>

#include <DHT.h>
#include "WiFi.h"
#include <HTTPClient.h>

#define DHTPIN 4
#define ONBOARD_LED  2
#define BAUD_RATE 115200
#define WIFI_MODE WIFI_STA
#define SCAN_INTERVAL 5000
#define DHTTYPE DHT22

String SSID = "";
String PASSWORD = "";
String IP = "http://temp-monitor-a38f32c02c5e.herokuapp.com/updateSensor";
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
HTTPClient http;
String SENSOR_NAME = "Test Sensor 1";
float TEMPERATURE = 0.0f;
float HUMIDITY = 0.0f;

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void printData(){
	Serial.println("TEMP: " + String(TEMPERATURE) + "C");
	Serial.println("HUMIDITY: " + String(HUMIDITY) + "%");
}

void blinkLED(int numOfBlinks){
	for (int i=0; i < numOfBlinks; i++){
		delay(3000);
		digitalWrite(ONBOARD_LED,HIGH);
		delay(100);
		digitalWrite(ONBOARD_LED,LOW);
	}
}

void setupWiFi(){
    WiFi.mode(WIFI_MODE);
    WiFi.begin(SSID, PASSWORD);
  	Serial.println("Connecting");

	int attemptCount = 0;
  	while(WiFi.status() != WL_CONNECTED) {
    	Serial.print(".");
		delay(100);
  	}
	Serial.print("\nConnected to WiFi network with IP Address: ");
	Serial.println(WiFi.localIP());
	printNetworkStatus();
}

void printNetworkStatus(){
	if (WiFi.status() != WL_CONNECTED){
    	Serial.println("\nNOT CONNECTED TO ANY NETWORK");
	}
	else{
    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
	}
}

void updateData(){
	TEMPERATURE = dht.readTemperature();
	HUMIDITY = dht.readHumidity();

	if (isnan(HUMIDITY) || isnan(TEMPERATURE)){
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}

void sendData(){


    http.begin(client, IP.c_str());
    http.addHeader("Content-Type", "application/json");

	StaticJsonDocument<96> doc;

	doc["name"] = SENSOR_NAME;
	doc["temperature"] = TEMPERATURE;
	doc["humidity"] = HUMIDITY;
	doc["time"] = 0;
	
	String output;
	serializeJson(doc, output);
	Serial.println(output);
	Serial.println();


    int httpResponseCode = http.POST(output);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

	doc.clear();
    http.end();
}

void setup() {
	Serial.begin(BAUD_RATE);
	delay(1000);
	setupWiFi();
	dht.begin();
}

void loop() {
  //Send an HTTP POST request every 5 seconds
	if ((millis() - lastTime) > timerDelay) {
		updateData();
		printData();
		//Check WiFi connection status
		if(WiFi.status()== WL_CONNECTED){
		sendData();
		}
		else {
		Serial.println("WiFi Disconnected");
		}
		lastTime = millis();
	}
}