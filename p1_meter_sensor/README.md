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

Settings/secrets to configure (see `secrets.example.h`):
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

#define ENERGY_CONSUMPTION_LOW_TARIF_TOPIC_ID 1
#define ENERGY_CONSUMPTION_HIGH_TARIF_TOPIC_ID 2
#define ENERGY_YIELD_LOW_TARIF_TOPIC_ID 3
#define ENERGY_YIELD_HIGH_TARIF_TOPIC_ID 4
#define ENERGY_ACTUAL_CONSUMPTION_TOPIC_ID 5
#define ENERGY_ACTUAL_RETURN_TOPIC_ID 6
#define GAS_USAGE_TOPIC_ID 7
```

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
