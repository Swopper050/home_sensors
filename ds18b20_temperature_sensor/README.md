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

Settings to configure:
```ino
// Wifi
const char *ssid = "";
const char *password = "";


// MQTT Broker
const char *mqtt_broker = "";
const char *topic = "";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;
```

To flash it:
 1. connect the ESP8266 to your computer via USB.
 2. Open the Arduino IDE.
 3. Under `Tools > Board` select the `Generic ESP8266 Moduel` option (make sure to add the board first as described above).
 4. Under `Tools > port` select the right port, i.e. your ESP8266.
 5. Make sure the settings are configured correctly.
 6. Click `Upload` and wih fingers crossed it probably will work 73.2% of the time. 

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
