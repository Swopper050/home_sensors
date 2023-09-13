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
const char *password = "";


// MQTT Broker
const char *mqtt_broker = ""; // ip
const char *topic = "";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;


WiFiClient espClient;
PubSubClient client(espClient);


void setup(void) {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Setup wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  // Setup mqtt client
  client.setServer(mqtt_broker, mqtt_port);
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }

  sensors.begin(); 
  digitalWrite(LED_BUILTIN, LOW);
}


void loop(void) { 
   sensors.requestTemperatures();

   Serial.print("Temperature is: "); 
   Serial.print(sensors.getTempCByIndex(0));
   Serial.print("\n");

   char msg[40];
   sprintf(msg, "{\"value\": %.2f}", sensors.getTempCByIndex(0));
   client.publish(topic, msg);

   delay(10000);
}
