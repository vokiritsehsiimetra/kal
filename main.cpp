#include <iostream>
#include "bmp280.h"

int main() {
    BMP280 bmp280(1, 0x76);

    if (!bmp280.begin()) {
        std::cerr << "Failed to initialize BMP280" << std::endl;
        return 1;
    }

    float temperature = bmp280.readTemperature();
    std::cout << "Temperature: " << temperature << " °C" << std::endl;

    return 0;
}
