#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include "Chrono.h"
#include "SHT21.h"


#define POLLING_SAMPLE 30000


const char ssid[] = "sweet internet 2.4";
const char pass[] = "";
const char temp_topic[] = "homeassistant/sensor/livingRoomTemp/state";
const char temp_register_topic[] = "homeassistant/sensor/livingRoomTemp/config";
const char humidity_topic[] = "homeassistant/sensor/livingRoomHumidity/state";
const char humidity_register_topic[] = "homeassistant/sensor/livingRoomHumidity/config";

const char temp_register_paylod[] =
  "{\"dev_cla\": \"temperature\", \"name\": \"Living Room Temperature\","
  "\"stat_t\": \"homeassistant/sensor/livingRoomTemp/state\", "
  "\"unit_of_meas\": \"°C\", \"val_tpl\": \"{{ value_json.temperature }}\"";

const char humidity_register_payload[] =
  "{\"dev_cla\": \"humidity\", \"name\": \"Living Room Humidity\","
  "\"state_t\": \"homeassistant/sensor/livingRoomHumidity/state\", "
  "\"unit_of_meas\": \"%\", \"val_tpl\": \"{{ value_json.temperature }}\"";


WiFiClient net;
MQTTClient client;
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
SHT21 SHT21;
Chrono temp_pub_chrono;
bool sensors_registered;


void connect() {
  Serial.print("Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("connected!");

  Serial.print("Connecting to MQTT...");
  while (!client.connect("livingroom_temp_hum", "home_sensors", "home_sensors")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("connected!");
}

void setup() {
    Serial.begin(115200);
    Serial.println("hello");

    WiFi.begin(ssid, pass);

    client.begin("192.168.1.20", net);

    if (!tempsensor.begin(0x18))
    {
        Serial.println("Couldn't find MCP9808!");
    }
    SHT21.begin();

    sensors_registered = false;
}


void loop() {
    client.loop();

    if (!client.connected() || WiFi.status() != WL_CONNECTED) {
        connect();
        return;
    }

    if(!sensors_registered) {
        Serial.println("Registering sensors");
        client.publish(temp_register_topic, temp_register_paylod);
        client.publish(humidity_register_topic, humidity_register_payload);
        sensors_registered = true;
        return;
    }

    if(temp_pub_chrono.hasPassed(POLLING_SAMPLE)) {
        temp_pub_chrono.restart();

        float temp = tempsensor.readTempC();
        Serial.print("Temperature ");
        Serial.println(String(temp).c_str());

        Serial.print("Humidity ");
        Serial.println(String(SHT21.getHumidity()).c_str());
        client.publish(temp_topic, String(temp).c_str());
        client.publish(humidity_topic, String(SHT21.getHumidity()).c_str());
    }

    delay(1);
}
