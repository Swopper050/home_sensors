P1 Meter Sensor on ESP8266
==========================

This is a sensor I use to extract energy data from my P1 meter.

For this sensor I basically was completely reliant on https://github.com/daniel-jong/esp8266_p1meter.

It is based on the following hardware:
 - An ESP8266 ([link](https://www.bol.com/nl/nl/p/azdelivery-3-x-esp32-nodemcu-module-wlan-wifi-development-board-met-cp2102-compatibel-met-arduino-inclusief-e-book/9300000074412300/?bltgh=rTDlsS2LmNVqDXMgsgJBTw.2_11.15.ProductImage))
 - A P1 cable ([link](https://www.bol.com/nl/nl/p/p1-slimme-meter-kabel/9200000111534146/?Referrer=ADVNLGOO002013-S--9200000111534146&gclid=CjwKCAjwu4WoBhBkEiwAojNdXojov4i6OqTI2QiD_DLBLSagKxh3q9OdywPkkMNj9096XdD6sKtLqBoCm3AQAvD_BwE))
 - A simple 5V adapter to power the ESP8266 via the micro usb port.

Code and flashing the ESP8266
-----------------------------
I flashed the sensor on arch using the Arduino IDE [here](https://archlinux.org/packages/extra/x86_64/arduino/).
Furthermore you need to install the libraries used in the file, and configure it accordingly.

Settings to configure:
```ino
//===START-CONFIG===
// WIFI
const char* ssid = "";
const char* password = "";
const bool debug = false;

// MQTT Broker
const char *mqtt_broker_ip = "";
const char *base_topic = "";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

const int energyConsumptionLowTarifTopicID = -1; 
const int energyConsumptionHighTarifTopicID = -1; 
const int energyYieldLowTarifTopicID = -1; 
const int energyYieldHighTarifTopicID = -1; 
const int energyActualConsumptionTopicID = -1; 
const int energyActualReturnTopicID = -1; 
const int gasUsageTopicID = -1; 
//===END-CONFIG===
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

 - Solder the brown (power supply) line of the DS18B20 sensor on a 3V I/O pin of the ESP8266.
 - Solder the green (ground) line of the DS18B20 sensor on a G I/O pin of the ESP8266.
 - Solder the greyish (data) line of the DS18B20 sensor on a RX I/O pin of the ESP8266.
 - Solder a 10K Ohm resistance between the brown and greyish line.


The looks
---------
The finished sensor looks like this:

<img src="https://github.com/Swopper050/home_sensors/assets/38559175/4181e90b-c6de-49aa-a6f9-1fda54590cce" alt="full_sensor" width="40%"/>
<img src="https://github.com/Swopper050/home_sensors/assets/38559175/caf94d1a-b74d-43cd-b18f-7b8e8ec1b35d" alt="adapter" width="40%"/>
<img src="https://github.com/Swopper050/home_sensors/assets/38559175/33ab8536-aaa8-478a-bad9-99cdcb1abcf6" alt="connection" width="100%"/>
