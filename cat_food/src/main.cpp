#include <Arduino.h>
#include <Servo.h>

#define CONTROL_PIN 6

Servo servo;

int pos = 0;

void setup() {
    servo.attach(CONTROL_PIN);
    pos = 0;
}

void loop() {
    servo.write(180);
    delay(2000);
    servo.write(0);
    delay(2000);
}
