#ifndef ICE_h
#define ICE_h

#include <Arduino.h>
#include <Wire.h>

#define BASE_ADDRESS 0b1010

class ICE {
public:
    ICE(uint8_t A1 = 0, uint8_t A0 = 0);

    void write(uint32_t address, uint8_t data);
    void write16(uint32_t address, uint16_t data);
    uint8_t read(uint32_t address);
    uint16_t read16(uint32_t address);

private:
    uint8_t lowAddress, highAddress;
};

#endif

