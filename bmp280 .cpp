#include "bmp280.h"
#include <pigpio.h>
#include <iostream>
#include <unistd.h>

#define BMP280_REG_TEMP_XLSB 0xFC
#define BMP280_REG_TEMP_LSB  0xFB
#define BMP280_REG_TEMP_MSB  0xFA
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_CONFIG    0xF5

BMP280::BMP280(int i2c_bus, int address) : i2c_bus(i2c_bus), address(address), i2c_handle(-1) {}

BMP280::~BMP280() {
    if (i2c_handle >= 0) {
        i2cClose(i2c_handle);
    }
}

bool BMP280::begin() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio initialization failed" << std::endl;
        return false;
    }

    i2c_handle = i2cOpen(i2c_bus, address, 0);
    if (i2c_handle < 0) {
        std::cerr << "Failed to open I2C" << std::endl;
        return false;
    }

    // Configure the BMP280
    writeRegister(BMP280_REG_CTRL_MEAS, 0x27); // Normal mode, temp and pressure oversampling 1
    writeRegister(BMP280_REG_CONFIG, 0xA0);    // Standby time 1000ms

    sleep(1); // Wait for sensor to stabilize

    return true;
}

void BMP280::writeRegister(uint8_t reg, uint8_t value) {
    i2cWriteByteData(i2c_handle, reg, value);
}

uint32_t BMP280::read24(uint8_t reg) {
    uint32_t value = i2cReadByteData(i2c_handle, reg) << 16;
    value |= i2cReadByteData(i2c_handle, reg + 1) << 8;
    value |= i2cReadByteData(i2c_handle, reg + 2);
    return value;
}

float BMP280::readTemperature() {
    uint32_t raw_temp = read24(BMP280_REG_TEMP_MSB);
    raw_temp >>= 4;
    // Convert raw_temp to actual temperature using calibration data
    // This is a simplified example, you should use the calibration data from the sensor
    float temp = raw_temp / 16384.0 - 51200.0;
    return temp / 100.0;
}
