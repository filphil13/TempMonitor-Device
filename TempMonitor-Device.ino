#include "TempMonitor-Device.h"

//APP SETUP
void setup() {
	Serial.begin(BAUD_RATE);
	delay(1000);	
	loadCreds();

	WiFi.mode(WIFI_MODE);
	setupWiFi(SSID,PASSWORD);
	dht.begin();
	
}

//MAINLOOP
void loop() {
	//check for command via Serial
	if(Serial.peek() != -1)
	{
		SPIMenu();
	}

	//Send an HTTP POST request every 5 seconds
	if ((millis() - lastTime) > timerDelay) {
		updateData();
		printData();
		sendData();
		
		lastTime = millis();
	}
}


//WIFI FUNCTIONS
bool setupWiFi(String ssid, String password){
    
    WiFi.begin(ssid.c_str(), password.c_str());
  	Serial.println("Connecting");

	int temp = millis();
  	while((WiFi.status() != WL_CONNECTED) && (millis() - lastTime) > timerDelay) {
    	Serial.print(".");
		delay(100);
		temp = millis();
  	}
	if (WiFi.status()==WL_CONNECTED){
		Serial.print("\nConnected to WiFi network with IP Address: ");
		Serial.println(WiFi.localIP());
		printNetworkStatus();
		return true;
	}
	else{
		Serial.println("Connection timed out, could not connect to wifi.");
		WiFi.disconnect();
		return false;

	}
}

void changeWifiCreds(String ssid, String password){
	WiFi.disconnect();
	//IF WIFI CHANGE SUCCESSFUL:
	//SAVE CREDENTIALS
	if(setupWiFi(ssid, password)){
		saveCreds();
	}

	//IF WIFI CHANGE FAILS:
	//
	//
	else{
		Serial.println("Error: Wifi change failed.");
	}
}

void scanWifi(){
	Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm\n");
  }
}

//TEMP DATA FUNCTIONS
void updateData(){
	TEMPERATURE = dht.readTemperature();
	HUMIDITY = dht.readHumidity();

	if (isnan(HUMIDITY) || isnan(TEMPERATURE)){
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}

void sendData(){
	if(WiFi.status()== WL_CONNECTED){
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
}

//SYSTEM FUNCTIONS

void loadCreds(){
	preferences.begin("sensor-creds",false);
	String ssid = preferences.getString("ssid", "");
	String password = preferences.getString("password", "");
	String name = preferences.getString("name", "");

	Serial.println(ssid);
	Serial.println(password);
	//If no credentials are saved, prompt user for setup on serial
	if (ssid == ""){
		unsigned long lastMsgTime = millis();
		Serial.println("Please Enter SSID");
		while(Serial.peek() == -1)
		{
			if ((millis() - lastTime) > timerDelay) {
				Serial.println("Please Enter SSID");

				lastTime = millis();
			}
		}
		ssid = Serial.readString();
		Serial.println("Please Enter PASSWORD");
		while(Serial.peek() == -1)
		{
			if ((millis() - lastTime) > timerDelay) {
				Serial.println("Please Enter PASSWORD");
				
				lastTime = millis();
			}
		}
		password = Serial.readString();
	}

	SSID = ssid;
	PASSWORD = password;
	SENSOR_NAME = name;
	saveCreds();

	preferences.end();	
}

void saveCreds(){
	preferences.begin("sensor-creds", false);

	preferences.putString("ssid", SSID);
	preferences.putString("password", PASSWORD);
	preferences.putString("name", SENSOR_NAME);

	preferences.end();
}

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

//SPI MENU
// h  - display spi menu
// cw - change wifi credentials
// cn - change sensor name
// pn - print network settings
// sn - scan nearby networks
// r  - restart sensor

void SPIMenu(){
	String msg = Serial.readString();
	if (msg == "h"){
		Serial.println("h  - display spi menu");
		Serial.println("cw - Change Wifi Credentials");
		Serial.println("cn - Change Sensor Name");
		Serial.println("pn - View Network Settings");
		Serial.println("sn - Print Nearby Networks");
		Serial.println("r  - Restart Sensor");
	}
	else if(msg == "cw"){

		Serial.println("Please Enter SSID");
		while(Serial.peek() == -1){}
		String ssid = Serial.readString();

		Serial.println("Please Enter PASSWORD");
		while(Serial.peek() == -1){}
		String password = Serial.readString();

		changeWifiCreds(ssid,password);
	}
	else if(msg == "cn"){
		Serial.println("Please Enter Name");
		while(Serial.peek() == -1){}

		SENSOR_NAME = Serial.readString();
	}
	else if(msg == "pn"){
		printNetworkStatus();
	}
	else if(msg == "sn"){
		scanWifi();
	}
	else if(msg == "r"){
		resetFunc();
	}
	else{
		Serial.println(msg);
		Serial.println(msg);
		Serial.println("Error: command not found.");
	}
}