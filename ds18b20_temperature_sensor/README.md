DS18B20 Temperature Sensor on ESP8266
=====================================

This is a simple sensor I use to log the temperature.

It is based on the following hardware:
 - An ESP8266 ([link](https://www.bol.com/nl/nl/p/azdelivery-3-x-esp32-nodemcu-module-wlan-wifi-development-board-met-cp2102-compatibel-met-arduino-inclusief-e-book/9300000074412300/?bltgh=rTDlsS2LmNVqDXMgsgJBTw.2_11.15.ProductImage))
 - A DS18B20 temperature sensor ([link](https://www.bol.com/nl/nl/p/azdelivery-5-x-3m-kabel-ds18b20-digitale-roestvrij-staal-temperatuursensor-waterdicht-compatibel-met-arduino-en-raspberry-pi-inclusief-e-book/9300000119723758/?Referrer=ADVNLGOO002013-S--9300000119723758&gclid=CjwKCAjwu4WoBhBkEiwAojNdXiypFsLXa3LE5nHXwcxSGfaG4pTQ8nt89tEkOCztx3bslwrjAw4FzxoCFTsQAvD_BwE))
 - A simple 5V adapter to power the ESP8266 via the micro usb port.

Code and flashing the ESP8266
-----------------------------
I flashed the sensor on arch using the Arduino IDE [here](https://archlinux.org/packages/extra/x86_64/arduino/).
In order to build and flash the ESP8266, you need to install the board into the Arduino IDE (I blindly followed [this](https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/) guide).
Furthermore you need to install the libraries used in the file, and configure it accordingly.

Settings/secrets to configure (see `secrets.example.ino`):
```ino
// Rename this file to 'secrets.h' after filling in your secrets.

#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "wifi_password"

#define OTA_HOSTNAME "hostname"
#define OTA_PASSWORD "ota_password"

#define MQTT_BROKER_IP "ip"
#define MQTT_TOPIC "your/topic"
#define MQTT_USERNAME "mqtt_username"
#define MQTT_PASSWORD "mqtt_password"
```

Furthermore you can update the amount of times the sensor publishes to MQTT in the `.ino` file.


Quick explanation of the soldering
----------------------------------

 - Solder the red (power supply) line of the DS18B20 sensor on a 3V I/O pin of the ESP8266.
 - Solder the black (ground) line of the DS18B20 sensor on a G I/O pin of the ESP8266.
 - Solder the yellow (data) line of the DS18B20 sensor on a D4 I/O pin of the ESP8266.
 - Solder a 4.7K Ohm resistance between the red and yellow line.


The looks
---------
The finished sensor looks like this (yes, I found soldering to be quite hard):

<img src="https://github.com/Swopper050/home_sensors/assets/38559175/d7cabc57-bbc8-4c1a-907e-5e79fb0b9740" alt="full_sensor" width="40%"/>
<img src="https://github.com/Swopper050/home_sensors/assets/38559175/c890fc20-fd7a-43da-8bb3-23079b392537" alt="adapter" width="40%"/>
<img src="https://github.com/Swopper050/home_sensors/assets/38559175/9cf90fb1-d367-4dfb-ae72-8375aee6c2ef" alt="connection"/>
