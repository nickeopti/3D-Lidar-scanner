#include "ICE.h"

ICE::ICE(uint8_t A1, uint8_t A0) {
    ICE::lowAddress = (BASE_ADDRESS << 3) | (A1 << 1) | A0;
    ICE::highAddress = (BASE_ADDRESS << 3) | (1 << 2) | (A1 << 1) | A0;
}

void ICE::write(uint32_t address, uint8_t data) {
    Wire.beginTransmission(address <= 0xFFFF ? lowAddress : highAddress);

    Wire.write((uint8_t) address >> 8); // MSB
    Wire.write((uint8_t) address & 0xFF); // LSB

    Wire.write(data);
    Wire.endTransmission();
}

void ICE::write16(uint32_t address, uint16_t data) {
    Wire.beginTransmission(address <= 0xFFFF ? lowAddress : highAddress);

    Wire.write((uint8_t) address >> 8); // MSB
    Wire.write((uint8_t) address & 0xFF); // LSB

    Wire.write(data >> 8); // MSB
    Wire.write(data & 0xFF); // LSB
    Wire.endTransmission();
}

uint8_t ICE::read(uint32_t address) {
    Wire.beginTransmission(address <= 0xFFFF ? lowAddress : highAddress);

    Wire.write((uint8_t) address >> 8); // MSB
    Wire.write((uint8_t) address & 0xFF); // LSB
    Wire.endTransmission();

    Wire.requestFrom(address <= 0xFFFF ? lowAddress : highAddress, (uint8_t) 1);
    return Wire.available() ? Wire.read() : 0;
}

uint16_t ICE::read16(uint32_t address) {
    Wire.beginTransmission(address <= 0xFFFF ? lowAddress : highAddress);

    Wire.write((uint8_t) address >> 8); // MSB
    Wire.write((uint8_t) address & 0xFF); // LSB
    Wire.endTransmission();

    Wire.requestFrom(address <= 0xFFFF ? lowAddress : highAddress, (uint8_t) 2);
    uint16_t data = 0;
    if (Wire.available())
        data = ((uint16_t) Wire.read()) << 8;
    if (Wire.available())
        data |= (uint16_t) Wire.read();

    return data;
}
