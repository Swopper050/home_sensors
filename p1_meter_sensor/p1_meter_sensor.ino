#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "CRC16.h"
#include "p1_meter_secrets.h"


//===START-CONFIG===
// WiFi
const char* ssid = WIFI_SSID;
const char* wifiPassword = WIFI_PASSWORD;
const bool debug = false;

// OtA
const char *OTAHostname = OTA_HOSTNAME;
const char *OTAPassword = OTA_PASSWORD;
const int OTAPort = 8266;

// MQTT Broker
const char *MQTTBrokerIP = MQTT_BROKER_IP;
const char *baseTopic = MQTT_BASE_TOPIC;
const char *MQTTUsername = MQTT_USERNAME;
const char *MQTTPassword = MQTT_PASSWORD;
const int MQTTPort = 1883;

const int energyConsumptionLowTarifTopicID = ENERGY_CONSUMPTION_LOW_TARIF_TOPIC_ID;
const int energyConsumptionHighTarifTopicID = ENERGY_CONSUMPTION_HIGH_TARIF_TOPIC_ID;
const int energyYieldLowTarifTopicID = ENERGY_YIELD_LOW_TARIF_TOPIC_ID;
const int energyYieldHighTarifTopicID = ENERGY_YIELD_HIGH_TARIF_TOPIC_ID;
const int energyActualConsumptionTopicID = ENERGY_ACTUAL_CONSUMPTION_TOPIC_ID;
const int energyActualReturnTopicID = ENERGY_ACTUAL_RETURN_TOPIC_ID;
const int gasUsageTopicID = GAS_USAGE_TOPIC_ID; 
//===END-CONFIG===


// Vars to store meter readings
float energyConsumptionLowTarif = 0; //Meter reading Electrics - consumption low tariff
float energyConsumptionHighTarif = 0; //Meter reading Electrics - consumption high tariff
float energyYieldLowTarif = 0; //Meter reading Electrics - return low tariff
float energyYieldHighTarif = 0; //Meter reading Electrics - return high tariff
float energyActualConsumption = 0;  //Meter reading Electrics - Actual consumption
float energyActualReturn = 0;  //Meter reading Electrics - Actual return
float gasUsage = 0;    //Meter reading Gas


#define MAXLINELENGTH 128 // longest normal line is 47 char (+3 for \r\n\0)
char telegram[MAXLINELENGTH];

unsigned int currentCRC=0;

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
    Serial.println("Booting...");

    Serial.begin(115200, SERIAL_8N1, SERIAL_FULL);
    Serial.flush();

    USC0(UART0) = USC0(UART0) | BIT(UCRXI);
    Serial.println("Serial port is ready to recieve.");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, wifiPassword);

    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    // Setup MQTT client
    client.setServer(MQTTBrokerIP, MQTTPort);
    reconnectMQTT();

    setupOTA();

    Serial.print("Ready, IP address: ");
    Serial.println(WiFi.localIP());
}


