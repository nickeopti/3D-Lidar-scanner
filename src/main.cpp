#include <Arduino.h>
#include <Lidar.h>
#include <Servo.h>
#include <RF24.h>
#include <ICE.h>

#define LIDAR_INTERRUPT 2
#define HALL_SENSOR 3
#define SERVO 5
#define STEPPER 6
#define CE 9
#define CSN 10

Lidar lidar;
Servo servo;
RF24  radio(CE, CSN);
const byte address[6] = "2Nodf";

ICE ice(0,0);

void setup() {
    lidar.begin();
    servo.attach(SERVO);
    servo.write(10);

    pinMode(STEPPER, OUTPUT);
    pinMode(HALL_SENSOR, INPUT);

    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MAX);
    radio.stopListening();

    delay(5000);
}

void step(void) {
    digitalWrite(STEPPER, HIGH);
    delayMicroseconds(500);
    digitalWrite(STEPPER, LOW);
}

uint32_t pos = 10;
bool up = true;
void loop() {

    while (digitalRead(HALL_SENSOR)) {
        step();
        delay(25);
    }

    for (uint32_t i = 0; i < 400; i++) {
        step();

        uint16_t distance = lidar.getDistance();
        ice.write16(2*i+800*(pos-10), distance);
        radio.write(&distance, sizeof(distance));

        delay(25);
    }

    pos += up ? 1 : -1;
    if (pos == 90 || pos == 10)
        up ^= 1;
    servo.write(pos);

    if (pos == 90) {
        delay(15000);
        for (uint32_t i = 0; i < 72000L; i += 2) {
            uint16_t distance = ice.read16(i);
            radio.write(&distance, sizeof(distance));
            delay(2);
        }
    }
}
