#ifndef FourICE_h
#define FourICE_h

#include <Arduino.h>
#include <Wire.h>
#include <ICE.h>

#define BASE_ADDRESS 0b1010

class FourICE {
public:
    FourICE();

    void fromBeginning(void);
    uint32_t getCounter(void);
    void writeNext(uint8_t data);
    uint8_t readNext();
private:
    ICE ice0, ice1, ice2, ice3;
    uint32_t counter;
    const uint32_t max = 131072L;
};

#endif