void setupOTA() {
    ArduinoOTA.setHostname(OTAHostname);
    ArduinoOTA.setPassword(OTAPassword);
    ArduinoOTA.setPort(OTAPort);

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


void publishValue(float value, int sensorID) {
    char sensorTopic[30];
    sprintf(sensorTopic, "%s/%d", baseTopic, sensorID);

    char msg[40];
    sprintf(msg, "{\"value\": %.2f}", value);
    client.publish(sensorTopic, msg);
}

void publishAllValues() {
    publishValue(energyConsumptionLowTarif, energyConsumptionLowTarifTopicID);
    publishValue(energyConsumptionHighTarif, energyConsumptionHighTarifTopicID);
    publishValue(energyYieldLowTarif, energyYieldLowTarifTopicID);
    publishValue(energyYieldHighTarif, energyYieldHighTarifTopicID);
    publishValue(energyActualConsumption, energyActualConsumptionTopicID);
    publishValue(energyActualReturn, energyActualReturnTopicID);
    publishValue(gasUsage, gasUsageTopicID);
}


bool isNumber(char* res, int len) {
    for (int i = 0; i < len; i++) {
        if (((res[i] < '0') || (res[i] > '9'))  && (res[i] != '.' && res[i] != 0)) {
            return false;
        }
    }

    return true;
}


int findCharInArrayRev(char array[], char c, int len) {
    for (int i = len - 1; i >= 0; i--) {
        if (array[i] == c) {
            return i;
        }
    }

    return -1;
}


long getValidVal(long valNew, long valOld, long maxDiffer) {
    if (valOld > 0 && ((valNew - valOld > maxDiffer) && (valOld - valNew > maxDiffer))) {
        return valOld;
    }

    return valNew;
}


float getValue(char* buffer, int maxlen) {
    int s = findCharInArrayRev(buffer, '(', maxlen - 2);
    if (s < 8) return 0;
    if (s > 32) s = 32;

    int l = findCharInArrayRev(buffer, '*', maxlen - 2) - s - 1;
    if (l < 4) return 0;
    if (l > 12) return 0;

    char res[16];
    memset(res, 0, sizeof(res));

    if (strncpy(res, buffer + s + 1, l)) {
        if (isNumber(res, l)) {
            return atof(res);
        }
    }

    return 0;
}


bool readNewValuesFromTelegram(int len) {
    //need to check for start
    int startChar = findCharInArrayRev(telegram, '/', len);
    int endChar = findCharInArrayRev(telegram, '!', len);
    bool validCRCFound = false;

    if (startChar >= 0) {
        //start found. Reset CRC calculation
        currentCRC=CRC16(0x0000,(unsigned char *) telegram+startChar, len-startChar);    

        if (debug) {
            for (int cnt=startChar; cnt<len-startChar;cnt++) {
                Serial.print(telegram[cnt]);
            }
        }
    } else if (endChar>=0) {
        currentCRC=CRC16(currentCRC,(unsigned char*)telegram+endChar, 1);
        char messageCRC[5];
        strncpy(messageCRC, telegram + endChar + 1, 4);
        messageCRC[4]=0;
    
        validCRCFound = (strtol(messageCRC, NULL, 16) == currentCRC);
        currentCRC = 0;

        if (debug) {
            for (int cnt=0; cnt<len;cnt++) {
                Serial.print(telegram[cnt]);
            }
        }
    } else {
        currentCRC=CRC16(currentCRC, (unsigned char*)telegram, len);

        if (debug) {
            for(int cnt=0; cnt<len;cnt++) {
                Serial.print(telegram[cnt]);
            }
        }
    }

    // 1-0:1.8.1(000992.992*kWh)
    // 1-0:1.8.1 = Elektra verbruik laag tarief (DSMR v4.0)
    if (strncmp(telegram, "1-0:1.8.1", strlen("1-0:1.8.1")) == 0) {
        energyConsumptionLowTarif = getValue(telegram, len);
    }
  

    // 1-0:1.8.2(000560.157*kWh)
    // 1-0:1.8.2 = Elektra verbruik hoog tarief (DSMR v4.0)
    if (strncmp(telegram, "1-0:1.8.2", strlen("1-0:1.8.2")) == 0) {
        energyConsumptionHighTarif = getValue(telegram, len);
    }
    

    // 1-0:2.8.1(000348.890*kWh)
    // 1-0:2.8.1 = Elektra opbrengst laag tarief (DSMR v4.0)
    if (strncmp(telegram, "1-0:2.8.1", strlen("1-0:2.8.1")) == 0) {
        energyYieldLowTarif = getValue(telegram, len);
    }

    // 1-0:2.8.2(000859.885*kWh)
    // 1-0:2.8.2 = Elektra opbrengst hoog tarief (DSMR v4.0)
    if (strncmp(telegram, "1-0:2.8.2", strlen("1-0:2.8.2")) == 0) {
        energyYieldHighTarif = getValue(telegram, len);
    }
    

    // 1-0:1.7.0(00.424*kW) Actueel verbruik
    // 1-0:2.7.0(00.000*kW) Actuele teruglevering
    // 1-0:1.7.x = Electricity consumption actual usage (DSMR v4.0)
    if (strncmp(telegram, "1-0:1.7.0", strlen("1-0:1.7.0")) == 0) {
        energyActualConsumption = getValue(telegram, len);
    }
    
    if (strncmp(telegram, "1-0:2.7.0", strlen("1-0:2.7.0")) == 0) {
        energyActualReturn = getValue(telegram, len);
    }
   

    // 0-1:24.2.1(150531200000S)(00811.923*m3)
    // 0-1:24.2.1 = Gas (DSMR v4.0) on Kaifa MA105 meter
    if (strncmp(telegram, "0-1:24.2.1", strlen("0-1:24.2.1")) == 0) {
        gasUsage = getValue(telegram, len);
    }

    return validCRCFound;
}


void readTelegram() {
    if (Serial.available()) {
        memset(telegram, 0, sizeof(telegram));
        while (Serial.available()) {
            int len = Serial.readBytesUntil('\n', telegram, MAXLINELENGTH);
            if (len < MAXLINELENGTH) {
                telegram[len] = '\n';
                telegram[len+1] = 0;
                yield();
                readNewValuesFromTelegram(len+1);
            }
        }
        publishAllValues();
    }
}


void reconnectMQTT() {
    while (!client.connected()) {
        String clientID = "esp8266-client-";
        clientID += String(WiFi.macAddress());

        Serial.printf("Trying to connect to the MQTT broker: %s\n", clientID.c_str());

        if (client.connect(clientID.c_str(), MQTTUsername, MQTTPassword)) {
            Serial.println("Succesfully connected to MQTT broker!");
        } else {
            Serial.println("Failed to connect to the MQTT broker: %s");
            delay(5000);
        }
    }
}


void loop() {
    ArduinoOTA.handle();

    if (!client.connected()) {
        reconnectMQTT();
    }

    readTelegram();
    delay(500);
}
