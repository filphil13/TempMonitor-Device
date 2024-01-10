#include "TempMonitor-Device.h"

//APP SETUP
void setup() {
	Serial.begin(BAUD_RATE); // Initialize serial communication
	delay(1000); // Delay for stability
	loadCreds(); // Load saved credentials
	WiFi.mode(WIFI_MODE); // Set WiFi mode
	setupWiFi(SSID,PASSWORD); // Connect to WiFi network
	dht.begin(); // Initialize DHT sensor
}

//MAINLOOP
void loop() {
	//check for command via Serial
	if(Serial.peek() != -1)
	{
		SPIMenu(); // Process commands from Serial
	}

	//Send an HTTP POST request every 5 seconds
	if ((millis() - lastTime) >= timerDelay) {
		updateData(); // Update temperature and humidity data
		printData(); // Print temperature, humidity, and URL
		sendData(); // Send data to server
		lastTime = millis(); // Update lastTime
	}
}

//WIFI FUNCTIONS

/**
 * Connects to a WiFi network using the provided SSID and password.
 * @param ssid The SSID of the WiFi network.
 * @param password The password of the WiFi network.
 * @return True if the connection is successful, false otherwise.
 */
bool setupWiFi(String ssid, String password){
    WiFi.begin(ssid.c_str(), password.c_str()); // Connect to the WiFi network
  	Serial.println("Connecting");
	lastTime = millis();
  	while((millis() - lastTime) < WIFI_TIMEOUT) {
    	switch(WiFi.status()) {
        	case WL_NO_SSID_AVAIL:
				Serial.println("[WiFi] SSID not found");
				break;
		  	case WL_SCAN_COMPLETED:
				Serial.println("[WiFi] Scan completed");
				break;
			case WL_IDLE_STATUS:
				Serial.println("[WiFi] Idle status");
				break;
			case WL_DISCONNECTED:
				Serial.println("[WiFi] Disconnected");
				break;
			case WL_CONNECT_FAILED:
				Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
				break;
			case WL_CONNECTION_LOST:
				Serial.println("[WiFi] Connection was lost");
				break;
			case WL_CONNECTED:
				Serial.println("[WiFi] WiFi is connected!");
				Serial.println("[WiFi] IP address: ");
				Serial.print(WiFi.localIP());
				printNetworkStatus();
				SSID = ssid;
				PASSWORD = password;
				return true;
			default:
				Serial.print("[WiFi] WiFi Status: ");
				Serial.println(WiFi.status());
				break;
        }
		delay(100);

  	}
	return false;
}

/**
 * Changes the WiFi credentials and saves them if the change is successful.
 * @param ssid The new SSID of the WiFi network.
 * @param password The new password of the WiFi network.
 */
