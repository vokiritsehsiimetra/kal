#include <iostream>
#include "bmp280.h"

int main() {
    BMP280 bmp280(1, 0x76, "tcp://localhost:1883", "sensor/temperature");

    if (!bmp280.begin()) {
        std::cerr << "Failed to initialize BMP280" << std::endl;
        return 1;
    }

    bmp280.publishTemperature();

    return 0;
}
