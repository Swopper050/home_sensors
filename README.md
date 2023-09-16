Home sensors
============

This repository is intended for myself. It is a place where I document and
store/iterate on the code I use for sensors at home.

Feel free to copy or use any of the code in this repository!

My sensors are connected to an MQTT broker that connects with [Climatics](https://climatics.nl),
but any system that supports MQTT can be connected to the sensors.


Arduino IDE for ESP8266
-----------------------

The code is written in the Arduino IDE, and I use it to flash the ESP8266 boards.
It took me some time to figure out how to setup the Arduino IDE for ESP8266, so
for those wondering, these are the steps I took (I use arch btw):
 1. Install the Arduino IDE ([link for arch](https://archlinux.org/packages/extra/x86_64/arduino/)).
 2. Install the ESP8266 board using [this guide](https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/).
 3. Furthermore, make sure to check the imports for all sensors and install the corresponding packages.

Then, in order to flash an ESP8266, do the following:
 1. Connect your ESP8266 board to your computer via USB.
 2. Open the Arduino IDE.
 3. Open the file you want to flash.
 4. Under `Tools > Board` select the `Generic ESP8266 Moduel` option (make sure to add the board first as described above).
 5. Under `Tools > port` select the right port, i.e. your ESP8266.
 6. Make sure the settings are configured correctly (wifi and everything).
 7. Click `Upload` and wih fingers crossed it probably will work 73.2% of the time. 

