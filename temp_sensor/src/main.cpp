#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include "Chrono.h"
#include "SHT21.h"


#define POLLING_SAMPLE 30000


const char ssid[] = "ssid";
const char pass[] = "pass";
const char temp_topic[] = "home/bedroom/temperature";
const char humidity_topic[] = "home/bedroom/humidity";

WiFiClient net;
MQTTClient client;
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
SHT21 SHT21;
Chrono temp_pub_chrono;


void connect() {
  Serial.print("Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("connected!");

  Serial.print("Connecting to MQTT...");
  while (!client.connect("inside_temperature", "inside_temperature", "inside_temperature")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("connected!");
}

void setup() {
    Serial.begin(115200);

    WiFi.begin(ssid, pass);

    client.begin("192.168.1.109", net);

    connect();

    if (!tempsensor.begin(0x18))
    {
        Serial.println("Couldn't find MCP9808!");
    }
    SHT21.begin();
}


void loop() {
    client.loop();

    if (!client.connected() || WiFi.status() != WL_CONNECTED) {
        connect();
    }

    if(temp_pub_chrono.hasPassed(POLLING_SAMPLE)) {
        temp_pub_chrono.restart();

        float temp = tempsensor.readTempC();
        client.publish(temp_topic, String(temp).c_str());
        client.publish(humidity_topic, String(SHT21.getHumidity()).c_str());
    }

    delay(1);
}
