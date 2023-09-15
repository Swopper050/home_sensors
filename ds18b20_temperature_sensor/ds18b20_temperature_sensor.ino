#include <OneWire.h> 
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with a OneWire device (i.e.
// (the dallas temperature sensor).
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);


const char *ssid = "";
const char *wifiPassword = "";


// MQTT Broker
const char *mqttBrokerIP = "";
const char *topic = "";
const char *mqttUsername = "";
const char *mqttPassword = "";
const int mqttPort = 1883;


WiFiClient espClient;
PubSubClient client(espClient);


void setup(void) {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // Setup wifi
    WiFi.begin(ssid, wifiPassword);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Trying to connect to WiFi..");
    }
    Serial.println("Succesfully connected to WiFi!");

    // Setup mqtt client
    client.setServer(mqttBrokerIP, mqttPort);
    reconnectMQTT();

    sensors.begin(); 
    digitalWrite(LED_BUILTIN, LOW);
}


void reconnectMQTT() {
    while (!client.connected()) {
        String clientID = "esp8266-client-";
        clientID += String(WiFi.macAddress());

        Serial.printf("Trying to connect to the MQTT broker: %s\n", clientID.c_str());

        if (client.connect(clientID.c_str(), mqttUsername, mqttPassword)) {
            Serial.println("Succesfully connected to MQTT broker!");
        } else {
            Serial.println("Failed to connect to the MQTT broker: %s");
            delay(5000);
        }
    }
}


void loop(void) { 
    if (!client.connected()) {
        reconnectMQTT();
    }

    sensors.requestTemperatures();

    Serial.print("Temperature is: "); 
    Serial.print(sensors.getTempCByIndex(0));
    Serial.print("\n");

    char msg[40];
    sprintf(msg, "{\"value\": %.2f}", sensors.getTempCByIndex(0));
    client.publish(topic, msg);

    delay(10000);
}
