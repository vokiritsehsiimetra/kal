#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>

class BMP280 {
public:
    BMP280(int i2c_bus, int address);
    ~BMP280();

    bool begin();
    float readTemperature();

private:
    int i2c_handle;
    int i2c_bus;
    int address;

    void writeRegister(uint8_t reg, uint8_t value);
    uint32_t read24(uint8_t reg);
};

#endif // BMP280_H
