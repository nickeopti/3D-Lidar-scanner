#ifndef Lidar_h
#define Lidar_h

#define LIDAR_ADDR_DEFAULT 0x62

#include <Arduino.h>

class Lidar {
public:
    Lidar(char lidarAddress = LIDAR_ADDR_DEFAULT);
    void begin(bool fasti2c = true);
    void resetLidar();
    int getDistance();

protected:
    void write(char myAddress, char myValue);
    void read(char myAddress, int numOfBytes, byte arrayToSave[2], bool monitorBusyFlag);

private:
    char address;
};

#endif

