#include <Arduino.h>
#include <Wire.h>
#include "Lidar.h"

char meassurementCounter = 0;

/****
SDA - blue wire  - A4
SCL - green wire - A5
****/

Lidar::Lidar(char lidarAddress) : address(lidarAddress) {}

void Lidar::begin(bool fasti2c) {
    Wire.begin(); // Start I2C
    if (fasti2c) {
#if ARDUINO >= 157
        Wire.setClock(400000UL); // Set I2C frequency to 400kHz, for Arduino Due
#else
        TWBR = ((F_CPU / 400000UL) - 16) / 2; // Set I2C frequency to 400kHz
#endif
    }

    //Configuration : Default range, higher speed short range
    write(0x02, 0x80); // Default
    write(0x04, 0x08); // Enable meassurement quick termination
    write(0x1c, 0x00); // Default
}

void Lidar::resetLidar() {
    write(0x00, 0x00);
}

int Lidar::getDistance() {
    // Correlate each every 100th meassurement
    boolean biasCorrection = meassurementCounter++ % 100 == 0;
    if (biasCorrection) {
        // Take acquisition & correlation processing with receiver bias correction
        write(0x00, 0x04);
    } else {
        // Take acquisition & correlation processing without receiver bias correction
        write(0x00, 0x03);
    }
    // Array to store high and low bytes of distance
    byte distanceArray[2];
    // Read two bytes from register 0x8f (autoincrement for reading 0x0f and 0x10)
    read(0x8f, 2, distanceArray, true);
    // Shift high byte and add to low byte
    int distance = (distanceArray[0] << 8) + distanceArray[1];

    return (distance);
}


void Lidar::write(char myAddress, char myValue) {
    Wire.beginTransmission(this->address);
    Wire.write(myAddress); // Set register for write
    Wire.write(myValue); // Write myValue to register

    // A nack means the device is not responding, report the error over serial
    int nackCatcher = Wire.endTransmission();
    if (nackCatcher != 0) {
        Serial.println(String("[Lidar::write] > nack: ")
                       + String(nackCatcher, HEX) + ", value="
                       + String(myValue, HEX));
    }

    delay(1); // 1 ms delay for robustness with successive reads and writes
}


void Lidar::read(char myAddress, int numOfBytes, byte arrayToSave[2], bool monitorBusyFlag) {

    int busyFlag = 0; // busyFlag monitors when the device is done with a measurement
    if (monitorBusyFlag) {
        busyFlag = 1; // Begin read immediately if not monitoring busy flag
    }
    int busyCounter = 0; // busyCounter counts number of times busy flag is checked, for timeout

    while (busyFlag != 0) { // Loop until device is not busy
        // Read status register to check busy flag
        Wire.beginTransmission(this->address);
        Wire.write(0x01); // Set the status register to be read

        // A nack means the device is not responding, report the error over serial
        int nackCatcher = Wire.endTransmission();
        if (nackCatcher != 0) {
            Serial.println("> nack");
        }

        Wire.requestFrom(this->address, 1); // Read register 0x01
        busyFlag = bitRead(Wire.read(), 0); // Assign the LSB of the status register to busyFlag

        busyCounter++; // Increment busyCounter for timeout

        // Handle timeout condition, exit while loop and goto bailout
        if (busyCounter > 9999) {
            goto bailout;
        }
    }

    // Device is not busy, begin read
    if (busyFlag == 0) {
        Wire.beginTransmission(this->address);
        Wire.write((int) myAddress); // Set the register to be read

        // A nack means the device is not responding, report the error over serial
        int nackCatcher = Wire.endTransmission();
        if (nackCatcher != 0) {
            Serial.println("> nack");
        }

        // Perform read of 1 or 2 bytes, save in arrayToSave
        Wire.requestFrom(this->address, numOfBytes);
        int i = 0;
        if (numOfBytes <= Wire.available()) {
            while (i < numOfBytes) {
                arrayToSave[i] = Wire.read();
                i++;
            }
        }
    }

    // bailout reports error over serial
    if (busyCounter > 9999) {
        bailout:
        busyCounter = 0;
        Serial.println("> read failed");
    }
}


