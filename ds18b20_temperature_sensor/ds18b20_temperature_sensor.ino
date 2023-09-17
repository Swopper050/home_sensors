#include <OneWire.h> 
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with a OneWire device (i.e.
// (the dallas temperature sensor).
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);


const int updateEveryNSeconds = 10;
int nSecondsSinceLastUpdate = 0;

const char *ssid = "";
const char *wifiPassword = "";

// OTA
const char *OTAPassword = "";
const char *OTAHostname = "";

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

    setupOTA();

    sensors.begin(); 
    digitalWrite(LED_BUILTIN, LOW);
}


void setupOTA() {
    ArduinoOTA.setPort(8266);
    ArduinoOTA.setHostname(OTAHostname);
    ArduinoOTA.setPassword(OTAPassword);

    ArduinoOTA.onStart([]() {
        Serial.println("Starting update..");
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nFinished update!");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Authentication Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    ArduinoOTA.begin();

    Serial.println("OTA Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
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
    ArduinoOTA.handle();
   
    nSecondsSinceLastUpdate += 1;
    if (nSecondsSinceLastUpdate < updateEveryNSeconds) {
        digitalWrite(LED_BUILTIN, HIGH);

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

        nSecondsSinceLastUpdate = 0;
        digitalWrite(LED_BUILTIN, LOW);
    }

    delay(1000);
}