void changeWifiCreds(String ssid, String password){
	WiFi.disconnect(); // Disconnect from current WiFi network
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

/**
 * Scans for nearby WiFi networks and prints their information.
 */
void scanWifi(){
	Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  while(numSsid == -2){	
		if (numSsid == -1) {
			Serial.println("Couldn't get a wifi connection");
			while (true);
		}

		// print the list of networks seen:
		Serial.print("number of available networks:");
		Serial.println(numSsid);
  }
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

/**
 * Updates the temperature and humidity data from the DHT sensor.
 */
void updateData(){
	TEMPERATURE = dht.readTemperature();
	HUMIDITY = dht.readHumidity();

	if (isnan(HUMIDITY) || isnan(TEMPERATURE)){
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}

/**
 * Sends the temperature and humidity data to the server via HTTP POST request.
 */
void sendData(){
	if(WiFi.status()== WL_CONNECTED){
		http.begin(client, URL.c_str());
		http.addHeader("Content-Type", "application/json");

		StaticJsonDocument<200> doc;

		doc["Temperature"] = TEMPERATURE;
		doc["Humidity"] = HUMIDITY;
		doc["Time"] = 0;
		
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

/**
 * Changes the URL address and saves it.
 * @param url The new URL address.
 */
void changeURL(String url){
	URL = url + "/api/update?name=" + SENSOR_NAME;
	saveCreds();
}

/**
 * Changes the sensor name and saves it.
 * @param name The new sensor name.
 */
void changeName(String name){
	SENSOR_NAME = name;
	saveCreds();
}

//SYSTEM FUNCTIONS

/**
 * Loads the saved credentials from the preferences.
 */
void loadCreds(){
	preferences.begin("sensor-creds",false);
	String ssid = preferences.getString("ssid", "");
	String password = preferences.getString("password", "");
	String name = preferences.getString("name", "NO_NAME_SET");
	String url = preferences.getString("url", "");

	Serial.println(ssid);
	Serial.println(password);

	//If no credentials are saved, prompt user for setup on serial
	if (ssid == ""){
		unsigned long lastMsgTime = millis();
		while(Serial.peek() == -1)
		{
			if ((millis() - lastTime) > timerDelay) {
				Serial.println("Please Enter SSID");

				lastTime = millis();
			}
		}
		ssid = getSSIDFromUser();
		password = getPasswordFromUser();
		changeWifiCreds(ssid,password);
		
	}

	//IMPORT ALL THE DATA INTO VARIABLES
	SSID = ssid;
	PASSWORD = password;
	SENSOR_NAME = name;
	URL = url;

	saveCreds();

	preferences.end();	
}

/**
 * Saves the credentials to the preferences.
 */
void saveCreds(){
	preferences.begin("sensor-creds", false);

	preferences.putString("ssid", SSID);
	preferences.putString("password", PASSWORD);
	preferences.putString("name", SENSOR_NAME);
	preferences.putString("url", URL);

	preferences.end();
}

String getSSIDFromUser() {
    Serial.println("Please Enter SSID");
    while(Serial.available() == 0) {
        delay(100); // wait for user input
    }
    String ssid = Serial.readString();
    return ssid;
}

String getPasswordFromUser() {
    Serial.println("Please Enter PASSWORD");
    while(Serial.available() == 0) {
        delay(100); // wait for user input
    }
    String password = Serial.readString();
    return password;
}
 
/**
 * Prints the saved credentials.
 */
void printCreds(){
	preferences.begin("sensor-creds",false);
	String ssid = preferences.getString("ssid", "");
	String password = preferences.getString("password", "");
	String name = preferences.getString("name", "NO_NAME_SET");
	String url = preferences.getString("url", "");

	Serial.println("SSID: " + SSID);
	Serial.println("PASSWORD: " + PASSWORD);
	Serial.println("NAME: " + SENSOR_NAME);
	Serial.println("URL: " + URL);

	preferences.end();

}

/**
 * Prints the temperature, humidity, and URL.
 */
void printData(){
	Serial.println("TEMP: " + String(TEMPERATURE) + "C");
	Serial.println("HUMIDITY: " + String(HUMIDITY) + "%");
	Serial.println(URL);

}

/**
 * Blinks the onboard LED a specified number of times.
 * @param numOfBlinks The number of times to blink the LED.
 */
void blinkLED(int numOfBlinks){
	for (int i=0; i < numOfBlinks; i++){
		delay(3000);
		digitalWrite(ONBOARD_LED,HIGH);
		delay(100);
		digitalWrite(ONBOARD_LED,LOW);
	}
}

/**
 * Prints the network status.
 */
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
// cu - change URL

/**
 * Processes commands from Serial and performs corresponding actions.
 */
void SPIMenu(){
	String msg = Serial.readString();

	if (msg == "h"){
		Serial.println("h  	- display spi menu");
		Serial.println("r  	- Restart Sensor");
		Serial.println("");
		Serial.println("cw 	- Change Wifi Credentials");
		Serial.println("cn 	- Change Sensor Name");
		Serial.println("cu	- Change URL");
		Serial.println("");
		Serial.println("pn 	- Print Network Settings");
		Serial.println("pc	- Print Credentials");
		Serial.println("sn 	- Print Nearby Networks");
		Serial.println("sc 	- Save Credentials");
		
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

	else if (msg == "sc"){
		saveCreds();
	}
	
	else if(msg == "cn"){
		Serial.println("Please Enter Name");
		while(Serial.peek() == -1){}

		String name = Serial.readString();
		changeName(name);
	}
	else if(msg == "cu"){
		Serial.println("Please Enter the new URL Address:");
		while(Serial.peek() == -1){}
		String url = Serial.readString();
		changeURL(url);
	}
	else if(msg == "pn"){
		printNetworkStatus();
	}
	else if(msg == "rc"){
		if (WiFi.status() != WL_CONNECTED){
			Serial.println("Reconnecting to WiFi...");
			setupWiFi(SSID, PASSWORD);
  		}
	}
	else if(msg == "pc"){
		printCreds();
	}
	else if(msg == "sn"){
		scanWifi();
	}
	else if(msg == "r"){
		resetFunc();
	}
	else{
		Serial.println(msg);
		Serial.println("Error: '"+ msg + "'command not found.");
	}
}

/*
#include <esp_ghota.h>

/**
 * Performs OTA (Over-The-Air) update using EspGhota library.
 */

void performOTAUpdate() {
	// Initialize the OTA updater
	/*
	EspGhota otaUpdater;

	// Set the GitHub repository details
	otaUpdater.setGitHubRepo("username", "repository");

	// Set the GitHub access token (optional)
	otaUpdater.setGitHubToken("your_access_token");

	// Set the version file path on GitHub
	otaUpdater.setVersionFilePath("/path/to/version/file");

	// Set the firmware file path on GitHub
	otaUpdater.setFirmwareFilePath("/path/to/firmware/file");

	// Set the callback function to be executed after the update
	otaUpdater.setUpdateCallback([]() {
		// Perform any necessary actions after the update
		// For example, restart the device
		ESP.restart();
	});

	// Start the OTA update process
	otaUpdater.begin();
	*/
}


//#include <esp_sleep.h>

/**
 * Puts the ESP32 into deep sleep mode for a specified duration.
 * @param duration The sleep duration in microseconds.
 */
/*
void deepSleepWithTimer(uint64_t duration) {
	esp_sleep_enable_timer_wakeup(duration);
	esp_deep_sleep_start();
}
*/
/**
 * Puts the ESP32 into deep sleep mode until a serial event occurs.
 */
/*
void deepSleepWithSerialWakeUp() {
	esp_sleep_enable_uart_wakeup();
	esp_deep_sleep_start();
}
*/