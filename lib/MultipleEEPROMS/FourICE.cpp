#include "FourICE.h"

FourICE::FourICE() : ice0(0,0), ice1(0,1), ice2(1,0), ice3(1,1) {}

void FourICE::fromBeginning() {
    counter = 0;
}

uint32_t FourICE::getCounter() {
    return counter;
}

void FourICE::writeNext(uint8_t data) {
    switch (++counter / FourICE::max) { // 128 * 1024 = 131072
        case 0:
            ice0.write((uint32_t) (counter), data);
            break;
        case 1:
            ice1.write((uint32_t) (counter % 131072), data);
            break;
        case 2:
            ice2.write((uint32_t) (counter % 131072), data);
            break;
        case 3:
            ice3.write((uint32_t) (counter % 131072), data);
            break;
    }
}

uint8_t FourICE::readNext() {
    switch (++counter / 131072) { // 128 * 1024 = 131072
        case 0:
            return ice0.read(counter % 131072);
        case 1:
            return ice1.read(counter % 131072);
        case 2:
            return ice2.read(counter % 131072);
        case 3:
            return ice3.read(counter % 131072);
        default:
            return 0;
    }
}
