#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>
#include "Chrono.h"


#define STATE_PUB_INTERVAL 5000


const char ssid[] = "ssid";
const char pass[] = "pass";
const char heating_state_topic[] = "home/heating/state";
const char heating_cmd_topic[] = "home/heating/cmd";

WiFiClient net;
MQTTClient client;
Chrono state_pub_chrono;

bool state;


unsigned long last_sampled = 0;


void state_change(String &topic, String &payload) {
    Serial.print(topic); Serial.print(" -> "); Serial.println(payload);
    if(payload == "ON") {
        digitalWrite(D6, HIGH);
        client.publish(heating_state_topic, "ON");
        state = true;
    } else {
        digitalWrite(D6, LOW);
        client.publish(heating_state_topic, "OFF");
        state = false;
    }
}


void connect() {
  Serial.print("Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("connected!");

  Serial.print("Connecting to MQTT...");
  while (!client.connect("inside_temperature_switch", "inside_temperature", "inside_temperature")) {
    Serial.print(".");
    delay(1000);
  }


  client.subscribe(heating_cmd_topic);
  Serial.println("connected!");
}

void setup() {
    Serial.begin(115200);
    pinMode(D6, OUTPUT);

    WiFi.begin(ssid, pass);

    client.begin("192.168.1.109", net);
    client.onMessage(state_change);

    connect();
    last_sampled = millis();
    state = false;
}


void loop() {
    client.loop();

    if (!client.connected() || WiFi.status() != WL_CONNECTED) {
        connect();
    }

    if(state_pub_chrono.hasPassed(STATE_PUB_INTERVAL)) {
        state_pub_chrono.restart();

        client.publish(heating_state_topic, state ? "OF" : "OFF");
    }

    delay(1);
}
