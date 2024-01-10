# TempMonitor-Device

## Description
TempMonitor-Device is a temperature and humidity monitoring device built for refrigeration units. It uses a DHT sensor to read temperature and humidity data, and sends this data to a server via HTTP POST requests.

## Technologies Used
* Arduino
* C/C++
* Python
* G-code

## Installation
To install this project, follow these steps:
1. Clone the repository to your local machine.
2. Open the `.ino` file in your Arduino IDE.
3. Install the necessary libraries:
    -ArduinoJson.h  // Library for handling JSON data
    -Preferences.h  // Library for storing preferences in ESP32
    -DHT.h          // Library for DHT temperature and humidity sensor
    -WiFi.h         // Library for WiFi connectivity
    -HTTPClient.h   // Library for making HTTP requests
4. Compile and upload the code to your Arduino board.

## Usage
To use this project, follow these steps:
1. Connect the DHT sensor to your Arduino board.
2. Connect the Arduino board to your computer.
3. Open the Serial Monitor in your Arduino IDE.
4. Send commands via the Serial Monitor to control the device.

## Contributing
Contributions are welcome!

## License
This project is licensed under the GPL-3.0 license. Please see the [GPL-3.0 license](https://www.gnu.org/licenses/gpl-3.0.en.html) for more information.